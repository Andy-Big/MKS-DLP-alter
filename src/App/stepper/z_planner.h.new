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
#pragma once

/**
 * planner.h
 *
 * Buffer movement commands and manage the acceleration profile plan
 *
 * Derived from Grbl
 * Copyright (c) 2009-2011 Simen Svale Skogsrud
 */


#include <math.h>

#include "main.h"
#include "motor.h"


//#define S_CURVE_ACCELERATION


class Planner;
extern Planner			zPlanner;




#define _BV(bit)			(1<<(bit))
#define TEST(n,b)			(((n)&_BV(b))!=0)
#define SBI(n,b)			(n |= _BV(b))
#define CBI(n,b)			(n &= ~_BV(b))
#define __SET_BIT(n,b,value) (n) ^= ((-value)^(n)) & (_BV(b))	/*--mks--*/
#define NOLESS(v,n)			do{ if (v < n) v = n; }while(0)
#define NOMORE(v,n)			do{ if (v > n) v = n; }while(0)


#define sq(x)				pow(x,2)
#define FABS(x)				fabs(x)
#define POW(x, y)			pow(x, y)
#define SQRT(x)				sqrt(x)
#define CEIL(x)				ceil(x)
#define FLOOR(x)			floor(x)
#define _MIN(a,b)			((a)<(b)?(a):(b))
#define _MAX(a,b)			(((a)>(b))?(a):(b))
#define LROUND(x)			lround(x)
#define ABS(v)				( v >= 0 ? v : -v )

#define cli()				__ASM volatile("cpsid i")	//__disable_irq()
#define sei()				__ASM volatile("cpsie i")	//__enable_irq()
#define UNEAR_ZERO(x)		((x) < 0.000001)




#define F_CPU						168000000
#define BLOCK_BUFFER_SIZE			8 // maximize block buffer
#define MAX_STEP_FREQUENCY			40000 // Max step frequency for Ultimaker (5000 pps / half step)
#define MINIMUM_PLANNER_SPEED		0.01 // (mm/sec)
#define MIN_STEPS_PER_SEGMENT		4
// Delay for delivery of first block to the stepper ISR, if the queue contains 2 or
// fewer movements. The delay is measured in milliseconds, and must be less than 250ms
#define BLOCK_DELAY_FOR_1ST_MOVE	100
#define TEMP_TIMER_FREQUENCY		1000
#define STEPPER_TIMER_RATE			(HAL_RCC_GetSysClockFreq() / 2)/STEPPER_TIMER_PRESCALE





enum BlockFlagBit : char {
  // Recalculate trapezoids on entry junction. For optimization.
  BLOCK_BIT_RECALCULATE,

  // Nominal speed always reached.
  // i.e., The segment is long enough, so the nominal speed is reachable if accelerating
  // from a safe speed (in consideration of jerking from zero speed).
  BLOCK_BIT_NOMINAL_LENGTH,

  // The block is segment 2+ of a longer move
  BLOCK_BIT_CONTINUED,

  // Sync the stepper counts from the block
  BLOCK_BIT_SYNC_POSITION

};

enum BlockFlag : char {
    BLOCK_FLAG_RECALCULATE          = _BV(BLOCK_BIT_RECALCULATE)
  , BLOCK_FLAG_NOMINAL_LENGTH       = _BV(BLOCK_BIT_NOMINAL_LENGTH)
  , BLOCK_FLAG_CONTINUED            = _BV(BLOCK_BIT_CONTINUED)
  , BLOCK_FLAG_SYNC_POSITION        = _BV(BLOCK_BIT_SYNC_POSITION)
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
typedef struct block_t {

  volatile uint8_t flag;                    // Block flags (See BlockFlag enum above) - Modified by ISR and main thread!

  // Fields used by the motion planner to manage acceleration
  float nominal_speed_sqr,                  // The nominal speed for this block in (mm/sec)^2
        entry_speed_sqr,                    // Entry speed at previous-current junction in (mm/sec)^2
        max_entry_speed_sqr,                // Maximum allowable junction entry speed in (mm/sec)^2
        millimeters,                        // The total travel of this block in mm
        acceleration;                       // acceleration mm/sec^2

  union {
    uint32_t steps;                     // Step count along each axis
    int32_t position;                   // New position to force when this sync block is executed
  };
  uint32_t step_event_count;                // The number of step events required to complete this block

  // Settings for the trapezoid generator
  uint32_t accelerate_until,                // The index of the step event on which to stop acceleration
           decelerate_after;                // The index of the step event on which to start decelerating

  #ifdef S_CURVE_ACCELERATION
    uint32_t cruise_rate,                   // The actual cruise rate to use, between end of the acceleration phase and start of deceleration phase
             acceleration_time,             // Acceleration time and deceleration time in STEP timer counts
             deceleration_time,
             acceleration_time_inverse,     // Inverse of acceleration and deceleration periods, expressed as integer. Scale depends on CPU being used
             deceleration_time_inverse;
  #else
    uint32_t acceleration_rate;             // The acceleration rate used for acceleration calculation
  #endif

  uint8_t direction_bits;                   // The direction bit set for this block (refers to *_DIRECTION_BIT in config.h)

  uint32_t nominal_rate,                    // The nominal step rate for this block in step_events/sec
           initial_rate,                    // The jerk-adjusted step rate at start of block
           final_rate,                      // The minimal rate at exit
           acceleration_steps_per_s2;       // acceleration steps/sec^2

} block_t;

#define BLOCK_MOD(n) ((n)&(BLOCK_BUFFER_SIZE-1))

typedef struct {
   uint32_t		max_acceleration_mm_per_s2;		// (mm/s^2) M201 XYZE
   uint32_t		min_segment_time_us;			// (µs) M205 B
   float		axis_steps_per_mm;				// (steps) M92 XYZE - Steps per millimeter
   float		max_feedrate_mm_s;				// (mm/s) M203 XYZE - Max speeds
   float		acceleration;					// (mm/s^2) M204 S - Normal acceleration. DEFAULT ACCELERATION for all printing moves.
   float		retract_acceleration;			// (mm/s^2) M204 R - Retract acceleration. Filament pull-back and push-forward while standing still in the other axes
   float		travel_acceleration;			// (mm/s^2) M204 T - Travel acceleration. DEFAULT ACCELERATION for all NON printing moves.
   float		min_feedrate_mm_s;				// (mm/s) M205 S - Minimum linear feedrate
   float		min_travel_feedrate_mm_s;		// (mm/s) M205 T - Minimum travel feedrate
   float		max_jerk;
} planner_settings_t;













class Planner {
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
                            block_buffer_nonbusy,   // Index of the first non busy block
                            block_buffer_planned,   // Index of the optimally planned block
                            block_buffer_tail;      // Index of the busy block, if any
    static uint16_t cleaning_buffer_counter;        // A counter to disable queuing of blocks
    static uint8_t delay_before_delivering;         // This counter delays delivery of blocks when queue becomes empty to allow the opportunity of merging blocks


    static planner_settings_t settings;

    static uint32_t max_acceleration_steps_per_s2; // (steps/s^2) Derived from mm_per_s2
    static float steps_to_mm;           // Millimeters per step

    #if HAS_JUNCTION_DEVIATION
      static float junction_deviation_mm;       // (mm) M205 J
      #if HAS_LINEAR_E_JERK
        static float max_e_jerk[DISTINCT_E];    // Calculated from junction_deviation_mm
      #endif
    #endif

      // (mm/s^2) M205 XYZ(E) - The largest speed change requiring no acceleration.
      static float max_jerk;

    /**
     * The current position of the tool in absolute steps
     * Recalculated if any axis_steps_per_mm are changed by gcode
     */
    static int32_t position;

  private:

    /**
     * Speed of previous path line segment
     */
    static float previous_speed;

    /**
     * Nominal speed of previous path line segment (mm/s)^2
     */
    static float previous_nominal_speed_sqr;

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
    static void set_max_acceleration(float targetValue);
    static void set_max_feedrate(float targetValue);
    static void set_max_jerk(float targetValue);


    // Manage fans, paste pressure, etc.
    static void check_axes_activity();

    // Number of moves currently in the planner including the busy block, if any
    __INLINE static uint8_t movesplanned() { return BLOCK_MOD(block_buffer_head - block_buffer_tail); }

    // Number of nonbusy moves currently in the planner
    __INLINE static uint8_t nonbusy_movesplanned() { return BLOCK_MOD(block_buffer_head - block_buffer_nonbusy); }

    // Remove all blocks from the buffer
    __INLINE static void clear_block_buffer() { block_buffer_nonbusy = block_buffer_planned = block_buffer_head = block_buffer_tail = 0; }

    // Check if movement queue is full
    __INLINE static bool is_full() { return block_buffer_tail == next_block_index(block_buffer_head); }

    // Get count of movement slots free
    __INLINE static uint8_t moves_free() { return BLOCK_BUFFER_SIZE - 1 - movesplanned(); }

    /**
     * Planner::get_next_free_block
     *
     * - Get the next head indices (passed by reference)
     * - Wait for the number of spaces to open up in the planner
     * - Return the first head block
     */
    __INLINE static block_t* get_next_free_block(uint8_t &next_buffer_head, const uint8_t count=1) {

      // Wait until there are enough slots free
      while (moves_free() < count) { idle(); }

      // Return the first available block
      next_buffer_head = next_block_index(block_buffer_head);
      return &block_buffer[block_buffer_head];
    }

    /**
     * Planner::_buffer_steps
     *
     * Add a new linear movement to the buffer (in terms of steps).
     *
     *  target      - target position in steps units
     *  fr_mm_s     - (target) speed of the move
     *  extruder    - target extruder
     *  millimeters - the length of the movement, if known
     *
     * Returns true if movement was buffered, false otherwise
     */
    static bool _buffer_steps(const int32_t target, float fr_mm_s, const float millimeters=0.0);

    /**
     * Planner::_populate_block
     *
     * Fills a new linear movement in the block (in terms of steps).
     *
     *  target      - target position in steps units
     *  fr_mm_s     - (target) speed of the move
     *  extruder    - target extruder
     *  millimeters - the length of the movement, if known
     *
     * Returns true is movement is acceptable, false otherwise
     */
    static bool _populate_block(block_t * const block, bool split_move, const int32_t target, float fr_mm_s, const float millimeters=0.0);

    /**
     * Planner::buffer_sync_block
     * Add a block to the buffer that just updates the position
     */
    static void buffer_sync_block();

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
     */
    static bool buffer_segment(const float a, const float fr_mm_s, const float &millimeters=0.0);


  public:

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
    static bool buffer_line(const float rz, const float fr_mm_s, const float millimeters=0.0);

    __INLINE static bool buffer_line(const uint32_t z, const float fr_mm_s, const float millimeters=0.0) {
      return buffer_line(z, fr_mm_s, millimeters);
    }

    /**
     * Set the planner.position and individual stepper positions.
     * Used by G92, G28, G29, and other procedures.
     *
     * The supplied position is in the cartesian coordinate space and is
     * translated in to machine space as needed. Modifiers such as leveling
     * and skew are also applied.
     *
     * Multiplies by axis_steps_per_mm[] and does necessary conversion
     * for COREXY / COREXZ / COREYZ to set the corresponding stepper positions.
     *
     * Clears previous speed values.
     */
    static void set_position_mm(const float rz);

    /**
     * Set the planner.position and individual stepper positions.
     *
     * The supplied position is in machine space, and no additional
     * conversions are applied.
     */
    static void set_machine_position_mm(float c);

    /**
     * Get an axis position according to stepper position(s)
     * For CORE machines apply translation from ABC to XYZ.
     */
    static float get_axis_position_mm();

    static inline float get_axis_positions_mm() {
        return get_axis_position_mm();
    }

    // Called to force a quick stop of the machine (for example, when
    // a Full Shutdown is required, or when endstops are hit)
    static void quick_stop();

    // Called when an endstop is triggered. Causes the machine to stop inmediately
    static void endstop_triggered();

    // Triggered position of an axis in mm (not core-savvy)
    static float triggered_position_mm();

    // Block until all buffered steps are executed / cleaned
    static void synchronize();

    // Wait for moves to finish and disable all steppers
    static void finish_and_disable();

    // Periodic tick to handle cleaning timeouts
    // Called from the Temperature ISR at ~1kHz
    static void tick() {
      if (cleaning_buffer_counter) --cleaning_buffer_counter;
    }

    /**
     * Does the buffer have any blocks queued?
     */
    __INLINE static bool has_blocks_queued() { return (block_buffer_head != block_buffer_tail); }

    /**
     * Get the current block for processing
     * and mark the block as busy.
     * Return nullptr if the buffer is empty
     * or if there is a first-block delay.
     *
     * WARNING: Called from Stepper ISR context!
     */
    static block_t* get_current_block();

    /**
     * "Release" the current block so its slot can be reused.
     * Called when the current block is no longer needed.
     */
    __INLINE static void release_current_block()
	{
      if (has_blocks_queued())
        block_buffer_tail = next_block_index(block_buffer_tail);
    }

  private:

    /**
     * Get the index of the next / previous block in the ring buffer
     */
    static constexpr uint8_t next_block_index(const uint8_t block_index) { return BLOCK_MOD(block_index + 1); }
    static constexpr uint8_t prev_block_index(const uint8_t block_index) { return BLOCK_MOD(block_index - 1); }

    /**
     * Calculate the distance (not time) it takes to accelerate
     * from initial_rate to target_rate using the given acceleration:
     */
    static float estimate_acceleration_distance(const float initial_rate, const float target_rate, const float accel) {
      if (accel == 0) return 0; // accel was 0, set acceleration distance to 0
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
    static float intersection_distance(const float initial_rate, const float final_rate, const float accel, const float distance) {
      if (accel == 0) return 0; // accel was 0, set intersection distance to 0
      return (accel * 2 * distance - sq(initial_rate) + sq(final_rate)) / (accel * 4);
    }

    /**
     * Calculate the maximum allowable speed squared at this point, in order
     * to reach 'target_velocity_sqr' using 'acceleration' within a given
     * 'distance'.
     */
    static float max_allowable_speed_sqr(const float accel, const float target_velocity_sqr, const float distance) {
      return target_velocity_sqr - 2 * accel * distance;
    }

    #ifdef S_CURVE_ACCELERATION
      /**
       * Calculate the speed reached given initial speed, acceleration and distance
       */
      static float final_speed(const float initial_velocity, const float accel, const float distance) {
        return SQRT(sq(initial_velocity) + 2 * accel * distance);
      }
    #endif

    static void calculate_trapezoid_for_block(block_t* const block, const float &entry_factor, const float &exit_factor);

    static void reverse_pass_kernel(block_t* const current, const block_t * const next);
    static void forward_pass_kernel(const block_t * const previous, block_t* const current, uint8_t block_index);

    static void reverse_pass();
    static void forward_pass();

    static void recalculate_trapezoids();

    static void recalculate();

};

#define PLANNER_XY_FEEDRATE() (planner.settings.max_feedrate_mm_s)

