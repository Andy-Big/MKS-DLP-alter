/**
* Marlin 3D Printer Firmware
* Copyright (c) 2020 MarlinFirmware [https://github.com/MarlinFirmware/Marlin]
*
* Based on Sprinter and grbl.
* Copyright (c) 2011 Camiel Gubbels / Erik van der Zalm
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <https://www.gnu.org/licenses/>.
*
*/

/**
* planner.cpp
*
* Buffer movement commands and manage the acceleration profile plan
*
* Derived from Grbl
* Copyright (c) 2009-2011 Simen Svale Skogsrud
*
* The ring buffer implementation gleaned from the wiring_serial library by David A. Mellis.
*
*
* Reasoning behind the mathematics in this module (in the key of 'Mathematica'):
*
* s == speed, a == acceleration, t == time, d == distance
*
* Basic definitions:
*   Speed[s_, a_, t_] := s + (a*t)
*   Travel[s_, a_, t_] := Integrate[Speed[s, a, t], t]
*
* Distance to reach a specific speed with a constant acceleration:
*   Solve[{Speed[s, a, t] == m, Travel[s, a, t] == d}, d, t]
*   d -> (m^2 - s^2)/(2 a) --> estimate_acceleration_distance()
*
* Speed after a given distance of travel with constant acceleration:
*   Solve[{Speed[s, a, t] == m, Travel[s, a, t] == d}, m, t]
*   m -> Sqrt[2 a d + s^2]
*
* DestinationSpeed[s_, a_, d_] := Sqrt[2 a d + s^2]
*
* When to start braking (di) to reach a specified destination speed (s2) after accelerating
* from initial speed s1 without ever stopping at a plateau:
*   Solve[{DestinationSpeed[s1, a, di] == DestinationSpeed[s2, a, d - di]}, di]
*   di -> (2 a d - s1^2 + s2^2)/(4 a) --> intersection_distance()
*
* IntersectionDistance[s1_, s2_, a_, d_] := (2 a d - s1^2 + s2^2)/(4 a)
*
* --
*
* The fast inverse function needed for Bézier interpolation for AVR
* was designed, written and tested by Eduardo José Tagle on April/2018
*/

#include "z_planner.h"
#include "z_stepper.h"
#include "config.h"
#include "motor.h"





Planner			zPlanner;



// public:

/**
* A ring buffer of moves described in steps
*/
block_t Planner::block_buffer[BLOCK_BUFFER_SIZE];
volatile uint8_t Planner::block_buffer_head,    // Index of the next block to be pushed
Planner::block_buffer_nonbusy, // Index of the first non-busy block
Planner::block_buffer_planned, // Index of the optimally planned block
Planner::block_buffer_tail;    // Index of the busy block, if any
uint16_t Planner::cleaning_buffer_counter;      // A counter to disable queuing of blocks
uint8_t Planner::delay_before_delivering;       // This counter delays delivery of blocks when queue becomes empty to allow the opportunity of merging blocks

planner_settings_t Planner::settings;           // Initialized by settings.load()

uint32_t Planner::max_acceleration_steps_per_s2; // (steps/s^2) Derived from mm_per_s2

float Planner::steps_to_mm;             // (mm) Millimeters per step

// private:

int32_t Planner::position;

uint32_t Planner::cutoff_long;

float Planner::previous_speed;
float Planner::previous_nominal_speed_sqr;

/**
* Class and Instance Methods
*/

Planner::Planner()
{
	init();
}

void Planner::init()
{
	position = 0;
	previous_speed = 0;
	previous_nominal_speed_sqr = 0;
	clear_block_buffer();
	delay_before_delivering = 0;
}

#ifdef S_CURVE_ACCELERATION
// All other 32-bit MPUs can easily do inverse using hardware division,
// so we don't need to reduce precision or to use assembly language at all.
// This routine, for all other archs, returns 0x100000000 / d ~= 0xFFFFFFFF / d
static __INLINE uint32_t get_period_inverse(const uint32_t d)
{
	return d ? 0xFFFFFFFF / d : 0xFFFFFFFF;
}
#endif

#define MINIMAL_STEP_RATE 120

/**
* Get the current block for processing
* and mark the block as busy.
* Return nullptr if the buffer is empty
* or if there is a first-block delay.
*
* WARNING: Called from Stepper ISR context!
*/
block_t* Planner::get_current_block()
{
	// Get the number of moves in the planner queue so far
	const uint8_t nr_moves = movesplanned();
	
	// If there are any moves queued ...
	if (nr_moves)
	{
		
		// If there is still delay of delivery of blocks running, decrement it
		if (delay_before_delivering)
		{
			--delay_before_delivering;
			// If the number of movements queued is less than 3, and there is still time
			//  to wait, do not deliver anything
			if (nr_moves < 3 && delay_before_delivering) return nullptr;
			delay_before_delivering = 0;
		}
		
		// If we are here, there is no excuse to deliver the block
		block_t * const block = &block_buffer[block_buffer_tail];
		
		// No trapezoid calculated? Don't execute yet.
		if (TEST(block->flag, BLOCK_BIT_RECALCULATE)) return nullptr;
		
		// As this block is busy, advance the nonbusy block pointer
		block_buffer_nonbusy = next_block_index(block_buffer_tail);
		
		// Push block_buffer_planned pointer, if encountered.
		uint8_t tmp = block_buffer_head;
		if (tmp == block_buffer_tail)
			block_buffer_planned = block_buffer_nonbusy;
		
		// Return the block
		return block;
	}
	
	
	return nullptr;
}

/**
* Calculate trapezoid parameters, multiplying the entry- and exit-speeds
* by the provided factors.
**
* ############ VERY IMPORTANT ############
* NOTE that the PRECONDITION to call this function is that the block is
* NOT BUSY and it is marked as RECALCULATE. That WARRANTIES the Stepper ISR
* is not and will not use the block while we modify it, so it is safe to
* alter its values.
*/
void Planner::calculate_trapezoid_for_block(block_t* const block, const float &entry_factor, const float &exit_factor)
{
	
	uint32_t initial_rate = (uint32_t)CEIL(block->nominal_rate * entry_factor),
	final_rate = (uint32_t)CEIL(block->nominal_rate * exit_factor); // (steps per second)
	
	// Limit minimal step rate (Otherwise the timer will overflow.)
	NOLESS(initial_rate, uint32_t(MINIMAL_STEP_RATE));
	NOLESS(final_rate, uint32_t(MINIMAL_STEP_RATE));
	
#ifdef S_CURVE_ACCELERATION
	uint32_t cruise_rate = initial_rate;
#endif
	
	const int32_t accel = block->acceleration_steps_per_s2;
	
	// Steps required for acceleration, deceleration to/from nominal rate
	uint32_t accelerate_steps = (uint32_t)CEIL(estimate_acceleration_distance(initial_rate, block->nominal_rate, accel)),
	decelerate_steps = (uint32_t)FLOOR(estimate_acceleration_distance(block->nominal_rate, final_rate, -accel));
	// Steps between acceleration and deceleration, if any
	int32_t plateau_steps = block->step_event_count - accelerate_steps - decelerate_steps;
	
	// Does accelerate_steps + decelerate_steps exceed step_event_count?
	// Then we can't possibly reach the nominal rate, there will be no cruising.
	// Use intersection_distance() to calculate accel / braking time in order to
	// reach the final_rate exactly at the end of this block.
	if (plateau_steps < 0)
	{
		const float accelerate_steps_float = CEIL(intersection_distance(initial_rate, final_rate, accel, block->step_event_count));
		accelerate_steps = _MIN(uint32_t(_MAX(accelerate_steps_float, 0)), block->step_event_count);
		plateau_steps = 0;
		
#ifdef S_CURVE_ACCELERATION
		// We won't reach the cruising rate. Let's calculate the speed we will reach
		cruise_rate = final_speed(initial_rate, accel, accelerate_steps);
#endif
	}
#ifdef S_CURVE_ACCELERATION
	else // We have some plateau time, so the cruise rate will be the nominal rate
		cruise_rate = block->nominal_rate;
#endif
	
#ifdef S_CURVE_ACCELERATION
	// Jerk controlled speed requires to express speed versus time, NOT steps
	uint32_t acceleration_time = ((float)(cruise_rate - initial_rate) / accel) * (STEPPER_TIMER_RATE),
	deceleration_time = ((float)(cruise_rate - final_rate) / accel) * (STEPPER_TIMER_RATE),
	// And to offload calculations from the ISR, we also calculate the inverse of those times here
	acceleration_time_inverse = get_period_inverse(acceleration_time),
	deceleration_time_inverse = get_period_inverse(deceleration_time);
#endif
	
	// Store new block parameters
	block->accelerate_until = accelerate_steps;
	block->decelerate_after = accelerate_steps + plateau_steps;
	block->initial_rate = initial_rate;
#ifdef S_CURVE_ACCELERATION
	block->acceleration_time = acceleration_time;
	block->deceleration_time = deceleration_time;
	block->acceleration_time_inverse = acceleration_time_inverse;
	block->deceleration_time_inverse = deceleration_time_inverse;
	block->cruise_rate = cruise_rate;
#endif
	block->final_rate = final_rate;
	
	/**
	* Laser trapezoid calculations
	*
	* Approximate the trapezoid with the laser, incrementing the power every `entry_per` while accelerating
	* and decrementing it every `exit_power_per` while decelerating, thus ensuring power is related to feedrate.
	*
	* LASER_POWER_INLINE_TRAPEZOID_CONT doesn't need this as it continuously approximates
	*
	* Note this may behave unreliably when running with S_CURVE_ACCELERATION
	*/
}

/*                            PLANNER SPEED DEFINITION
						    +--------+   <- current->nominal_speed
						   /          \
current->entry_speed ->   +            \
						  |             + <- next->entry_speed (aka exit speed)
						  +-------------+
time -->

Recalculates the motion plan according to the following basic guidelines:

1. Go over every feasible block sequentially in reverse order and calculate the junction speeds
(i.e. current->entry_speed) such that:
a. No junction speed exceeds the pre-computed maximum junction speed limit or nominal speeds of
neighboring blocks.
b. A block entry speed cannot exceed one reverse-computed from its exit speed (next->entry_speed)
with a maximum allowable deceleration over the block travel distance.
c. The last (or newest appended) block is planned from a complete stop (an exit speed of zero).
2. Go over every block in chronological (forward) order and dial down junction speed values if
a. The exit speed exceeds the one forward-computed from its entry speed with the maximum allowable
acceleration over the block travel distance.

When these stages are complete, the planner will have maximized the velocity profiles throughout the all
of the planner blocks, where every block is operating at its maximum allowable acceleration limits. In
other words, for all of the blocks in the planner, the plan is optimal and no further speed improvements
are possible. If a new block is added to the buffer, the plan is recomputed according to the said
guidelines for a new optimal plan.

To increase computational efficiency of these guidelines, a set of planner block pointers have been
created to indicate stop-compute points for when the planner guidelines cannot logically make any further
changes or improvements to the plan when in normal operation and new blocks are streamed and added to the
planner buffer. For example, if a subset of sequential blocks in the planner have been planned and are
bracketed by junction velocities at their maximums (or by the first planner block as well), no new block
added to the planner buffer will alter the velocity profiles within them. So we no longer have to compute
them. Or, if a set of sequential blocks from the first block in the planner (or a optimal stop-compute
point) are all accelerating, they are all optimal and can not be altered by a new block added to the
planner buffer, as this will only further increase the plan speed to chronological blocks until a maximum
junction velocity is reached. However, if the operational conditions of the plan changes from infrequently
used feed holds or feedrate overrides, the stop-compute pointers will be reset and the entire plan is
recomputed as stated in the general guidelines.

Planner buffer index mapping:
- block_buffer_tail: Points to the beginning of the planner buffer. First to be executed or being executed.
- block_buffer_head: Points to the buffer block after the last block in the buffer. Used to indicate whether
the buffer is full or empty. As described for standard ring buffers, this block is always empty.
- block_buffer_planned: Points to the first buffer block after the last optimally planned block for normal
streaming operating conditions. Use for planning optimizations by avoiding recomputing parts of the
planner buffer that don't change with the addition of a new block, as describe above. In addition,
this block can never be less than block_buffer_tail and will always be pushed forward and maintain
this requirement when encountered by the Planner::release_current_block() routine during a cycle.

NOTE: Since the planner only computes on what's in the planner buffer, some motions with lots of short
line segments, like G2/3 arcs or complex curves, may seem to move slow. This is because there simply isn't
enough combined distance traveled in the entire buffer to accelerate up to the nominal speed and then
decelerate to a complete stop at the end of the buffer, as stated by the guidelines. If this happens and
becomes an annoyance, there are a few simple solutions: (1) Maximize the machine acceleration. The planner
will be able to compute higher velocity profiles within the same combined distance. (2) Maximize line
motion(s) distance per block to a desired tolerance. The more combined distance the planner has to use,
the faster it can go. (3) Maximize the planner buffer size. This also will increase the combined distance
for the planner to compute over. It also increases the number of computations the planner has to perform
to compute an optimal plan, so select carefully.
*/

// The kernel called by recalculate() when scanning the plan from last to first entry.
void Planner::reverse_pass_kernel(block_t* const current, const block_t * const next)
{
	if (current)
	{
		// If entry speed is already at the maximum entry speed, and there was no change of speed
		// in the next block, there is no need to recheck. Block is cruising and there is no need to
		// compute anything for this block,
		// If not, block entry speed needs to be recalculated to ensure maximum possible planned speed.
		const float max_entry_speed_sqr = current->max_entry_speed_sqr;
		
		// Compute maximum entry speed decelerating over the current block from its exit speed.
		// If not at the maximum entry speed, or the previous block entry speed changed
		if (current->entry_speed_sqr != max_entry_speed_sqr || (next && TEST(next->flag, BLOCK_BIT_RECALCULATE)))
		{
			
			// If nominal length true, max junction speed is guaranteed to be reached.
			// If a block can de/ac-celerate from nominal speed to zero within the length of the block, then
			// the current block and next block junction speeds are guaranteed to always be at their maximum
			// junction speeds in deceleration and acceleration, respectively. This is due to how the current
			// block nominal speed limits both the current and next maximum junction speeds. Hence, in both
			// the reverse and forward planners, the corresponding block junction speed will always be at the
			// the maximum junction speed and may always be ignored for any speed reduction checks.
			
			const float new_entry_speed_sqr = TEST(current->flag, BLOCK_BIT_NOMINAL_LENGTH)
				? max_entry_speed_sqr
					: _MIN(max_entry_speed_sqr, max_allowable_speed_sqr(-current->acceleration, next ? next->entry_speed_sqr : sq(float(MINIMUM_PLANNER_SPEED)), current->millimeters));
			if (current->entry_speed_sqr != new_entry_speed_sqr)
			{
				
				// Need to recalculate the block speed - Mark it now, so the stepper
				// ISR does not consume the block before being recalculated
				SBI(current->flag, BLOCK_BIT_RECALCULATE);
				
				// But there is an inherent race condition here, as the block may have
				// become BUSY just before being marked RECALCULATE, so check for that!
				if (zStepper.is_block_busy(current))
				{
					// Block became busy. Clear the RECALCULATE flag (no point in
					// recalculating BUSY blocks). And don't set its speed, as it can't
					// be updated at this time.
					CBI(current->flag, BLOCK_BIT_RECALCULATE);
				}
				else
				{
					// Block is not BUSY so this is ahead of the Stepper ISR:
					// Just Set the new entry speed.
					current->entry_speed_sqr = new_entry_speed_sqr;
				}
			}
		}
	}
}

/**
* recalculate() needs to go over the current plan twice.
* Once in reverse and once forward. This implements the reverse pass.
*/
void Planner::reverse_pass()
{
	// Initialize block index to the last block in the planner buffer.
	uint8_t block_index = prev_block_index(block_buffer_head);
	
	// Read the index of the last buffer planned block.
	// The ISR may change it so get a stable local copy.
	uint8_t planned_block_index = block_buffer_planned;
	
	// If there was a race condition and block_buffer_planned was incremented
	//  or was pointing at the head (queue empty) break loop now and avoid
	//  planning already consumed blocks
	if (planned_block_index == block_buffer_head) return;
	
	// Reverse Pass: Coarsely maximize all possible deceleration curves back-planning from the last
	// block in buffer. Cease planning when the last optimal planned or tail pointer is reached.
	// NOTE: Forward pass will later refine and correct the reverse pass to create an optimal plan.
	const block_t *next = nullptr;
	while (block_index != planned_block_index)
	{
		
		// Perform the reverse pass
		block_t *current = &block_buffer[block_index];
		
		// Only consider non sync and page blocks
		if (!TEST(current->flag, BLOCK_BIT_SYNC_POSITION))
		{
			reverse_pass_kernel(current, next);
			next = current;
		}
		
		// Advance to the next
		block_index = prev_block_index(block_index);
		
		// The ISR could advance the block_buffer_planned while we were doing the reverse pass.
		// We must try to avoid using an already consumed block as the last one - So follow
		// changes to the pointer and make sure to limit the loop to the currently busy block
		while (planned_block_index != block_buffer_planned)
		{
			
			// If we reached the busy block or an already processed block, break the loop now
			if (block_index == planned_block_index) return;
			
			// Advance the pointer, following the busy block
			planned_block_index = next_block_index(planned_block_index);
		}
	}
}

// The kernel called by recalculate() when scanning the plan from first to last entry.
void Planner::forward_pass_kernel(const block_t* const previous, block_t* const current, const uint8_t block_index)
{
	if (previous)
	{
		// If the previous block is an acceleration block, too short to complete the full speed
		// change, adjust the entry speed accordingly. Entry speeds have already been reset,
		// maximized, and reverse-planned. If nominal length is set, max junction speed is
		// guaranteed to be reached. No need to recheck.
		if (!TEST(previous->flag, BLOCK_BIT_NOMINAL_LENGTH) && previous->entry_speed_sqr < current->entry_speed_sqr)
		{
				
				// Compute the maximum allowable speed
				const float new_entry_speed_sqr = max_allowable_speed_sqr(-previous->acceleration, previous->entry_speed_sqr, previous->millimeters);
				
				// If true, current block is full-acceleration and we can move the planned pointer forward.
				if (new_entry_speed_sqr < current->entry_speed_sqr)
				{
					
					// Mark we need to recompute the trapezoidal shape, and do it now,
					// so the stepper ISR does not consume the block before being recalculated
					SBI(current->flag, BLOCK_BIT_RECALCULATE);
					
					// But there is an inherent race condition here, as the block maybe
					// became BUSY, just before it was marked as RECALCULATE, so check
					// if that is the case!
					if (zStepper.is_block_busy(current))
					{
						// Block became busy. Clear the RECALCULATE flag (no point in
						//  recalculating BUSY blocks and don't set its speed, as it can't
						//  be updated at this time.
						CBI(current->flag, BLOCK_BIT_RECALCULATE);
					}
					else
					{
						// Block is not BUSY, we won the race against the Stepper ISR:
						
						// Always <= max_entry_speed_sqr. Backward pass sets this.
						current->entry_speed_sqr = new_entry_speed_sqr; // Always <= max_entry_speed_sqr. Backward pass sets this.
						
						// Set optimal plan pointer.
						block_buffer_planned = block_index;
					}
				}
			}
		
		// Any block set at its maximum entry speed also creates an optimal plan up to this
		// point in the buffer. When the plan is bracketed by either the beginning of the
		// buffer and a maximum entry speed or two maximum entry speeds, every block in between
		// cannot logically be further improved. Hence, we don't have to recompute them anymore.
		if (current->entry_speed_sqr == current->max_entry_speed_sqr)
			block_buffer_planned = block_index;
	}
}

/**
* recalculate() needs to go over the current plan twice.
* Once in reverse and once forward. This implements the forward pass.
*/
void Planner::forward_pass()
{
	
	// Forward Pass: Forward plan the acceleration curve from the planned pointer onward.
	// Also scans for optimal plan breakpoints and appropriately updates the planned pointer.
	
	// Begin at buffer planned pointer. Note that block_buffer_planned can be modified
	//  by the stepper ISR,  so read it ONCE. It it guaranteed that block_buffer_planned
	//  will never lead head, so the loop is safe to execute. Also note that the forward
	//  pass will never modify the values at the tail.
	uint8_t block_index = block_buffer_planned;
	
	block_t *block;
	const block_t * previous = nullptr;
	while (block_index != block_buffer_head)
	{
		
		// Perform the forward pass
		block = &block_buffer[block_index];
		
		// Skip SYNC and page blocks
		if (!TEST(block->flag, BLOCK_BIT_SYNC_POSITION))
		{
			// If there's no previous block or the previous block is not
			// BUSY (thus, modifiable) run the forward_pass_kernel. Otherwise,
			// the previous block became BUSY, so assume the current block's
			// entry speed can't be altered (since that would also require
			// updating the exit speed of the previous block).
			if (!previous || !zStepper.is_block_busy(previous))
				forward_pass_kernel(previous, block, block_index);
			previous = block;
		}
		// Advance to the previous
		block_index = next_block_index(block_index);
	}
}

/**
* Recalculate the trapezoid speed profiles for all blocks in the plan
* according to the entry_factor for each junction. Must be called by
* recalculate() after updating the blocks.
*/
void Planner::recalculate_trapezoids()
{
	// The tail may be changed by the ISR so get a local copy.
	uint8_t block_index = block_buffer_tail,
	head_block_index = block_buffer_head;
	// Since there could be a sync block in the head of the queue, and the
	// next loop must not recalculate the head block (as it needs to be
	// specially handled), scan backwards to the first non-SYNC block.
	while (head_block_index != block_index)
	{
		
		// Go back (head always point to the first free block)
		const uint8_t prev_index = prev_block_index(head_block_index);
		
		// Get the pointer to the block
		block_t *prev = &block_buffer[prev_index];
		
		// If not dealing with a sync block, we are done. The last block is not a SYNC block
		if (!TEST(prev->flag, BLOCK_BIT_SYNC_POSITION)) break;
		
		// Examine the previous block. This and all following are SYNC blocks
		head_block_index = prev_index;
	}
	
	// Go from the tail (currently executed block) to the first block, without including it)
	block_t *block = nullptr, *next = nullptr;
	float current_entry_speed = 0.0, next_entry_speed = 0.0;
	while (block_index != head_block_index)
	{
		
		next = &block_buffer[block_index];
		
		// Skip sync and page blocks
		if (!TEST(next->flag, BLOCK_BIT_SYNC_POSITION))
		{
			next_entry_speed = SQRT(next->entry_speed_sqr);
			
			if (block)
			{
				// Recalculate if current block entry or exit junction speed has changed.
				if (TEST(block->flag, BLOCK_BIT_RECALCULATE) || TEST(next->flag, BLOCK_BIT_RECALCULATE))
				{
					
					// Mark the current block as RECALCULATE, to protect it from the Stepper ISR running it.
					// Note that due to the above condition, there's a chance the current block isn't marked as
					// RECALCULATE yet, but the next one is. That's the reason for the following line.
					SBI(block->flag, BLOCK_BIT_RECALCULATE);
					
					// But there is an inherent race condition here, as the block maybe
					// became BUSY, just before it was marked as RECALCULATE, so check
					// if that is the case!
					if (!zStepper.is_block_busy(block)) {
						// Block is not BUSY, we won the race against the Stepper ISR:
						
						// NOTE: Entry and exit factors always > 0 by all previous logic operations.
						const float current_nominal_speed = SQRT(block->nominal_speed_sqr),
						nomr = 1.0f / current_nominal_speed;
						calculate_trapezoid_for_block(block, current_entry_speed * nomr, next_entry_speed * nomr);
					}
					
					// Reset current only to ensure next trapezoid is computed - The
					// stepper is free to use the block from now on.
					CBI(block->flag, BLOCK_BIT_RECALCULATE);
				}
			}
			
			block = next;
			current_entry_speed = next_entry_speed;
		}
		
		block_index = next_block_index(block_index);
	}
	
	// Last/newest block in buffer. Exit speed is set with MINIMUM_PLANNER_SPEED. Always recalculated.
	if (next)
	{
		
		// Mark the next(last) block as RECALCULATE, to prevent the Stepper ISR running it.
		// As the last block is always recalculated here, there is a chance the block isn't
		// marked as RECALCULATE yet. That's the reason for the following line.
		SBI(next->flag, BLOCK_BIT_RECALCULATE);
		
		// But there is an inherent race condition here, as the block maybe
		// became BUSY, just before it was marked as RECALCULATE, so check
		// if that is the case!
		if (!zStepper.is_block_busy(block))
		{
			// Block is not BUSY, we won the race against the Stepper ISR:
			
			const float next_nominal_speed = SQRT(next->nominal_speed_sqr),
			nomr = 1.0f / next_nominal_speed;
			calculate_trapezoid_for_block(next, next_entry_speed * nomr, float(MINIMUM_PLANNER_SPEED) * nomr);
		}
		
		// Reset next only to ensure its trapezoid is computed - The stepper is free to use
		// the block from now on.
		CBI(next->flag, BLOCK_BIT_RECALCULATE);
	}
}

void Planner::recalculate()
{
	// Initialize block index to the last block in the planner buffer.
	const uint8_t block_index = prev_block_index(block_buffer_head);
	// If there is just one block, no planning can be done. Avoid it!
	if (block_index != block_buffer_planned)
	{
		reverse_pass();
		forward_pass();
	}
	recalculate_trapezoids();
}

void Planner::quick_stop()
{
	
	// Remove all the queued blocks. Note that this function is NOT
	// called from the Stepper ISR, so we must consider tail as readonly!
	// that is why we set head to tail - But there is a race condition that
	// must be handled: The tail could change between the read and the assignment
	// so this must be enclosed in a critical section
	
	const bool was_enabled = zStepper.suspend();
	
	// Drop all queue entries
	block_buffer_nonbusy = block_buffer_planned = block_buffer_head = block_buffer_tail;
	
	// Restart the block delay for the first movement - As the queue was
	// forced to empty, there's no risk the ISR will touch this.
	delay_before_delivering = BLOCK_DELAY_FOR_1ST_MOVE;
	
	// Make sure to drop any attempt of queuing moves for 1 second
	cleaning_buffer_counter = 0;
	
	// Reenable Stepper ISR
	if (was_enabled)
		zStepper.wake_up();
	
	// And stop the stepper ISR
	zStepper.quick_stop();
}

void Planner::endstop_triggered()
{
	// Record stepper position and discard the current block
	zStepper.endstop_triggered();
}

float Planner::triggered_position_mm()
{
	return zStepper.triggered_position() * steps_to_mm;
}

void Planner::finish_and_disable()
{
	while (has_blocks_queued() || cleaning_buffer_counter) idle();
	Z_ENA_GPIO_Port->BSRR = Z_ENA_Pin;
}

/**
* Get an axis position according to stepper position(s)
* For CORE machines apply translation from ABC to XYZ.
*/
float Planner::get_axis_position_mm()
{
	float axis_steps;
	axis_steps = zStepper.position();
	return axis_steps * steps_to_mm;
}

/**
* Block until all buffered steps are executed / cleaned
*/
void Planner::synchronize()
{
	while (has_blocks_queued() || cleaning_buffer_counter) idle();
}

/**
* Planner::_buffer_steps
*
* Add a new linear movement to the planner queue (in terms of steps).
*
*  target        - target position in steps units
*  target_float  - target position in direct (mm, degrees) units. optional
*  fr_mm_s       - (target) speed of the move
*  extruder      - target extruder
*  millimeters   - the length of the movement, if known
*
* Returns true if movement was properly queued, false otherwise (if cleaning)
*/
bool Planner::_buffer_steps(const int32_t target, float fr_mm_s, const float millimeters)
{
	
	// If we are cleaning, do not accept queuing of movements
	if (cleaning_buffer_counter)
		return false;
	
	// Wait for the next available block
	uint8_t next_buffer_head;
	block_t * const block = get_next_free_block(next_buffer_head);
	
	// Fill the block with the specified movement
	if (!_populate_block(block, false, target, fr_mm_s, millimeters))
	{
		// Movement was not queued, probably because it was too short.
		//  Simply accept that as movement queued and done
		return true;
	}
	
	// If this is the first added movement, reload the delay, otherwise, cancel it.
	uint8_t tmp = block_buffer_head;
	if (tmp == block_buffer_tail)
	{
		// If it was the first queued block, restart the 1st block delivery delay, to
		// give the planner an opportunity to queue more movements and plan them
		// As there are no queued movements, the Stepper ISR will not touch this
		// variable, so there is no risk setting this here (but it MUST be done
		// before the following line!!)
		delay_before_delivering = BLOCK_DELAY_FOR_1ST_MOVE;
	}
	
	// Move buffer head
	block_buffer_head = next_buffer_head;
	
	// Recalculate and optimize trapezoidal speed profiles
	recalculate();
	
	// Movement successfully queued!
	return true;
}

/**
* Planner::_populate_block
*
* Fills a new linear movement in the block (in terms of steps).
*
*  target      - target position in steps units
*  fr_mm_s     - (target) speed of the move
*  extruder    - target extruder
*
* Returns true if movement is acceptable, false otherwise
*/
bool Planner::_populate_block(block_t * const block, bool split_move, const int32_t target, float fr_mm_s, const float millimeters)
{
	
	const int32_t dc = target - position;
	
	
	// Compute direction bit-mask for this block
	uint8_t dm = 0;
	if (dc < 0)
		dm = 1;
	
	// Clear all flags, including the "busy" bit
	block->flag = 0x00;
	
	// Set direction bits
	block->direction_bits = dm;
	
	block->steps = (ABS(dc));
	
	/**
	* This part of the code calculates the total length of the movement.
	* For cartesian bots, the X_AXIS is the real X movement and same for Y_AXIS.
	* But for corexy bots, that is not true. The "X_AXIS" and "Y_AXIS" motors (that should be named to A_AXIS
	* and B_AXIS) cannot be used for X and Y length, because A=X+Y and B=X-Y.
	* So we need to create other 2 "AXIS", named X_HEAD and Y_HEAD, meaning the real displacement of the Head.
	* Having the real displacement of the head, we can calculate the total movement length and apply the desired speed.
	*/
	float steps_dist_mm = dc * steps_to_mm;
	
	
	if (block->steps < MIN_STEPS_PER_SEGMENT)
	{
		block->millimeters = (0);
	}
	else
	{
		if (millimeters)
			block->millimeters = millimeters;
		else
			block->millimeters = SQRT(sq(steps_dist_mm));
	}
	
	block->step_event_count = block->steps;
	
	// Bail if this is a zero-length block
	if (block->step_event_count < MIN_STEPS_PER_SEGMENT)
		return false;
	
	// Enable active axes
	if (block->steps)
		// enable pin
		Z_ENA_GPIO_Port->BSRR = (uint32_t)Z_ENA_Pin << 16U;
	
	const float inverse_millimeters = 1.0f / block->millimeters;  // Inverse millimeters to remove multiple divides
	
	// Calculate inverse time for this move. No divide by zero due to previous checks.
	// Example: At 120mm/s a 60mm move takes 0.5s. So this will give 2.0.
	float inverse_secs = fr_mm_s * inverse_millimeters;
	
	// Get the number of non busy movements in queue (non busy means that they can be altered)
	const uint8_t moves_queued = nonbusy_movesplanned();
	
	// Slow down when the buffer starts to empty, rather than wait at the corner for a buffer refill
	
	block->nominal_speed_sqr = sq(block->millimeters * inverse_secs);   // (mm/sec)^2 Always > 0
	block->nominal_rate = (uint32_t)CEIL(block->step_event_count * inverse_secs); // (step/sec) Always > 0
	
	// Calculate and limit speed in mm/sec
	
	float current_speed;
	float speed_factor = 1.0f; // factor <1 decreases speed
	
	// Linear axes first with less logic
	current_speed = steps_dist_mm * inverse_secs;
	const float cs = ABS(current_speed),  max_fr = settings.max_feedrate_mm_s;
	if (cs > max_fr)
		NOMORE(speed_factor, max_fr / cs);
	
	// Correct the speed
	if (speed_factor < 1.0f)
	{
		current_speed *= speed_factor;
		block->nominal_rate *= speed_factor;
		block->nominal_speed_sqr = block->nominal_speed_sqr * sq(speed_factor);
	}
	
	// Compute and limit the acceleration rate for the trapezoid generator.
	const float steps_per_mm = block->step_event_count * inverse_millimeters;
	uint32_t accel;
	if (!block->steps)
	{
		// convert to: acceleration steps/sec^2
		accel = (uint32_t)CEIL(settings.retract_acceleration * steps_per_mm);
	}
	else
	{

		// Start with print or travel acceleration
		accel = (uint32_t)CEIL((systemInfo.print_is_printing ? settings.acceleration : settings.travel_acceleration) * steps_per_mm);

		// Limit acceleration per axis
		if (block->step_event_count <= cutoff_long)
		{
				if (block->steps && max_acceleration_steps_per_s2 < accel)
				{
					const uint32_t comp = max_acceleration_steps_per_s2 * block->step_event_count;
					if (accel * block->steps > comp)
						accel = comp / block->steps;
				}
		}
		else
		{
			if (block->steps && max_acceleration_steps_per_s2 < accel)
			{
				const float comp = (float)max_acceleration_steps_per_s2 * (float)block->step_event_count;
				if ((float)accel * (float)block->steps > comp)
					accel = (uint32_t)(comp / (float)block->steps);
			}
		}
	}
	block->acceleration_steps_per_s2 = accel;
	block->acceleration = accel / steps_per_mm;
	#ifndef S_CURVE_ACCELERATION
	block->acceleration_rate = (uint32_t)(accel * (4096.0f * 4096.0f / (STEPPER_TIMER_RATE)));
	#endif

	float vmax_junction_sqr; // Initial limit on the segment entry velocity (mm/s)^2


	#ifdef USE_CACHED_SQRT
	#define CACHED_SQRT(N, V) \
	static float saved_V, N; \
		if (V != saved_V) { N = SQRT(V); saved_V = V; }
	#else
	#define CACHED_SQRT(N, V) const float N = SQRT(V)
	#endif
	
	/**
	* Adapted from Průša MKS firmware
	* https://github.com/prusa3d/Prusa-Firmware
	*/
	CACHED_SQRT(nominal_speed, block->nominal_speed_sqr);
	
	// Exit speed limited by a jerk to full halt of a previous last segment
	static float previous_safe_speed;
	
	// Start with a safe speed (from which the machine may halt to stop immediately).
	float safe_speed = nominal_speed;
	
	uint8_t limited = 0;
	const float jerk = ABS(current_speed),   // cs : Starting from zero, change in speed for this axis
	maxj = settings.max_jerk; // mj : The max jerk setting for this axis
	if (jerk > maxj)
	{                          // cs > mj : New current speed too fast?
		if (limited)
		{                            // limited already?
			const float mjerk = nominal_speed * maxj; // ns*mj
			if (jerk * safe_speed > mjerk)
				safe_speed = mjerk / jerk; // ns*mj/cs
		}
		else
		{
			safe_speed *= maxj / jerk;              // Initial limit: ns*mj/cs
			++limited;                              // Initially limited
		}
	}
	
	float vmax_junction;
	if (moves_queued && !UNEAR_ZERO(previous_nominal_speed_sqr))
	{
		// Estimate a maximum velocity allowed at a joint of two successive segments.
		// If this maximum velocity allowed is lower than the minimum of the entry / exit safe velocities,
		// then the machine is not coasting anymore and the safe entry / exit velocities shall be used.
		
		// Factor to multiply the previous / current nominal velocities to get componentwise limited velocities.
		float v_factor = 1;
		limited = 0;
		
		// The junction velocity will be shared between successive segments. Limit the junction velocity to their minimum.
		// Pick the smaller of the nominal speeds. Higher speed shall not be achieved at the junction during coasting.
		CACHED_SQRT(previous_nominal_speed, previous_nominal_speed_sqr);
		
		float smaller_speed_factor = 1.0f;
		if (nominal_speed < previous_nominal_speed)
		{
			vmax_junction = nominal_speed;
			smaller_speed_factor = vmax_junction / previous_nominal_speed;
		}
		else
			vmax_junction = previous_nominal_speed;
		
		// Limit an axis. We have to differentiate: coasting, reversal of an axis, full stop.
		float v_exit = previous_speed * smaller_speed_factor,
		v_entry = current_speed;
		if (limited)
		{
			v_exit *= v_factor;
			v_entry *= v_factor;
		}
		
		// Calculate jerk depending on whether the axis is coasting in the same direction or reversing.
		const float jerk = (v_exit > v_entry)
			? //                                  coasting             axis reversal
				( (v_entry > 0 || v_exit < 0) ? (v_exit - v_entry) : _MAX(v_exit, -v_entry) )
					: // v_exit <= v_entry                coasting             axis reversal
		( (v_entry < 0 || v_exit > 0) ? (v_entry - v_exit) : _MAX(-v_exit, v_entry) );
		
		const float maxj = settings.max_jerk;
		
		if (jerk > maxj)
		{
			v_factor *= maxj / jerk;
			++limited;
		}
		
		if ((v_entry < 0 && v_exit > 0) || (v_entry > 0 && v_exit < 0))		// move direction changed
			vmax_junction *= v_factor;
		
		//		if (limited) vmax_junction *= v_factor;
		
		// Now the transition velocity is known, which maximizes the shared exit / entry velocity while
		// respecting the jerk factors, it may be possible, that applying separate safe exit / entry velocities will achieve faster prints.
		const float vmax_junction_threshold = vmax_junction * 0.99f;
		if (previous_safe_speed > vmax_junction_threshold && safe_speed > vmax_junction_threshold)
			vmax_junction = safe_speed;
	}
	else
		vmax_junction = safe_speed;
	
	previous_safe_speed = safe_speed;
	
	vmax_junction_sqr = sq(vmax_junction);          // Go up or down to the new speed
	
	
	// Max entry speed of this block equals the max exit speed of the previous block.
	block->max_entry_speed_sqr = vmax_junction_sqr;
	
	// Initialize block entry speed. Compute based on deceleration to user-defined MINIMUM_PLANNER_SPEED.
	const float v_allowable_sqr = max_allowable_speed_sqr(-block->acceleration, sq(float(MINIMUM_PLANNER_SPEED)), block->millimeters);
	
	// If we are trying to add a split block, start with the
	// max. allowed speed to avoid an interrupted first move.
	block->entry_speed_sqr = !split_move ? sq(float(MINIMUM_PLANNER_SPEED)) : _MIN(vmax_junction_sqr, v_allowable_sqr);
	
	// Initialize planner efficiency flags
	// Set flag if block will always reach maximum junction speed regardless of entry/exit speeds.
	// If a block can de/ac-celerate from nominal speed to zero within the length of the block, then
	// the current block and next block junction speeds are guaranteed to always be at their maximum
	// junction speeds in deceleration and acceleration, respectively. This is due to how the current
	// block nominal speed limits both the current and next maximum junction speeds. Hence, in both
	// the reverse and forward planners, the corresponding block junction speed will always be at the
	// the maximum junction speed and may always be ignored for any speed reduction checks.
	block->flag |= block->nominal_speed_sqr <= v_allowable_sqr ? BLOCK_FLAG_RECALCULATE | BLOCK_FLAG_NOMINAL_LENGTH : BLOCK_FLAG_RECALCULATE;
	
	// Update previous path unit_vector and nominal speed
	previous_speed = current_speed;
	previous_nominal_speed_sqr = block->nominal_speed_sqr;
	
	position = target;  // Update the position
	
	
	return true;        // Movement was accepted
	
} // _populate_block()

/**
* Planner::buffer_sync_block
* Add a block to the buffer that just updates the position
*/
void Planner::buffer_sync_block()
{
	// Wait for the next available block
	uint8_t next_buffer_head;
	block_t * const block = get_next_free_block(next_buffer_head);
	
	// Clear block
	memset(block, 0, sizeof(block_t));
	
	block->flag = BLOCK_FLAG_SYNC_POSITION;
	
	block->position = position;
	
	// If this is the first added movement, reload the delay, otherwise, cancel it.
	uint8_t tmp = block_buffer_head;
	if (tmp == block_buffer_tail)
	{
		// If it was the first queued block, restart the 1st block delivery delay, to
		// give the planner an opportunity to queue more movements and plan them
		// As there are no queued movements, the Stepper ISR will not touch this
		// variable, so there is no risk setting this here (but it MUST be done
		// before the following line!!)
		delay_before_delivering = BLOCK_DELAY_FOR_1ST_MOVE;
	}
	
	block_buffer_head = next_buffer_head;
	
	zStepper.wake_up();
} // buffer_sync_block()

/**
* Planner::buffer_segment
*
* Add a new linear movement to the buffer in axis units.
*
* Leveling and kinematics should be applied ahead of calling this.
*
*  a,b,c,e     - target positions in mm and/or degrees
*  fr_mm_s     - (target) speed of the move
*  extruder    - target extruder
*  millimeters - the length of the movement, if known
*
* Return 'false' if no segment was queued due to cleaning, cold extrusion, full queue, etc.
*/
bool Planner::buffer_segment(const float c, const float fr_mm_s, const float &millimeters)
{
	
	// If we are cleaning, do not accept queuing of movements
	if (cleaning_buffer_counter)
		return false;
	
	// The target position of the tool in absolute steps
	// Calculate target position in absolute steps
	const int32_t target = int32_t(LROUND(c * settings.axis_steps_per_mm));
	
	// Queue the movement. Return 'false' if the move was not queued.
	if (!_buffer_steps(target, fr_mm_s, millimeters))
		return false;
	
	zStepper.wake_up();
	return true;
} // buffer_segment()

/**
* Add a new linear movement to the buffer.
* The target is cartesian. It's translated to
* delta/scara if needed.
*
*  rx,ry,rz,e   - target position in mm or degrees
*  fr_mm_s      - (target) speed of the move (mm/s)
*  extruder     - target extruder
*  millimeters  - the length of the movement, if known
*  inv_duration - the reciprocal if the duration of the movement, if known (kinematic only if feeedrate scaling is enabled)
*/
bool Planner::buffer_line(const float rz, const float fr_mm_s, const float millimeters)
{
	float machine = rz;
	
	return buffer_segment(machine, fr_mm_s, millimeters);
} // buffer_line()

/**
* Directly set the planner ABC position (and stepper positions)
* converting mm (or angles for SCARA) into steps.
*
* The provided ABC position is in machine units.
*/

void Planner::set_machine_position_mm(float c)
{
	position = (LROUND(c * settings.axis_steps_per_mm));
	if (has_blocks_queued())
	{
		//previous_nominal_speed_sqr = 0.0; // Reset planner junction speeds. Assume start from rest.
		//previous_speed.reset();
		buffer_sync_block();
	}
	else
		zStepper.set_position(position);
}

void Planner::set_position_mm(const float rz)
{
	float machine = rz;
	set_machine_position_mm(machine);
}

// Recalculate the steps/s^2 acceleration rates, based on the mm/s^2
void Planner::reset_acceleration_rates()
{
	uint32_t highest_rate = 1;
	max_acceleration_steps_per_s2 = (uint32_t)(settings.max_acceleration_mm_per_s2 * settings.axis_steps_per_mm);
	NOLESS(highest_rate, max_acceleration_steps_per_s2);
	cutoff_long = 4294967295UL / highest_rate; // 0xFFFFFFFFUL
}

// Recalculate position, steps_to_mm if settings.axis_steps_per_mm changes!
void Planner::refresh_positioning()
{
	steps_to_mm = 1.0f / settings.axis_steps_per_mm;
	set_position_mm((float)systemInfo.current_position_steps * steps_to_mm);
	reset_acceleration_rates();
}

void Planner::set_max_acceleration(float targetValue)
{
	settings.max_acceleration_mm_per_s2 = targetValue;
	
	// Update steps per s2 to agree with the units per s2 (since they are used in the planner)
	reset_acceleration_rates();
}

void Planner::set_max_feedrate(float targetValue)
{
	settings.max_feedrate_mm_s = targetValue;
}

void Planner::set_max_jerk(float targetValue)
{
	settings.max_jerk = targetValue;
}

