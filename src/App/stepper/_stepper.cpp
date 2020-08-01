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
 * stepper.cpp - A singleton object to execute motion plans using stepper motors
 * Marlin Firmware
 *
 * Derived from Grbl
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

/* The timer calculations of this module informed by the 'RepRap cartesian firmware' by Zack Smith
   and Philipp Tiefenbacher. */

#include "Marlin.h"
#include "_stepper.h"
#include "endstops.h"
#include "planner.h"
//#include "temperature.h"
//#include "ultralcd.h"
//#include "language.h"
//#include "cardreader.h"
//#include "mks_cfg.h"
//#include "speed_lookuptable.h"
#include "tim.h"









    #define X_ENABLE_INIT SET_OUTPUT(X_ENABLE_PIN)
    #define X_ENABLE_WRITE(STATE) WRITE(X_ENABLE_PIN,STATE)
    #define X_ENABLE_READ READ(X_ENABLE_PIN)
	#define X_DIR_INIT SET_OUTPUT(X_DIR_PIN)
	#define X_DIR_WRITE(STATE) WRITE(X_DIR_PIN,STATE)
	#define X_DIR_READ READ(X_DIR_PIN)
	#define X_STEP_INIT SET_OUTPUT(X_STEP_PIN)
	#define X_STEP_WRITE(STATE) WRITE(X_STEP_PIN,STATE)
	#define X_STEP_READ READ(X_STEP_PIN)







Stepper stepper; // Singleton

// public:

block_t* Stepper::current_block = NULL;  // A pointer to the block currently being traced

#if ENABLED(ABORT_ON_ENDSTOP_HIT_FEATURE_ENABLED)
  bool Stepper::abort_on_endstop_hit = false;
#endif


uint8_t Stepper::last_direction_bits = 0;        // The next stepping-bits to be output
int16_t Stepper::cleaning_buffer_counter = 0;

long Stepper::counter_X = 0;

volatile uint32_t Stepper::step_events_completed = 0; // The number of step events executed in the current block

long Stepper::acceleration_time, Stepper::deceleration_time;

volatile long Stepper::count_position[NUM_AXIS] = { 0 };
volatile signed char Stepper::count_direction[NUM_AXIS] = { 1, 1, 1, 1 };

uint8_t Stepper::step_loops, Stepper::step_loops_nominal;

uint16_t Stepper::hStepperTim.Instance->ARR_nominal,
         Stepper::acc_step_rate; // needed for deceleration start point

volatile long Stepper::endstops_trigsteps[XYZ];

  #define Z_APPLY_DIR(v,Q) Z_DIR_WRITE(v)

// intRes = longIn1 * longIn2 >> 24
// uses:
// r26 to store 0
// r27 to store bits 16-23 of the 48bit result. The top bit is used to round the two byte result.
// note that the lower two bytes and the upper byte of the 48bit result are not calculated.
// this can cause the result to be out by one as the lower bytes may cause carries into the upper ones.
// B0 A0 are bits 24-39 and are the returned value
// C1 B1 A1 is longIn1
// D2 C2 B2 A2 is longIn2
//
#define MultiU24X32toH16(intRes, longIn1, longIn2)  intRes = (uint16_t)((((uint64_t)longIn1 * (uint64_t)longIn2)>> 24)& 0XFFFF);
/*
#define MultiU24X32toH16(intRes, longIn1, longIn2) \
  asm volatile ( \
                 "clr r26 \n\t" \
                 "mul %A1, %B2 \n\t" \
                 "mov r27, r1 \n\t" \
                 "mul %B1, %C2 \n\t" \
                 "movw %A0, r0 \n\t" \
                 "mul %C1, %C2 \n\t" \
                 "add %B0, r0 \n\t" \
                 "mul %C1, %B2 \n\t" \
                 "add %A0, r0 \n\t" \
                 "adc %B0, r1 \n\t" \
                 "mul %A1, %C2 \n\t" \
                 "add r27, r0 \n\t" \
                 "adc %A0, r1 \n\t" \
                 "adc %B0, r26 \n\t" \
                 "mul %B1, %B2 \n\t" \
                 "add r27, r0 \n\t" \
                 "adc %A0, r1 \n\t" \
                 "adc %B0, r26 \n\t" \
                 "mul %C1, %A2 \n\t" \
                 "add r27, r0 \n\t" \
                 "adc %A0, r1 \n\t" \
                 "adc %B0, r26 \n\t" \
                 "mul %B1, %A2 \n\t" \
                 "add r27, r1 \n\t" \
                 "adc %A0, r26 \n\t" \
                 "adc %B0, r26 \n\t" \
                 "lsr r27 \n\t" \
                 "adc %A0, r26 \n\t" \
                 "adc %B0, r26 \n\t" \
                 "mul %D2, %A1 \n\t" \
                 "add %A0, r0 \n\t" \
                 "adc %B0, r1 \n\t" \
                 "mul %D2, %B1 \n\t" \
                 "add %B0, r0 \n\t" \
                 "clr r1 \n\t" \
                 : \
                 "=&r" (intRes) \
                 : \
                 "d" (longIn1), \
                 "d" (longIn2) \
                 : \
                 "r26" , "r27" \
               )
*/
// Some useful constants

/**
 *         __________________________
 *        /|                        |\     _________________         ^
 *       / |                        | \   /|               |\        |
 *      /  |                        |  \ / |               | \       s
 *     /   |                        |   |  |               |  \      p
 *    /    |                        |   |  |               |   \     e
 *   +-----+------------------------+---+--+---------------+----+    e
 *   |               BLOCK 1            |      BLOCK 2          |    d
 *
 *                           time ----->
 *
 *  The trapezoid is the shape the speed curve over time. It starts at block->initial_rate, accelerates
 *  first block->accelerate_until step_events_completed, then keeps going at constant speed until
 *  step_events_completed reaches block->decelerate_after after which it decelerates until the trapezoid generator is reset.
 *  The slope of acceleration is calculated using v = u + at where t is the accumulated timer values of the steps so far.
 */
void Stepper::wake_up() {
  // hStepperTim.Instance->CNT = 0;
  ENABLE_STEPPER_DRIVER_INTERRUPT();
}

/**
 * Set the stepper direction of each axis
 *
 *   COREXY: X_AXIS=A_AXIS and Y_AXIS=B_AXIS
 *   COREXZ: X_AXIS=A_AXIS and Z_AXIS=C_AXIS
 *   COREYZ: Y_AXIS=B_AXIS and Z_AXIS=C_AXIS
 */
void Stepper::set_directions() {

    if (motor_direction(Z_AXIS))
	{
      Z_APPLY_DIR(INVERT_Z_DIR, false);
      count_direction[Z_AXIS] = -1;
    }
    else
	{
      Z_APPLY_DIR(!INVERT_Z_DIR, false);
      count_direction[Z_AXIS] = 1;
    }

}

/**
 * Stepper Driver Interrupt
 *
 * Directly pulses the stepper motors at high frequency.
 * Timer 1 runs at a base frequency of 2MHz, with this ISR using hStepperTim.Instance->ARR compare mode.
 *
 * hStepperTim.Instance->ARR   Frequency
 *     1     2 MHz
 *    50    40 KHz
 *   100    20 KHz - capped max rate
 *   200    10 KHz - nominal max rate
 *  2000     1 KHz - sleep rate
 *  4000   500  Hz - init rate
 */
 /*
ISR(TIMER1_COMPA_vect) {
  #if ENABLED(LIN_ADVANCE)
    Stepper::advance_isr_scheduler();
  #else
    Stepper::isr();
  #endif
}
*/

//#define _ENABLE_ISRs() do { cli(); if (thermalManager.in_temp_isr) CBI(TIMSK0, OCIE0B); else SBI(TIMSK0, OCIE0B); ENABLE_STEPPER_DRIVER_INTERRUPT(); } while(0)
#define _ENABLE_ISRs() do { sei(); ENABLE_STEPPER_DRIVER_INTERRUPT(); } while(0)

void IsrStepperHandler() { 
	Stepper::StepperHandler(); 
  }

void Stepper::StepperHandler() {


  uint16_t ocr_val;

  #define ENDSTOP_NOMINAL_OCR_VAL 3000 // Check endstops every 1.5ms to guarantee two stepper ISRs within 5ms for BLTouch
  #define OCR_VAL_TOLERANCE       1000 // First max delay is 2.0ms, last min delay is 0.5ms, all others 1.5ms

    // Disable Timer0 ISRs and enable global ISR again to capture UART events (incoming chars)
    //CBI(TIMSK0, OCIE0B); // Temperature ISR
    DISABLE_STEPPER_DRIVER_INTERRUPT();
    sei();

  if (step_remaining && ENDSTOPS_ENABLED)
  {   // Just check endstops - not yet time for a step
    endstops.update();

    // Next ISR either for endstops or stepping
    ocr_val = step_remaining <= ENDSTOP_NOMINAL_OCR_VAL ? step_remaining : ENDSTOP_NOMINAL_OCR_VAL;
    step_remaining -= ocr_val;
  
    hStepperTim.Instance->ARR = ocr_val;
    if(hStepperTim.Instance->CNT > hStepperTim.Instance->ARR)
      hStepperTim.Instance->CNT = hStepperTim.Instance->ARR-1;
       
    NOLESS(hStepperTim.Instance->ARR, hStepperTim.Instance->CNT + 16);
  
    _ENABLE_ISRs(); // re-enable ISRs
    return;
  }

  //
  // When cleaning, discard the current block and run fast
  //
  if (cleaning_buffer_counter)
  {
    if (cleaning_buffer_counter < 0)
	{          // Count up for endstop hit
      if (current_block)
		  planner.discard_current_block(); // Discard the active block that led to the trigger
      if (!planner.discard_continued_block())   // Discard next CONTINUED block
        cleaning_buffer_counter = 0;            // Keep discarding until non-CONTINUED
    }
    else
	{
      planner.discard_current_block();
      --cleaning_buffer_counter;                // Count down for abort print
      #if ENABLED(SD_FINISHED_STEPPERRELEASE) && defined(SD_FINISHED_RELEASECOMMAND)
        if (!cleaning_buffer_counter)
			enqueue_and_echo_commands_P(PSTR(SD_FINISHED_RELEASECOMMAND));
      #endif
    }
    current_block = NULL;                       // Prep to get a new block after cleaning
    
    hStepperTim.Instance->ARR = 200;    // Run at max speed - 10 KHz
    if(hStepperTim.Instance->CNT > hStepperTim.Instance->ARR)
     hStepperTim.Instance->CNT = hStepperTim.Instance->ARR-1;

    _ENABLE_ISRs();
    return;
  }

  // If there is no current block, attempt to pop one from the buffer
  if (!current_block)
  {
    // Anything in the buffer?
    if ((current_block = planner.get_current_block()))
	{
      trapezoid_generator_reset();

      // Initialize Bresenham counters to 1/2 the ceiling
      counter_Z = -(current_block->step_event_count >> 1);

      step_events_completed = 0;

    }
    else {
      
	  hStepperTim.Instance->ARR = 2000;			// Run at slow speed - 1 KHz
	  if(hStepperTim.Instance->CNT>hStepperTim.Instance->ARR)
		  hStepperTim.Instance->CNT = hStepperTim.Instance->ARR-1;

      _ENABLE_ISRs(); // re-enable ISRs
      return;
    }
  }

  // Update endstops state, if enabled
  endstops.update();

  // Take multiple steps per interrupt (For high speed moves)
  bool all_steps_done = false;
  for (uint8_t i = step_loops; i--;)
  {

    #define _INVERT_STEP_PIN(AXIS) INVERT_## AXIS ##_STEP_PIN



    /**
     * Estimate the number of cycles that the stepper logic already takes
     * up between the start and stop of the X stepper pulse.
     *
     * Currently this uses very modest estimates of around 5 cycles.
     * True values may be derived by careful testing.
     *
     * Once any delay is added, the cost of the delay code itself
     * may be subtracted from this value to get a more accurate delay.
     * Delays under 20 cycles (1.25µs) will be very accurate, using NOPs.
     * Longer delays use a loop. The resolution is 8 cycles.
     */
    #define _CYCLE_APPROX_1 5
    #define _CYCLE_APPROX_1 0
    #define _CYCLE_APPROX_2 _CYCLE_APPROX_1
    #define _CYCLE_APPROX_3 _CYCLE_APPROX_2
    #define _CYCLE_APPROX_4 _CYCLE_APPROX_3
    #define _CYCLE_APPROX_5 _CYCLE_APPROX_4
    #define _CYCLE_APPROX_6 _CYCLE_APPROX_5
    #define _CYCLE_APPROX_7 _CYCLE_APPROX_6 + 5

    #define CYCLES_EATEN_XYZE _CYCLE_APPROX_7
    #define EXTRA_CYCLES_XYZE (STEP_PULSE_CYCLES - (CYCLES_EATEN_XYZE))

    /**
     * If a minimum pulse time was specified get the timer 0 value.
     *
     * TCNT0 has an 8x prescaler, so it increments every 8 cycles.
     * That's every 0.5µs on 16MHz and every 0.4µs on 20MHz.
     * 20 counts of TCNT0 -by itself- is a good pulse delay.
     * 10µs = 160 or 200 cycles.
     */
    #if EXTRA_CYCLES_XYZE > 20
      uint32_t pulse_start = TCNT0;
    #endif

     // Advance the Bresenham counter; start a pulse if the axis needs a step
     counter_Z += current_block->steps[Z_AXIS];
      if (counter_Z > 0)
	  {
		  Z_STEP_WRITE(!INVERT_Z_STEP_PIN);
	  }

    // For minimum pulse time wait before stopping pulses
    #if EXTRA_CYCLES_XYZE > 20
      while (EXTRA_CYCLES_XYZE > (uint32_t)(TCNT0 - pulse_start) * (INT0_PRESCALER)) { /* nada */ }
      pulse_start = TCNT0;
    #elif EXTRA_CYCLES_XYZE > 0
      DELAY_NOPS(EXTRA_CYCLES_XYZE);
    #endif

    // Stop an active pulse, reset the Bresenham counter, update the position
    if (counter_Z > 0)
	{
        counter_Z -= current_block->step_event_count;
        count_position[Z_AXIS] += count_direction[Z_AXIS];
        Z_STEP_WRITE(INVERT_Z_STEP_PIN);
    }

    if (++step_events_completed >= current_block->step_event_count) {
      all_steps_done = true;
      break;
    }

    // For minimum pulse time wait after stopping pulses also
    #if EXTRA_CYCLES_XYZE > 20
      if (i) while (EXTRA_CYCLES_XYZE > (uint32_t)(TCNT0 - pulse_start) * (INT0_PRESCALER)) { /* nada */ }
    #elif EXTRA_CYCLES_XYZE > 0
      if (i) DELAY_NOPS(EXTRA_CYCLES_XYZE);
    #endif

  } // steps_loop

  // Calculate new timer value
  if (step_events_completed <= (uint32_t)current_block->accelerate_until) {

    MultiU24X32toH16(acc_step_rate, acceleration_time, current_block->acceleration_rate);
    acc_step_rate += current_block->initial_rate;

    // upper limit
    NOMORE(acc_step_rate, current_block->nominal_rate);

    // step_rate to timer interval
    const uint16_t interval = calc_timer_interval(acc_step_rate);

    ocr_val = (uint16_t)interval;
    if (ENDSTOPS_ENABLED && interval > ENDSTOP_NOMINAL_OCR_VAL)
	{
        const uint16_t remainder = (uint16_t)interval % (ENDSTOP_NOMINAL_OCR_VAL);
        ocr_val = (remainder < OCR_VAL_TOLERANCE) ? ENDSTOP_NOMINAL_OCR_VAL + remainder : ENDSTOP_NOMINAL_OCR_VAL;
        step_remaining = (uint16_t)interval - ocr_val;
    };  // split step into multiple ISRs if larger than ENDSTOP_NOMINAL_OCR_VAL
    
	  hStepperTim.Instance->ARR = ocr_val;
	  if(hStepperTim.Instance->CNT>hStepperTim.Instance->ARR)
		  hStepperTim.Instance->CNT = hStepperTim.Instance->ARR-1;

    acceleration_time += interval;

  }
  else if (step_events_completed > (uint32_t)current_block->decelerate_after) {
    uint16_t step_rate;
    MultiU24X32toH16(step_rate, deceleration_time, current_block->acceleration_rate);

    if (step_rate < acc_step_rate) { // Still decelerating?
      step_rate = acc_step_rate - step_rate;
      NOLESS(step_rate, current_block->final_rate);
    }
    else
      step_rate = current_block->final_rate;

    // step_rate to timer interval
    const uint16_t interval = calc_timer_interval(step_rate);

      ocr_val = (uint16_t)interval;
      if (ENDSTOPS_ENABLED && interval > ENDSTOP_NOMINAL_OCR_VAL)
	  {
        const uint16_t remainder = (uint16_t)interval % (ENDSTOP_NOMINAL_OCR_VAL);
        ocr_val = (remainder < OCR_VAL_TOLERANCE) ? ENDSTOP_NOMINAL_OCR_VAL + remainder : ENDSTOP_NOMINAL_OCR_VAL;
        step_remaining = (uint16_t)interval - ocr_val;
      } // split step into multiple ISRs if larger than ENDSTOP_NOMINAL_OCR_VAL
	  
    
	  hStepperTim.Instance->ARR = ocr_val;
	  if(hStepperTim.Instance->CNT>hStepperTim.Instance->ARR)
		  hStepperTim.Instance->CNT = hStepperTim.Instance->ARR-1;

    deceleration_time += interval;

  }
  else {

    ocr_val = (uint16_t)hStepperTim.Instance->ARR_nominal;
    if (ENDSTOPS_ENABLED && hStepperTim.Instance->ARR_nominal > ENDSTOP_NOMINAL_OCR_VAL)
    {
  	const uint16_t remainder = (uint16_t)hStepperTim.Instance->ARR_nominal % (ENDSTOP_NOMINAL_OCR_VAL);
  	ocr_val = (remainder < OCR_VAL_TOLERANCE) ? ENDSTOP_NOMINAL_OCR_VAL + remainder : ENDSTOP_NOMINAL_OCR_VAL;
  	step_remaining = (uint16_t)hStepperTim.Instance->ARR_nominal - ocr_val;
    };  // split step into multiple ISRs if larger than ENDSTOP_NOMINAL_OCR_VAL
	  
    
	  hStepperTim.Instance->ARR = ocr_val;
	  if(hStepperTim.Instance->CNT>hStepperTim.Instance->ARR)
		  hStepperTim.Instance->CNT = hStepperTim.Instance->ARR-1;

    // ensure we're running at the correct step rate, even if we just came off an acceleration
    step_loops = step_loops_nominal;
  }

  NOLESS(hStepperTim.Instance->ARR, hStepperTim.Instance->CNT + 16);

  // If current block is finished, reset pointer
  if (all_steps_done) {
    current_block = NULL;
    planner.discard_current_block();
  }
  _ENABLE_ISRs(); // re-enable ISRs
}


void Stepper::init() {

  // Init Digipot Motor Current

  // Init Dir Pins
    Z_DIR_INIT;

  // Init Enable Pins - steppers default to disabled.
    Z_ENABLE_INIT;
    if (!0)
	{
		if(HIGH)
			stepperZ.Step_Clock(stepperZ.getStatus() & STATUS_HIZ); else stepperZ.softFree();
	}

  // Init endstops and pullups
  endstops.init();

  // Init Step Pins
    Z_STEP_INIT;
    Z_STEP_WRITE(_INVERT_STEP_PIN(Z));
    Z_DISABLE();


  ENABLE_STEPPER_DRIVER_INTERRUPT();

  endstops.enable(true); // Start with endstops active. After homing they can be disabled
  sei();

  set_directions(); // Init directions to last_direction_bits = 0
}


/**
 * Block until all buffered steps are executed / cleaned
 */
void Stepper::synchronize()
{
	while (planner.blocks_queued() || cleaning_buffer_counter)
		idle();
}
void Stepper::waitUntilEndOfAllBuffers()
{
	while (planner.blocks_queued());
}
void Stepper:: waitUntilEndOfAllMoves()
{
}

/**
 * Set the stepper positions directly in steps
 *
 * The input is based on the typical per-axis XYZ steps.
 * For CORE machines XYZ needs to be translated to ABC.
 *
 * This allows get_axis_position_mm to correctly
 * derive the current XYZ position later on.
 */
void Stepper::set_position(const long &a) {

  synchronize(); // Bad to set stepper counts in the middle of a move

  CRITICAL_SECTION_START;

    // default non-h-bot planning
    count_position[Z_AXIS] = a;

  CRITICAL_SECTION_END;
}

void Stepper::set_position(const long &v) {
  CRITICAL_SECTION_START;
  count_position[Z_AXIS] = v;
  CRITICAL_SECTION_END;
}

/**
 * Get a stepper's position in steps.
 */
long Stepper::position() {
  CRITICAL_SECTION_START;
  const long count_pos = count_position[Z_AXIS];
  CRITICAL_SECTION_END;
  return count_pos;
}

/**
 * Get an axis position according to stepper position(s)
 * For CORE machines apply translation from ABC to XYZ.
 */
float Stepper::get_axis_position_mm() {
  float axis_steps;
    axis_steps = position(Z_AXIS);
  return axis_steps * planner.steps_to_mm[Z_AXIS];
}

void Stepper::finish_and_disable() {
  synchronize();
  disable_all_steppers();
}

void Stepper::quick_stop() {
  cleaning_buffer_counter = 5000;
  DISABLE_STEPPER_DRIVER_INTERRUPT();
  while (planner.blocks_queued())
	  planner.discard_current_block();
  current_block = NULL;
  ENABLE_STEPPER_DRIVER_INTERRUPT();
}

void Stepper::endstop_triggered() {

  endstops_trigsteps[Z_AXIS] = count_position[];

  kill_current_block();
  cleaning_buffer_counter = -1; // Discard the rest of the move
}

void Stepper::report_positions() {
}

