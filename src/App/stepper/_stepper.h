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
 * stepper.h - stepper motor driver: executes motion plans of planner.c using the stepper motors
 * Derived from Grbl
 *
 * Copyright (c) 2009-2011 Simen Svale Skogsrud
 *
 * Grbl is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Grbl is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Grbl.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef STEPPER_H
#define STEPPER_H

#include "planner.h"
#include "speed_lookuptable.h"
//#include "stepper_indirection.h"
//#include "language.h"
//#include "types.h"
#include "tim.h"

class Stepper;
extern Stepper stepper;

//#define ENABLE_STEPPER_DRIVER_INTERRUPT()  SBI(TIMSK1, OCIE1A)
//#define DISABLE_STEPPER_DRIVER_INTERRUPT() CBI(TIMSK1, OCIE1A)
#define ENABLE_STEPPER_DRIVER_INTERRUPT()   HAL_TIM_Base_Start_IT(&hStepperTim)    //?a?¡ì?¨¨?D?¡ì?¡ì?¡ì??a?a???¨¬?¡ìo?¡§¡è????D??
#define DISABLE_STEPPER_DRIVER_INTERRUPT() 	HAL_TIM_Base_Stop_IT(&hStepperTim)		//?a?¡ì?¨¨?D?¡ì?¡ì?¡ì??a1????¨¬?¡ìo?¡§¡è????D??

// intRes = intIn1 * intIn2 >> 16
// uses:
// r26 to store 0
// r27 to store the byte 1 of the 24 bit result
#define MultiU16X8toH16(intRes, charIn1, intIn2) intRes = (uint16_t)(((uint8_t)((uint8_t)charIn1 * (uint16_t)intIn2))>> 16)
/*
#define MultiU16X8toH16(intRes, charIn1, intIn2) \
  asm volatile ( \
                 "clr r26 \n\t" \
                 "mul %A1, %B2 \n\t" \
                 "movw %A0, r0 \n\t" \
                 "mul %A1, %A2 \n\t" \
                 "add %A0, r1 \n\t" \
                 "adc %B0, r26 \n\t" \
                 "lsr r0 \n\t" \
                 "adc %A0, r26 \n\t" \
                 "adc %B0, r26 \n\t" \
                 "clr r1 \n\t" \
                 : \
                 "=&r" (intRes) \
                 : \
                 "d" (charIn1), \
                 "d" (intIn2) \
                 : \
                 "r26" \
               )
*/
class Stepper {

  public:

    static block_t* current_block;  // A pointer to the block currently being traced

    #if ENABLED(ABORT_ON_ENDSTOP_HIT_FEATURE_ENABLED)
      static bool abort_on_endstop_hit;
    #endif

    static int16_t cleaning_buffer_counter;

  private:

    static uint8_t last_direction_bits;        // The next stepping-bits to be output

    // Counter variables for the Bresenham line tracer
    static long counter_X, counter_Y, counter_Z, counter_E;
    static volatile uint32_t step_events_completed; // The number of step events executed in the current block

	#define _NEXT_ISR(T)	do{hStepperTim.Instance->ARR = T;  if(hStepperTim.Instance->CNT>hStepperTim.Instance->ARR) hStepperTim.Instance->CNT = hStepperTim.Instance->ARR-1;}while(0)

    static long acceleration_time, deceleration_time;
    static uint8_t step_loops, step_loops_nominal;

    static uint16_t hStepperTim.Instance->ARR_nominal,
                    acc_step_rate; // needed for deceleration start point

    static volatile long endstops_trigsteps[XYZ];
    static volatile long endstops_stepsTotal, endstops_stepsDone;

    //
    // Positions of stepper motors, in step units
    //
    static volatile long count_position[NUM_AXIS];

    //
    // Current direction of stepper motors (+1 or -1)
    //
    static volatile signed char count_direction[NUM_AXIS];

    //
    // Mixing extruder mix counters
    //

  public:

    //
    // Constructor / initializer
    //
    Stepper() { };

    //
    // Initialize stepper hardware
    //
    static void init();

    //
    // Interrupt Service Routines
    //

    //static void isr();
		static void StepperHandler(void);

    //
    // Block until all buffered steps are executed
    //
    static void synchronize();
		static void waitUntilEndOfAllBuffers();
		static void waitUntilEndOfAllMoves();

    //
    // Set the current position in steps
    //
    static void set_position(const long &a, const long &b, const long &c, const long &e);
    static void set_position(const AxisEnum &a, const long &v);
    static void set_e_position(const long &e);

    //
    // Set direction bits for all steppers
    //
    static void set_directions();

    //
    // Get the position of a stepper, in steps
    //
    static long position(const AxisEnum axis);

    //
    // Report the positions of the steppers, in steps
    //
    static void report_positions();

    //
    // Get the position (mm) of an axis based on stepper position(s)
    //
    static float get_axis_position_mm(const AxisEnum axis);

    //
    // The stepper subsystem goes to sleep when it runs out of things to execute. Call this
    // to notify the subsystem that it is time to go to work.
    //
    static void wake_up();

    //
    // Wait for moves to finish and disable all steppers
    //
    static void finish_and_disable();

    //
    // Quickly stop all steppers and clear the blocks queue
    //
    static void quick_stop();

    //
    // The direction of a single motor
    //
    FORCE_INLINE static bool motor_direction()
	{
		return TEST(last_direction_bits, Z_AXIS);
	}

    static inline void kill_current_block() {
      step_events_completed = current_block->step_event_count;
    }

    //
    // Handle a triggered endstop
    //
    static void endstop_triggered(const AxisEnum axis);

    //
    // Triggered position of an axis in mm (not core-savvy)
    //
    FORCE_INLINE static float triggered_position_mm(const AxisEnum axis) {
      return endstops_trigsteps[axis] * planner.steps_to_mm[axis];
    }

  private:

    FORCE_INLINE static unsigned short calc_timer_interval(unsigned short step_rate) {
      unsigned short timer;

      NOMORE(step_rate, MAX_STEP_FREQUENCY);

      if (step_rate > 20000) { // If steprate > 20kHz >> step 4 times
        step_rate >>= 2;
        step_loops = 4;
      }
      else if (step_rate > 10000) { // If steprate > 10kHz >> step 2 times
        step_rate >>= 1;
        step_loops = 2;
      }
      else {
        step_loops = 1;
      }
	timer = (uint16_t)(F_CPU() / (step_rate * (2*42+2))); 	/*--mks--*/ 
		if(timer < 9)  timer = 9;	//200KHz
		
 		return timer;

    }

    // Initialize the trapezoid generator from the current block.
    // Called whenever a new block begins.
    FORCE_INLINE static void trapezoid_generator_reset()
	{

      static int8_t last_extruder = -1;

      if (current_block->direction_bits != last_direction_bits || current_block->active_extruder != last_extruder) {
        last_direction_bits = current_block->direction_bits;
        last_extruder = current_block->active_extruder;
        set_directions();
      }

      deceleration_time = 0;
      // step_rate to timer interval
      hStepperTim.Instance->ARR_nominal = calc_timer_interval(current_block->nominal_rate);
      // make a note of the number of step loops required at nominal speed
      step_loops_nominal = step_loops;
      acc_step_rate = current_block->initial_rate;
      acceleration_time = calc_timer_interval(acc_step_rate);
      hStepperTim.Instance->ARR = acceleration_time;
	  if(hStepperTim.Instance->CNT > hStepperTim.Instance->ARR)
		  hStepperTim.Instance->CNT = hStepperTim.Instance->ARR-1;

    }


};

#endif // STEPPER_H
