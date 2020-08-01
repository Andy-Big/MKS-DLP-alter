/**
 * Marlin 3D Printer Firmware
 * Copyright (C) 2016 MarlinFirmware [https://github.com/MarlinFirmware/Marlin]
 *
 * Based on Sprinter and grbl.
 * Copyright (C) 2011 Camiel Gubbels / Erik van der Zalm
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
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

/**
 * planner.h
 *
 * Buffer movement commands and manage the acceleration profile plan
 *
 * Derived from Grbl
 * Copyright (c) 2009-2011 Simen Svale Skogsrud
 */

#ifndef PLANNER_H
#define PLANNER_H

//#include "types.h"
//#include "enum.h"

#include <math.h>

#include "main.h"


#define _BV(bit)			(1<<(bit))
#define TEST(n,b)			(((n)&_BV(b))!=0)
#define SBI(n,b)			(n |= _BV(b))
#define CBI(n,b)			(n &= ~_BV(b))
#define __SET_BIT(n,b,value) (n) ^= ((-value)^(n)) & (_BV(b))	/*--mks--*/


#define NOLESS(v,n)			do{ if (v < n) v = n; }while(0)
#define NOMORE(v,n)			do{ if (v > n) v = n; }while(0)

#define sq(x)				pow(x,2)
//#define ATAN2(y, x)			atan2(y, x)
#define FABS(x)				fabs(x)
#define POW(x, y)			pow(x, y)
#define SQRT(x)				sqrt(x)
#define CEIL(x)				ceil(x)
#define FLOOR(x)			floor(x)
#define min(a,b)			((a)<(b)?(a):(b))
#define max(a,b)			(((a)>(b))?(a):(b))
#define LROUND(x)			lround(x)
//#define FMOD(x, y)			fmod(x, y)
//#define HYPOT(x,y)			SQRT(HYPOT2(x,y))

#define CRITICAL_SECTION_START		cli();
#define CRITICAL_SECTION_END		sei();
#define cli()				__ASM volatile("cpsid i")	//__disable_irq()
#define sei()				__ASM volatile("cpsie i")	//__enable_irq()
#define ZERO(a)				memset(a,0,sizeof(a))
#define COUNT(a)			(sizeof(a)/sizeof(*a))
#define UNEAR_ZERO(x)		((x) < 0.000001)




#define F_CPU()						168000000
#define BLOCK_BUFFER_SIZE			16 // maximize block buffer
#define MAX_STEP_FREQUENCY			40000 // Max step frequency for Ultimaker (5000 pps / half step)
#define MINIMUM_PLANNER_SPEED		0.01 // (mm/sec)
#define MIN_STEPS_PER_SEGMENT		4


class Planner;

extern Planner		zPlanner;


enum BlockFlagBit
{
	// Recalculate trapezoids on entry junction. For optimization.
	BLOCK_BIT_RECALCULATE,

	// Nominal speed always reached.
	// i.e., The segment is long enough, so the nominal speed is reachable if accelerating
	// from a safe speed (in consideration of jerking from zero speed).
	BLOCK_BIT_NOMINAL_LENGTH,

	// Start from a halt at the start of this block, respecting the maximum allowed jerk.
	BLOCK_BIT_START_FROM_FULL_HALT,

	// The block is busy
	BLOCK_BIT_BUSY,

	// The block is segment 2+ of a longer move
	BLOCK_BIT_CONTINUED
};

enum BlockFlag
{
	BLOCK_FLAG_RECALCULATE          = (1 << BLOCK_BIT_RECALCULATE),
	BLOCK_FLAG_NOMINAL_LENGTH       = (1 << BLOCK_BIT_NOMINAL_LENGTH),
	BLOCK_FLAG_START_FROM_FULL_HALT = (1 << BLOCK_BIT_START_FROM_FULL_HALT),
	BLOCK_FLAG_BUSY                 = (1 << BLOCK_BIT_BUSY),
	BLOCK_FLAG_CONTINUED            = (1 << BLOCK_BIT_CONTINUED)
};

/**
 * struct block_t
 *
 * A single entry in the planner buffer.
 * Tracks linear movement over multiple axes.
 *
 * The "nominal" values are as-specified by gcode, and
 * may never actually be reached due to acceleration limits.
 */
typedef struct
{

	uint8_t flag;                             // Block flags (See BlockFlag enum above)

	// Fields used by the Bresenham algorithm for tracing the line
	int32_t steps;                  // Step count along each axis
	uint32_t step_event_count;                // The number of step events required to complete this block

	int32_t accelerate_until,                 // The index of the step event on which to stop acceleration
		  decelerate_after,                 // The index of the step event on which to start decelerating
		  acceleration_rate;                // The acceleration rate used for acceleration calculation

	uint8_t direction_bits;                   // The direction bit set for this block (refers to *_DIRECTION_BIT in config.h)

	// Fields used by the motion planner to manage acceleration
	float nominal_speed,                      // The nominal speed for this block in mm/sec
		entry_speed,                        // Entry speed at previous-current junction in mm/sec
		max_entry_speed,                    // Maximum allowable junction entry speed in mm/sec
		millimeters,                        // The total travel of this block in mm
		acceleration;                       // acceleration mm/sec^2

	// Settings for the trapezoid generator
	uint32_t nominal_rate,                    // The nominal step rate for this block in step_events/sec
		   initial_rate,                    // The jerk-adjusted step rate at start of block
		   final_rate,                      // The minimal rate at exit
		   acceleration_steps_per_s2;       // acceleration steps/sec^2

	uint32_t segment_time_us;

} block_t;

#define BLOCK_MOD(n) ((n)&(BLOCK_BUFFER_SIZE-1))

class Planner
{
	public:

	/**
	 * The move buffer, calculated in stepper steps
	 *
	 * block_buffer is a ring buffer...
	 *
	 *             head,tail : indexes for write,read
	 *            head==tail : the buffer is empty
	 *            head!=tail : blocks are in the buffer
	 *   head==(tail-1)%size : the buffer is full
	 *
	 *  Writer of head is Planner::buffer_segment().
	 *  Reader of tail is Stepper::isr(). Always consider tail busy / read-only
	 */
	static block_t block_buffer[BLOCK_BUFFER_SIZE];
	static volatile uint8_t block_buffer_head,      // Index of the next block to be pushed
							block_buffer_tail;      // Index of the busy block, if any

	static float max_feedrate_mm_s,         // Max speeds in mm per second
				 axis_steps_per_mm,
				 steps_to_mm;
	static unsigned long max_acceleration_steps_per_s2,
					max_acceleration_mm_per_s2; // Use M201 to override

	static uint32_t min_segment_time_us; // Use 'M205 B<µs>' to override
	static float min_feedrate_mm_s,
				 acceleration,         // Normal acceleration mm/s^2  DEFAULT ACCELERATION for all printing moves. M204 SXXXX
				 travel_acceleration,  // Travel acceleration mm/s^2  DEFAULT ACCELERATION for all NON printing moves. M204 MXXXX
				 max_jerk,       // The largest speed change requiring no acceleration
				 min_travel_feedrate_mm_s;


	private:

	/**
	 * The current position of the tool in absolute steps
	 * Recalculated if any axis_steps_per_mm are changed by gcode
	 */
	static int32_t position;

	/**
	 * Speed of previous path line segment
	 */
	static float previous_speed;

	/**
	 * Nominal speed of previous path line segment
	 */
	static float previous_nominal_speed;

	/**
	 * Limit where 64bit math is necessary for acceleration calculation
	 */
	static uint32_t cutoff_long;

	public:

	/**
	 * Instance Methods
	 */

	Planner();

	void init();

	/**
	 * Static (class) Methods
	 */

	static void reset_acceleration_rates();
	static void refresh_positioning();

	// Manage fans, paste pressure, etc.
	static void check_axes_activity();

	/**
	 * Number of moves currently in the planner
	 */
	__INLINE static uint8_t movesplanned()
	{
		register uint8_t tmp = block_buffer_head;
		tmp -= block_buffer_tail;
		return BLOCK_MOD(tmp + BLOCK_BUFFER_SIZE);
	}

	__INLINE static bool is_full()
	{
		return block_buffer_tail == next_block_index(block_buffer_head);
	}

	// Update multipliers based on new diameter measurements
	static void calculate_volumetric_multipliers();


	__INLINE static float fade_scaling_factor_for_z(const float &rz)
	{
		UNUSED(rz);
		return 1.0;
	}

	__INLINE static bool leveling_active_at_z(const float &rz)
	{
		UNUSED(rz);
		return true;
	}


	/**
	 * Planner::_buffer_steps
	 *
	 * Add a new linear movement to the buffer (in terms of steps).
	 *
	 *  target      - target position in steps units
	 *  fr_mm_s     - (target) speed of the move
	 *  extruder    - target extruder
	 */
	static void _buffer_steps(const int32_t target, float fr_mm_s);

	/**
	 * Planner::buffer_segment
	 *
	 * Add a new linear movement to the buffer in axis units.
	 *
	 * Leveling and kinematics should be applied ahead of calling this.
	 *
	 *  a,b,c,e   - target positions in mm and/or degrees
	 *  fr_mm_s   - (target) speed of the move
	 *  extruder  - target extruder
	 */
	static void buffer_segment(const float c, const float fr_mm_s);

	static void _set_position_mm(const float c);

	/**
	 * Add a new linear movement to the buffer.
	 * The target is NOT translated to delta/scara
	 *
	 * Leveling will be applied to input on cartesians.
	 * Kinematic machines should call buffer_line_kinematic (for leveled moves).
	 * (Cartesians may also call buffer_line_kinematic.)
	 *
	 *  rx,ry,rz,e   - target position in mm or degrees
	 *  fr_mm_s      - (target) speed of the move (mm/s)
	 *  extruder     - target extruder
	 */
	__INLINE static void buffer_line(float rz, const float fr_mm_s)
	{
		buffer_segment(rz, fr_mm_s);
	}

	/**
	 * Add a new linear movement to the buffer.
	 * The target is cartesian, it's translated to delta/scara if
	 * needed.
	 *
	 *  cart     - x,y,z,e CARTESIAN target in mm
	 *  fr_mm_s  - (target) speed of the move (mm/s)
	 *  extruder - target extruder
	 */
	__INLINE static void buffer_line_kinematic(const float cart, const float fr_mm_s)
	{
		const float raw = cart;
		buffer_segment(raw, fr_mm_s);
	}

	/**
	 * Set the planner.position and individual stepper positions.
	 * Used by G92, G28, G29, and other procedures.
	 *
	 * Multiplies by axis_steps_per_mm[] and does necessary conversion
	 * for COREXY / COREXZ / COREYZ to set the corresponding stepper positions.
	 *
	 * Clears previous speed values.
	 */
	static void set_position_mm(const float v);
	__INLINE static void set_z_position_mm(const float z)
	{
		_set_position_mm(z);
	}

	/**
	 * Sync from the stepper positions. (e.g., after an interrupted move)
	 */
	static void sync_from_steppers();

	/**
	 * Does the buffer have any blocks queued?
	 */
	static bool blocks_queued()
	{
		register uint8_t tmp = block_buffer_head;
		return (tmp != block_buffer_tail);
	}

	/**
	 * "Discard" the block and "release" the memory.
	 * Called when the current block is no longer needed.
	 */
	__INLINE static void discard_current_block()
	{
		if (blocks_queued())
			block_buffer_tail = BLOCK_MOD(block_buffer_tail + 1);
	}

	/**
	 * "Discard" the next block if it's continued.
	 * Called after an interrupted move to throw away the rest of the move.
	 */
	__INLINE static bool discard_continued_block()
	{
		const bool discard = blocks_queued() && TEST(block_buffer[block_buffer_tail].flag, BLOCK_BIT_CONTINUED);
		if (discard)
			discard_current_block();
		return discard;
	}

	/**
	 * The current block. NULL if the buffer is empty.
	 * This also marks the block as busy.
	 * WARNING: Called from Stepper ISR context!
	 */
	static block_t* get_current_block()
	{
		if (blocks_queued())
		{
			block_t * const block = &block_buffer[block_buffer_tail];
			SBI(block->flag, BLOCK_BIT_BUSY);
			return block;
		}
		else
		{
			return NULL;
		}
	}

	private:

	/**
	 * Get the index of the next / previous block in the ring buffer
	 */
	static constexpr int8_t next_block_index(const int8_t block_index)
	{
		return BLOCK_MOD(block_index + 1);
	}
	static constexpr int8_t prev_block_index(const int8_t block_index)
	{
		return BLOCK_MOD(block_index - 1);
	}

	/**
	 * Calculate the distance (not time) it takes to accelerate
	 * from initial_rate to target_rate using the given acceleration:
	 */
	static float estimate_acceleration_distance(const float &initial_rate, const float &target_rate, const float &accel)
	{
		if (accel == 0)
			return 0; // accel was 0, set acceleration distance to 0
		return (sq(target_rate) - sq(initial_rate)) / (accel * 2);
	}

	/**
	 * Return the point at which you must start braking (at the rate of -'accel') if
	 * you start at 'initial_rate', accelerate (until reaching the point), and want to end at
	 * 'final_rate' after traveling 'distance'.
	 *
	 * This is used to compute the intersection point between acceleration and deceleration
	 * in cases where the "trapezoid" has no plateau (i.e., never reaches maximum speed)
	 */
	static float intersection_distance(const float &initial_rate, const float &final_rate, const float &accel, const float &distance)
	{
		if (accel == 0)
			return 0; // accel was 0, set intersection distance to 0
		return (accel * 2 * distance - sq(initial_rate) + sq(final_rate)) / (accel * 4);
	}

	/**
	 * Calculate the maximum allowable speed at this point, in order
	 * to reach 'target_velocity' using 'acceleration' within a given
	 * 'distance'.
	 */
	static float max_allowable_speed(const float &accel, const float &target_velocity, const float &distance)
	{
	  return SQRT(sq(target_velocity) - 2 * accel * distance);
	}

	static void calculate_trapezoid_for_block(block_t* const block, const float &entry_factor, const float &exit_factor);

	static void reverse_pass_kernel(block_t* const current, const block_t * const next);
	static void forward_pass_kernel(const block_t * const previous, block_t* const current);

	static void reverse_pass();
	static void forward_pass();

	static void recalculate_trapezoids();

	static void recalculate();

};

extern Planner planner;

#endif // PLANNER_H
