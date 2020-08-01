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
 */

#include "z_planner.h"
#include "z_stepper.h"
#include "config.h"


	 
extern float		current_position;
	 


Planner				zPlanner;



  // public:

/**
 * A ring buffer of moves described in steps
 */
block_t Planner::block_buffer[BLOCK_BUFFER_SIZE];
volatile uint8_t Planner::block_buffer_head = 0,           // Index of the next block to be pushed
                 Planner::block_buffer_tail = 0;

float Planner::max_feedrate_mm_s, // Max speeds in mm per second
      Planner::axis_steps_per_mm,
      Planner::steps_to_mm;

unsigned long Planner::max_acceleration_steps_per_s2,
         Planner::max_acceleration_mm_per_s2; // Use M201 to override by software

// Initialized by settings.load()
float Planner::min_feedrate_mm_s,
      Planner::acceleration,         // Normal acceleration mm/s^2  DEFAULT ACCELERATION for all printing moves. M204 SXXXX
      Planner::retract_acceleration, // Retract acceleration mm/s^2 filament pull-back and push-forward while standing still in the other axes M204 TXXXX
      Planner::travel_acceleration,  // Travel acceleration mm/s^2  DEFAULT ACCELERATION for all NON printing moves. M204 MXXXX
      Planner::max_jerk,       // The largest speed change requiring no acceleration
      Planner::min_travel_feedrate_mm_s;

  volatile bool Planner::is_printing = 0;

// private:

int32_t Planner::position = 0;

uint32_t Planner::cutoff_long;

float Planner::previous_speed,
      Planner::previous_nominal_speed;

/**
 * Class and Instance Methods
 */

Planner::Planner() { init(); }

void Planner::init() {
  block_buffer_head = block_buffer_tail = 0;
  position = 0;
  previous_speed = 0;
  previous_nominal_speed = 0.0;
}

#define MINIMAL_STEP_RATE 120

/**
 * Calculate trapezoid parameters, multiplying the entry- and exit-speeds
 * by the provided factors.
 */
void Planner::calculate_trapezoid_for_block(block_t* const block, const float &entry_factor, const float &exit_factor) {
  uint32_t initial_rate = (int32_t)CEIL(block->nominal_rate * entry_factor),
           final_rate = (int32_t)CEIL(block->nominal_rate * exit_factor); // (steps per second)

  // Limit minimal step rate (Otherwise the timer will overflow.)
  NOLESS(initial_rate, MINIMAL_STEP_RATE);
  NOLESS(final_rate, MINIMAL_STEP_RATE);

  const int32_t accel = block->acceleration_steps_per_s2;

          // Steps required for acceleration, deceleration to/from nominal rate
  int32_t accelerate_steps = (int32_t)CEIL(estimate_acceleration_distance(initial_rate, block->nominal_rate, accel)),
          decelerate_steps = (int32_t)FLOOR(estimate_acceleration_distance(block->nominal_rate, final_rate, -accel)),
          // Steps between acceleration and deceleration, if any
          plateau_steps = block->step_event_count - accelerate_steps - decelerate_steps;

  // Does accelerate_steps + decelerate_steps exceed step_event_count?
  // Then we can't possibly reach the nominal rate, there will be no cruising.
  // Use intersection_distance() to calculate accel / braking time in order to
  // reach the final_rate exactly at the end of this block.
  if (plateau_steps < 0) {
    accelerate_steps = (int32_t)CEIL(intersection_distance(initial_rate, final_rate, accel, block->step_event_count));
    NOLESS(accelerate_steps, 0); // Check limits due to numerical round-off
    accelerate_steps = min((uint32_t)accelerate_steps, block->step_event_count);//(We can cast here to unsigned, because the above line ensures that we are above zero)
    plateau_steps = 0;
  }

  // block->accelerate_until = accelerate_steps;
  // block->decelerate_after = accelerate_steps+plateau_steps;

  CRITICAL_SECTION_START;  // Fill variables used by the stepper in a critical section
  if (!TEST(block->flag, BLOCK_BIT_BUSY)) { // Don't update variables if block is busy.
    block->accelerate_until = accelerate_steps;
    block->decelerate_after = accelerate_steps + plateau_steps;
    block->initial_rate = initial_rate;
    block->final_rate = final_rate;
  }
  CRITICAL_SECTION_END;
}

// "Junction jerk" in this context is the immediate change in speed at the junction of two blocks.
// This method will calculate the junction jerk as the euclidean distance between the nominal
// velocities of the respective blocks.
//inline float junction_jerk(block_t *before, block_t *after) {
//  return SQRT(
//    POW((before->speed_x-after->speed_x), 2)+POW((before->speed_y-after->speed_y), 2));
//}


// The kernel called by recalculate() when scanning the plan from last to first entry.
void Planner::reverse_pass_kernel(block_t* const current, const block_t * const next) {
  if (!current || !next) return;
  // If entry speed is already at the maximum entry speed, no need to recheck. Block is cruising.
  // If not, block in state of acceleration or deceleration. Reset entry speed to maximum and
  // check for maximum allowable speed reductions to ensure maximum possible planned speed.
  float max_entry_speed = current->max_entry_speed;
  if (current->entry_speed != max_entry_speed) {
    // If nominal length true, max junction speed is guaranteed to be reached. Only compute
    // for max allowable speed if block is decelerating and nominal length is false.
    current->entry_speed = (TEST(current->flag, BLOCK_BIT_NOMINAL_LENGTH) || max_entry_speed <= next->entry_speed)
      ? max_entry_speed
      : min(max_entry_speed, max_allowable_speed(-current->acceleration, next->entry_speed, current->millimeters));
    SBI(current->flag, BLOCK_BIT_RECALCULATE);
  }
}

/**
 * recalculate() needs to go over the current plan twice.
 * Once in reverse and once forward. This implements the reverse pass.
 */
void Planner::reverse_pass() {
  if (movesplanned() > 3) {
    const uint8_t endnr = BLOCK_MOD(block_buffer_tail + 2); // tail is running. tail+1 shouldn't be altered because it's connected to the running block.
                                                            // tail+2 because the index is not yet advanced when checked
    uint8_t blocknr = prev_block_index(block_buffer_head);
    block_t* current = &block_buffer[blocknr];

    do {
      const block_t * const next = current;
      blocknr = prev_block_index(blocknr);
      current = &block_buffer[blocknr];
      if (TEST(current->flag, BLOCK_BIT_START_FROM_FULL_HALT)) // Up to this every block is already optimized.
        break;
      reverse_pass_kernel(current, next);
    } while (blocknr != endnr);
  }
}

// The kernel called by recalculate() when scanning the plan from first to last entry.
void Planner::forward_pass_kernel(const block_t * const previous, block_t* const current) {
  if (!previous) return;

  // If the previous block is an acceleration block, but it is not long enough to complete the
  // full speed change within the block, we need to adjust the entry speed accordingly. Entry
  // speeds have already been reset, maximized, and reverse planned by reverse planner.
  // If nominal length is true, max junction speed is guaranteed to be reached. No need to recheck.
  if (!TEST(previous->flag, BLOCK_BIT_NOMINAL_LENGTH)) {
    if (previous->entry_speed < current->entry_speed) {
      float entry_speed = min(current->entry_speed,
                               max_allowable_speed(-previous->acceleration, previous->entry_speed, previous->millimeters));
      // Check for junction speed change
      if (current->entry_speed != entry_speed) {
        current->entry_speed = entry_speed;
        SBI(current->flag, BLOCK_BIT_RECALCULATE);
      }
    }
  }
}

/**
 * recalculate() needs to go over the current plan twice.
 * Once in reverse and once forward. This implements the forward pass.
 */
void Planner::forward_pass() {
  block_t* block[3] = { NULL, NULL, NULL };

  for (uint8_t b = block_buffer_tail; b != block_buffer_head; b = next_block_index(b)) {
    block[0] = block[1];
    block[1] = block[2];
    block[2] = &block_buffer[b];
    forward_pass_kernel(block[0], block[1]);
  }
  forward_pass_kernel(block[1], block[2]);
}

/**
 * Recalculate the trapezoid speed profiles for all blocks in the plan
 * according to the entry_factor for each junction. Must be called by
 * recalculate() after updating the blocks.
 */
void Planner::recalculate_trapezoids() {
  int8_t block_index = block_buffer_tail;
  block_t *current, *next = NULL;

  while (block_index != block_buffer_head) {
    current = next;
    next = &block_buffer[block_index];
    if (current) {
      // Recalculate if current block entry or exit junction speed has changed.
      if (TEST(current->flag, BLOCK_BIT_RECALCULATE) || TEST(next->flag, BLOCK_BIT_RECALCULATE)) {
        // NOTE: Entry and exit factors always > 0 by all previous logic operations.
        const float nomr = 1.0 / current->nominal_speed;
        calculate_trapezoid_for_block(current, current->entry_speed * nomr, next->entry_speed * nomr);
        CBI(current->flag, BLOCK_BIT_RECALCULATE); // Reset current only to ensure next trapezoid is computed
      }
    }
    block_index = next_block_index(block_index);
  }
  // Last/newest block in buffer. Exit speed is set with MINIMUM_PLANNER_SPEED. Always recalculated.
  if (next) {
    const float nomr = 1.0 / next->nominal_speed;
    calculate_trapezoid_for_block(next, next->entry_speed * nomr, (MINIMUM_PLANNER_SPEED) * nomr);
    CBI(next->flag, BLOCK_BIT_RECALCULATE);
  }
}

/*
 * Recalculate the motion plan according to the following algorithm:
 *
 *   1. Go over every block in reverse order...
 *
 *      Calculate a junction speed reduction (block_t.entry_factor) so:
 *
 *      a. The junction jerk is within the set limit, and
 *
 *      b. No speed reduction within one block requires faster
 *         deceleration than the one, true constant acceleration.
 *
 *   2. Go over every block in chronological order...
 *
 *      Dial down junction speed reduction values if:
 *      a. The speed increase within one block would require faster
 *         acceleration than the one, true constant acceleration.
 *
 * After that, all blocks will have an entry_factor allowing all speed changes to
 * be performed using only the one, true constant acceleration, and where no junction
 * jerk is jerkier than the set limit, Jerky. Finally it will:
 *
 *   3. Recalculate "trapezoids" for all blocks.
 */
void Planner::recalculate() {
  reverse_pass();
  forward_pass();
  recalculate_trapezoids();
}



/**
 * Maintain fans, paste extruder pressure,
 */
void Planner::check_axes_activity() {
  unsigned char axis_active = 0;

  if (blocks_queued())
  {
    block_t* block;

    for (uint8_t b = block_buffer_tail; b != block_buffer_head; b = next_block_index(b)) {
      block = &block_buffer[b];
      if (block->steps)
		  axis_active++;
    }
  }
  else {
  }
    if (!axis_active)
	  HAL_GPIO_WritePin(Z_ENA_GPIO_Port, Z_ENA_Pin, (GPIO_PinState)1);
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
void Planner::_buffer_steps(const int32_t target, float fr_mm_s) {

  const int32_t dc = target - position;


  // Compute direction bit-mask for this block
  uint8_t dm = 0;
    if (dc < 0)
		dm = 1;

  // Calculate the buffer head after we push this byte
  const uint8_t next_buffer_head = next_block_index(block_buffer_head);

  // If the buffer is full: good! That means we are well ahead of the robot.
  // Rest here until there is room in the buffer.
  while (block_buffer_tail == next_buffer_head);
//	  idle();

  // Prepare to set up new block
  block_t* block = &block_buffer[block_buffer_head];

  // Clear all flags, including the "busy" bit
  block->flag = 0x00;

  // Set direction bits
  block->direction_bits = dm;

  // Number of steps for each axis
  // See http://www.corexy.com/theory.html
    // default non-h-bot planning
    block->steps = labs(dc);

  block->step_event_count = block->steps;

  // Bail if this is a zero-length block
  if (block->step_event_count < MIN_STEPS_PER_SEGMENT) return;

      if (block->steps)
		  HAL_GPIO_WritePin(Z_ENA_GPIO_Port, Z_ENA_Pin, (GPIO_PinState)0);;

  if (is_printing)
    NOLESS(fr_mm_s, min_feedrate_mm_s);
  else
    NOLESS(fr_mm_s, min_travel_feedrate_mm_s);

  /**
   * This part of the code calculates the total length of the movement.
   * For cartesian bots, the X_AXIS is the real X movement and same for Y_AXIS.
   * But for corexy bots, that is not true. The "X_AXIS" and "Y_AXIS" motors (that should be named to A_AXIS
   * and B_AXIS) cannot be used for X and Y length, because A=X+Y and B=X-Y.
   * So we need to create other 2 "AXIS", named X_HEAD and Y_HEAD, meaning the real displacement of the Head.
   * Having the real displacement of the head, we can calculate the total movement length and apply the desired speed.
   */
    float delta_mm;
    delta_mm = dc * steps_to_mm;

  if (block->steps < MIN_STEPS_PER_SEGMENT)
  {
  }
  else {
    block->millimeters = SQRT(
        sq(delta_mm)
    );
  }
  const float inverse_millimeters = 1.0 / block->millimeters;  // Inverse millimeters to remove multiple divides

  // Calculate inverse time for this move. No divide by zero due to previous checks.
  // Example: At 120mm/s a 60mm move takes 0.5s. So this will give 2.0.
  float inverse_secs = fr_mm_s * inverse_millimeters;

  const uint8_t moves_queued = movesplanned();

  // Slow down when the buffer starts to empty, rather than wait at the corner for a buffer refill
  block->nominal_speed = block->millimeters * inverse_secs;           //   (mm/sec) Always > 0
  block->nominal_rate = (int32_t)CEIL(block->step_event_count * inverse_secs); // (step/sec) Always > 0


  // Calculate and limit speed in mm/sec for each axis
  float current_speed, speed_factor = 1.0; // factor <1 decreases speed
    const float cs = FABS((current_speed = delta_mm * inverse_secs));
    if (cs > max_feedrate_mm_s)
		NOMORE(speed_factor, max_feedrate_mm_s / cs);


  // Correct the speed
  if (speed_factor < 1.0) {
    current_speed *= speed_factor;
    block->nominal_speed *= speed_factor;
    block->nominal_rate *= speed_factor;
  }

  // Compute and limit the acceleration rate for the trapezoid generator.
  const float steps_per_mm = block->step_event_count * inverse_millimeters;
  uint32_t accel;
  if (!block->steps) {
    // convert to: acceleration steps/sec^2
    accel = (int32_t)CEIL((is_printing ? acceleration : travel_acceleration) * steps_per_mm);
  }
  else {
    #define LIMIT_ACCEL_LONG() do{ \
      if (block->steps && max_acceleration_steps_per_s2 < accel) { \
        const uint32_t comp = max_acceleration_steps_per_s2 * block->step_event_count; \
        if (accel * block->steps > comp) accel = comp / block->steps; \
      } \
    }while(0)

    #define LIMIT_ACCEL_FLOAT() do{ \
      if (block->steps && max_acceleration_steps_per_s2 < accel) { \
        const float comp = (float)max_acceleration_steps_per_s2 * (float)block->step_event_count; \
        if ((float)accel * (float)block->steps > comp) accel = (int32_t)(comp / (float)block->steps); \
      } \
    }while(0)

    // Start with print or travel acceleration
    accel = (int32_t)CEIL((is_printing ? acceleration : travel_acceleration) * steps_per_mm);

    // Limit acceleration per axis
    if (block->step_event_count <= cutoff_long) {
      LIMIT_ACCEL_LONG();
    }
    else {
      LIMIT_ACCEL_FLOAT();
    }
  }
  block->acceleration_steps_per_s2 = accel;
  block->acceleration = accel / steps_per_mm;
  //block->acceleration_rate = (long)(accel * 16777216.0 / ((F_CPU) * 0.125)); // * 8.388608
  block->acceleration_rate = (long)(accel * 8.388608); // * 8.388608

  // Initial limit on the segment entry velocity
  float vmax_junction;

  /**
   * Adapted from Průša MKS firmware
   * https://github.com/prusa3d/Prusa-Firmware
   *
   * Start with a safe speed (from which the machine may halt to stop immediately).
   */

  // Exit speed limited by a jerk to full halt of a previous last segment
  static float previous_safe_speed;

  float safe_speed = block->nominal_speed;
  uint8_t limited = 0;
  const float jerk = FABS(current_speed), maxj = max_jerk;
    if (jerk > maxj)
	{
      if (limited)
	  {
        const float mjerk = maxj * block->nominal_speed;
        if (jerk * safe_speed > mjerk) safe_speed = mjerk / jerk;
      }
      else
	  {
        ++limited;
        safe_speed = maxj;
      }
    }


  if (moves_queued && !UNEAR_ZERO(previous_nominal_speed)) {
    // Estimate a maximum velocity allowed at a joint of two successive segments.
    // If this maximum velocity allowed is lower than the minimum of the entry / exit safe velocities,
    // then the machine is not coasting anymore and the safe entry / exit velocities shall be used.

    // The junction velocity will be shared between successive segments. Limit the junction velocity to their minimum.
    // Pick the smaller of the nominal speeds. Higher speed shall not be achieved at the junction during coasting.
    vmax_junction = min(block->nominal_speed, previous_nominal_speed);

    // Factor to multiply the previous / current nominal velocities to get componentwise limited velocities.
    float v_factor = 1;
    limited = 0;

    // Now limit the jerk in all axes.
    const float smaller_speed_factor = vmax_junction / previous_nominal_speed;
      // Limit an axis. We have to differentiate: coasting, reversal of an axis, full stop.
      float v_exit = previous_speed * smaller_speed_factor, v_entry = current_speed;
      if (limited)
	  {
        v_exit *= v_factor;
        v_entry *= v_factor;
      }

      // Calculate jerk depending on whether the axis is coasting in the same direction or reversing.
      const float jerk = (v_exit > v_entry)
          ? //                                  coasting             axis reversal
            ( (v_entry > 0 || v_exit < 0) ? (v_exit - v_entry) : max(v_exit, -v_entry) )
          : // v_exit <= v_entry                coasting             axis reversal
            ( (v_entry < 0 || v_exit > 0) ? (v_entry - v_exit) : max(-v_exit, v_entry) );

      if (jerk > max_jerk)
	  {
        v_factor *= max_jerk / jerk;
        ++limited;
      }
    if (limited) vmax_junction *= v_factor;
    // Now the transition velocity is known, which maximizes the shared exit / entry velocity while
    // respecting the jerk factors, it may be possible, that applying separate safe exit / entry velocities will achieve faster prints.
    const float vmax_junction_threshold = vmax_junction * 0.99f;
    if (previous_safe_speed > vmax_junction_threshold && safe_speed > vmax_junction_threshold) {
      // Not coasting. The machine will stop and start the movements anyway,
      // better to start the segment from start.
      SBI(block->flag, BLOCK_BIT_START_FROM_FULL_HALT);
      vmax_junction = safe_speed;
    }
  }
  else {
    SBI(block->flag, BLOCK_BIT_START_FROM_FULL_HALT);
    vmax_junction = safe_speed;
  }

  // Max entry speed of this block equals the max exit speed of the previous block.
  block->max_entry_speed = vmax_junction;

  // Initialize block entry speed. Compute based on deceleration to user-defined MINIMUM_PLANNER_SPEED.
  const float v_allowable = max_allowable_speed(-block->acceleration, MINIMUM_PLANNER_SPEED, block->millimeters);
  block->entry_speed = min(vmax_junction, v_allowable);

  // Initialize planner efficiency flags
  // Set flag if block will always reach maximum junction speed regardless of entry/exit speeds.
  // If a block can de/ac-celerate from nominal speed to zero within the length of the block, then
  // the current block and next block junction speeds are guaranteed to always be at their maximum
  // junction speeds in deceleration and acceleration, respectively. This is due to how the current
  // block nominal speed limits both the current and next maximum junction speeds. Hence, in both
  // the reverse and forward planners, the corresponding block junction speed will always be at the
  // the maximum junction speed and may always be ignored for any speed reduction checks.
  block->flag |= BLOCK_FLAG_RECALCULATE | (block->nominal_speed <= v_allowable ? BLOCK_FLAG_NOMINAL_LENGTH : 0);

  // Update previous path unit_vector and nominal speed
  previous_speed =  current_speed;
  previous_nominal_speed = block->nominal_speed;
  previous_safe_speed = safe_speed;

  const float bnsr = 1.0 / block->nominal_speed;
  calculate_trapezoid_for_block(block, block->entry_speed * bnsr, safe_speed * bnsr);

  // Move buffer head
  block_buffer_head = next_buffer_head;

  // Update the position (only when a move was queued)
//  static_assert(COUNT(target) > 1, "Parameter to _buffer_steps must be (&target)[XYZE]!");
  position = target;

  recalculate();

} // _buffer_steps()

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
void Planner::buffer_segment(const float c, const float fr_mm_s)
{

    //gCfgItems.breakpoint_reprint_flg = 0;
  // When changing extruders recalculate steps corresponding to the E position
  // The target position of the tool in absolute steps
  // Calculate target position in absolute steps
  const int32_t target = LROUND(c * axis_steps_per_mm);

  // Always split the first move into two (if not homing or probing)
  if (!blocks_queued()) {

    #define _BETWEEN() (position + target) >> 1
    const int32_t between = _BETWEEN();
    DISABLE_STEPPER_DRIVER_INTERRUPT();

    _buffer_steps(between, fr_mm_s);

    const uint8_t next = block_buffer_head;
    _buffer_steps(target, fr_mm_s);
    SBI(block_buffer[next].flag, BLOCK_BIT_CONTINUED);
    ENABLE_STEPPER_DRIVER_INTERRUPT();
  }
  else
    _buffer_steps(target, fr_mm_s);

  zStepper.wake_up();

} // buffer_segment()

/**
 * Directly set the planner XYZ position (and stepper positions)
 * converting mm (or angles for SCARA) into steps.
 *
 * On CORE machines stepper ABC will be translated from the given XYZ.
 */

void Planner::_set_position_mm(const float c)
{
  const int32_t nc = position = LROUND(c * axis_steps_per_mm);
  zStepper.set_position(nc);
  previous_nominal_speed = 0.0; // Resets planner junction speeds. Assumes start from rest.
  previous_speed = 0;
}

/**
 * Sync from the stepper positions. (e.g., after an interrupted move)
 */
void Planner::sync_from_steppers() {
    position = zStepper.position();
}

/**
 * Setters for planner position (also setting stepper position).
 */
void Planner::set_position_mm(const float v)
{
  position = LROUND(v * axis_steps_per_mm);
  zStepper.set_position(position);
  previous_speed = 0.0;
}

// Recalculate the steps/s^2 acceleration rates, based on the mm/s^2
void Planner::reset_acceleration_rates()
{
  uint32_t highest_rate = 1;
  max_acceleration_steps_per_s2 = (int32_t)(max_acceleration_mm_per_s2 * axis_steps_per_mm);
  NOLESS(highest_rate, max_acceleration_steps_per_s2);
  cutoff_long = 4294967295UL / highest_rate;
}

// Recalculate position, steps_to_mm if axis_steps_per_mm changes!
void Planner::refresh_positioning()
{
  steps_to_mm = 1.0 / axis_steps_per_mm;
  _set_position_mm(current_position);
  reset_acceleration_rates();
}

