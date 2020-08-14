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
* along with Grbl.  If not, see <https://www.gnu.org/licenses/>.
*/

/**
* Timer calculations informed by the 'RepRap cartesian firmware' by Zack Smith
* and Philipp Tiefenbacher.
*/

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

/**
* Marlin uses the Bresenham algorithm. For a detailed explanation of theory and
* method see https://www.cs.helsinki.fi/group/goa/mallinnus/lines/bresenh.html
*/

/**
* Jerk controlled movements planner added Apr 2018 by Eduardo José Tagle.
* Equations based on Synthethos TinyG2 sources, but the fixed-point
* implementation is new, as we are running the ISR with a variable period.
* Also implemented the Bézier velocity curve evaluation in ARM assembler,
* to avoid impacting ISR speed.
*/

#include "z_stepper.h"
#include "z_endstops.h"
#include "z_planner.h"
#include "config.h"
#include "motor.h"




extern Stepper		zStepper;







// private:

block_t* Stepper::current_block; // (= nullptr) A pointer to the block currently being traced

uint8_t Stepper::last_direction_bits, // = 0
Stepper::axis_did_move; // = 0

bool Stepper::abort_current_block;


uint32_t Stepper::acceleration_time, Stepper::deceleration_time;
uint8_t Stepper::steps_per_isr;


int32_t Stepper::delta_error;

uint32_t Stepper::advance_dividend;
uint32_t Stepper::advance_divisor = 0,
		Stepper::step_events_completed = 0, // The number of step events executed in the current block
		Stepper::accelerate_until,          // The count at which to stop accelerating
		Stepper::decelerate_after,          // The count at which to start decelerating
		Stepper::step_event_count;          // The total event count for the current block
/*
#ifdef S_CURVE_ACCELERATION
int32_t __attribute__((used)) Stepper::bezier_A asm("bezier_A");    // A coefficient in Bézier speed curve with alias for assembler
int32_t __attribute__((used)) Stepper::bezier_B asm("bezier_B");    // B coefficient in Bézier speed curve with alias for assembler
int32_t __attribute__((used)) Stepper::bezier_C asm("bezier_C");    // C coefficient in Bézier speed curve with alias for assembler
uint32_t __attribute__((used)) Stepper::bezier_F asm("bezier_F");   // F coefficient in Bézier speed curve with alias for assembler
uint32_t __attribute__((used)) Stepper::bezier_AV asm("bezier_AV"); // AV coefficient in Bézier speed curve with alias for assembler
bool Stepper::bezier_2nd_half;    // =false If Bézier curve has been initialized or not
#endif
*/
#ifdef S_CURVE_ACCELERATION
int32_t Stepper::bezier_A;    // A coefficient in Bézier speed curve with alias for assembler
int32_t Stepper::bezier_B;    // B coefficient in Bézier speed curve with alias for assembler
int32_t Stepper::bezier_C;    // C coefficient in Bézier speed curve with alias for assembler
uint32_t Stepper::bezier_F;   // F coefficient in Bézier speed curve with alias for assembler
uint32_t Stepper::bezier_AV; // AV coefficient in Bézier speed curve with alias for assembler
bool Stepper::bezier_2nd_half;    // =false If Bézier curve has been initialized or not
#endif

int32_t Stepper::ticks_nominal = -1;
#ifndef S_CURVE_ACCELERATION
uint32_t Stepper::acc_step_rate; // needed for deceleration start point
#endif

int32_t Stepper::endstops_trigsteps;
int32_t Stepper::count_position;
int8_t Stepper::count_direction;

#define Z_APPLY_DIR(v,Q) Z_DIR_WRITE(v)
#define Z_APPLY_STEP(v,Q) Z_STEP_WRITE(v)


#define CYCLES_TO_NS(CYC) (1000UL * (CYC) / ((F_CPU) / 1000000))
#define NS_PER_PULSE_TIMER_TICK (1000000000UL / (STEPPER_TIMER_RATE))

// Round up when converting from ns to timer ticks
#define NS_TO_PULSE_TIMER_TICKS(NS) (((NS) + (NS_PER_PULSE_TIMER_TICK) / 2) / (NS_PER_PULSE_TIMER_TICK))

#define TIMER_SETUP_NS (CYCLES_TO_NS(TIMER_READ_ADD_AND_STORE_CYCLES))

#define PULSE_HIGH_TICK_COUNT uint32_t(NS_TO_PULSE_TIMER_TICKS(_MIN_PULSE_HIGH_NS - _MIN(_MIN_PULSE_HIGH_NS, TIMER_SETUP_NS)))
#define PULSE_LOW_TICK_COUNT uint32_t(NS_TO_PULSE_TIMER_TICKS(_MIN_PULSE_LOW_NS - _MIN(_MIN_PULSE_LOW_NS, TIMER_SETUP_NS)))

#define USING_TIMED_PULSE() uint32_t start_pulse_count = 0
#define START_TIMED_PULSE(DIR) (start_pulse_count = __HAL_TIM_GET_COUNTER(&hStepperTim))
#define AWAIT_TIMED_PULSE(DIR) while (PULSE_##DIR##_TICK_COUNT > __HAL_TIM_GET_COUNTER(&hStepperTim) - start_pulse_count) { }
#define START_HIGH_PULSE()  START_TIMED_PULSE(HIGH)
#define AWAIT_HIGH_PULSE()  AWAIT_TIMED_PULSE(HIGH)
#define START_LOW_PULSE()   START_TIMED_PULSE(LOW)
#define AWAIT_LOW_PULSE()   AWAIT_TIMED_PULSE(LOW)

#if MINIMUM_STEPPER_PRE_DIR_DELAY > 0
#define DIR_WAIT_BEFORE() DELAY_NS(MINIMUM_STEPPER_PRE_DIR_DELAY)
#else
#define DIR_WAIT_BEFORE()
#endif

#if MINIMUM_STEPPER_POST_DIR_DELAY > 0
#define DIR_WAIT_AFTER() DELAY_NS(MINIMUM_STEPPER_POST_DIR_DELAY)
#else
#define DIR_WAIT_AFTER()
#endif

/**
* Set the stepper direction of each axis
*
*   COREXY: X_AXIS=A_AXIS and Y_AXIS=B_AXIS
*   COREXZ: X_AXIS=A_AXIS and Z_AXIS=C_AXIS
*   COREYZ: Y_AXIS=B_AXIS and Z_AXIS=C_AXIS
*/
void Stepper::set_directions()
{
	
	DIR_WAIT_BEFORE();
	
	if (motor_direction())
	{
		HAL_GPIO_WritePin(Z_DIR_GPIO_Port, Z_DIR_Pin, (GPIO_PinState)cfgzMotor.invert_z_dir);
		count_direction = -1;
	}
	else
	{
		HAL_GPIO_WritePin(Z_DIR_GPIO_Port, Z_DIR_Pin, (GPIO_PinState)!cfgzMotor.invert_z_dir);
		count_direction = 1;
	}
	
	DIR_WAIT_AFTER();
}

#ifdef S_CURVE_ACCELERATION
/**
*  This uses a quintic (fifth-degree) Bézier polynomial for the velocity curve, giving
*  a "linear pop" velocity curve; with pop being the sixth derivative of position:
*  velocity - 1st, acceleration - 2nd, jerk - 3rd, snap - 4th, crackle - 5th, pop - 6th
*
*  The Bézier curve takes the form:
*
*  V(t) = P_0 * B_0(t) + P_1 * B_1(t) + P_2 * B_2(t) + P_3 * B_3(t) + P_4 * B_4(t) + P_5 * B_5(t)
*
*  Where 0 <= t <= 1, and V(t) is the velocity. P_0 through P_5 are the control points, and B_0(t)
*  through B_5(t) are the Bernstein basis as follows:
*
*        B_0(t) =   (1-t)^5        =   -t^5 +  5t^4 - 10t^3 + 10t^2 -  5t   +   1
*        B_1(t) =  5(1-t)^4 * t    =   5t^5 - 20t^4 + 30t^3 - 20t^2 +  5t
*        B_2(t) = 10(1-t)^3 * t^2  = -10t^5 + 30t^4 - 30t^3 + 10t^2
*        B_3(t) = 10(1-t)^2 * t^3  =  10t^5 - 20t^4 + 10t^3
*        B_4(t) =  5(1-t)   * t^4  =  -5t^5 +  5t^4
*        B_5(t) =             t^5  =    t^5
*                                      ^       ^       ^       ^       ^       ^
*                                      |       |       |       |       |       |
*                                      A       B       C       D       E       F
*
*  Unfortunately, we cannot use forward-differencing to calculate each position through
*  the curve, as Marlin uses variable timer periods. So, we require a formula of the form:
*
*        V_f(t) = A*t^5 + B*t^4 + C*t^3 + D*t^2 + E*t + F
*
*  Looking at the above B_0(t) through B_5(t) expanded forms, if we take the coefficients of t^5
*  through t of the Bézier form of V(t), we can determine that:
*
*        A =    -P_0 +  5*P_1 - 10*P_2 + 10*P_3 -  5*P_4 +  P_5
*        B =   5*P_0 - 20*P_1 + 30*P_2 - 20*P_3 +  5*P_4
*        C = -10*P_0 + 30*P_1 - 30*P_2 + 10*P_3
*        D =  10*P_0 - 20*P_1 + 10*P_2
*        E = - 5*P_0 +  5*P_1
*        F =     P_0
*
*  Now, since we will (currently) *always* want the initial acceleration and jerk values to be 0,
*  We set P_i = P_0 = P_1 = P_2 (initial velocity), and P_t = P_3 = P_4 = P_5 (target velocity),
*  which, after simplification, resolves to:
*
*        A = - 6*P_i +  6*P_t =  6*(P_t - P_i)
*        B =  15*P_i - 15*P_t = 15*(P_i - P_t)
*        C = -10*P_i + 10*P_t = 10*(P_t - P_i)
*        D = 0
*        E = 0
*        F = P_i
*
*  As the t is evaluated in non uniform steps here, there is no other way rather than evaluating
*  the Bézier curve at each point:
*
*        V_f(t) = A*t^5 + B*t^4 + C*t^3 + F          [0 <= t <= 1]
*
* Floating point arithmetic execution time cost is prohibitive, so we will transform the math to
* use fixed point values to be able to evaluate it in realtime. Assuming a maximum of 250000 steps
* per second (driver pulses should at least be 2µS hi/2µS lo), and allocating 2 bits to avoid
* overflows on the evaluation of the Bézier curve, means we can use
*
*   t: unsigned Q0.32 (0 <= t < 1) |range 0 to 0xFFFFFFFF unsigned
*   A:   signed Q24.7 ,            |range = +/- 250000 * 6 * 128 = +/- 192000000 = 0x0B71B000 | 28 bits + sign
*   B:   signed Q24.7 ,            |range = +/- 250000 *15 * 128 = +/- 480000000 = 0x1C9C3800 | 29 bits + sign
*   C:   signed Q24.7 ,            |range = +/- 250000 *10 * 128 = +/- 320000000 = 0x1312D000 | 29 bits + sign
*   F:   signed Q24.7 ,            |range = +/- 250000     * 128 =      32000000 = 0x01E84800 | 25 bits + sign
*
* The trapezoid generator state contains the following information, that we will use to create and evaluate
* the Bézier curve:
*
*  blk->step_event_count [TS] = The total count of steps for this movement. (=distance)
*  blk->initial_rate     [VI] = The initial steps per second (=velocity)
*  blk->final_rate       [VF] = The ending steps per second  (=velocity)
*  and the count of events completed (step_events_completed) [CS] (=distance until now)
*
*  Note the abbreviations we use in the following formulae are between []s
*
*  For Any 32bit CPU:
*
*    At the start of each trapezoid, calculate the coefficients A,B,C,F and Advance [AV], as follows:
*
*      A =  6*128*(VF - VI) =  768*(VF - VI)
*      B = 15*128*(VI - VF) = 1920*(VI - VF)
*      C = 10*128*(VF - VI) = 1280*(VF - VI)
*      F =    128*VI        =  128*VI
*     AV = (1<<32)/TS      ~= 0xFFFFFFFF / TS (To use ARM UDIV, that is 32 bits) (this is computed at the planner, to offload expensive calculations from the ISR)
*
*    And for each point, evaluate the curve with the following sequence:
*
*      void lsrs(uint32_t& d, uint32_t s, int cnt) {
*        d = s >> cnt;
*      }
*      void lsls(uint32_t& d, uint32_t s, int cnt) {
*        d = s << cnt;
*      }
*      void lsrs(int32_t& d, uint32_t s, int cnt) {
*        d = uint32_t(s) >> cnt;
*      }
*      void lsls(int32_t& d, uint32_t s, int cnt) {
*        d = uint32_t(s) << cnt;
*      }
*      void umull(uint32_t& rlo, uint32_t& rhi, uint32_t op1, uint32_t op2) {
*        uint64_t res = uint64_t(op1) * op2;
*        rlo = uint32_t(res & 0xFFFFFFFF);
*        rhi = uint32_t((res >> 32) & 0xFFFFFFFF);
*      }
*      void smlal(int32_t& rlo, int32_t& rhi, int32_t op1, int32_t op2) {
*        int64_t mul = int64_t(op1) * op2;
*        int64_t s = int64_t(uint32_t(rlo) | ((uint64_t(uint32_t(rhi)) << 32U)));
*        mul += s;
*        rlo = int32_t(mul & 0xFFFFFFFF);
*        rhi = int32_t((mul >> 32) & 0xFFFFFFFF);
*      }
*      int32_t _eval_bezier_curve_arm(uint32_t curr_step) {
*        uint32_t flo = 0;
*        uint32_t fhi = bezier_AV * curr_step;
*        uint32_t t = fhi;
*        int32_t alo = bezier_F;
*        int32_t ahi = 0;
*        int32_t A = bezier_A;
*        int32_t B = bezier_B;
*        int32_t C = bezier_C;
*
*        lsrs(ahi, alo, 1);          // a  = F << 31
*        lsls(alo, alo, 31);         //
*        umull(flo, fhi, fhi, t);    // f *= t
*        umull(flo, fhi, fhi, t);    // f>>=32; f*=t
*        lsrs(flo, fhi, 1);          //
*        smlal(alo, ahi, flo, C);    // a+=(f>>33)*C
*        umull(flo, fhi, fhi, t);    // f>>=32; f*=t
*        lsrs(flo, fhi, 1);          //
*        smlal(alo, ahi, flo, B);    // a+=(f>>33)*B
*        umull(flo, fhi, fhi, t);    // f>>=32; f*=t
*        lsrs(flo, fhi, 1);          // f>>=33;
*        smlal(alo, ahi, flo, A);    // a+=(f>>33)*A;
*        lsrs(alo, ahi, 6);          // a>>=38
*
*        return alo;
*      }
*
*  This is rewritten in ARM assembly for optimal performance (43 cycles to execute).
*
*  For AVR, the precision of coefficients is scaled so the Bézier curve can be evaluated in real-time:
*  Let's reduce precision as much as possible. After some experimentation we found that:
*
*    Assume t and AV with 24 bits is enough
*       A =  6*(VF - VI)
*       B = 15*(VI - VF)
*       C = 10*(VF - VI)
*       F =     VI
*      AV = (1<<24)/TS   (this is computed at the planner, to offload expensive calculations from the ISR)
*
*    Instead of storing sign for each coefficient, we will store its absolute value,
*    and flag the sign of the A coefficient, so we can save to store the sign bit.
*    It always holds that sign(A) = - sign(B) = sign(C)
*
*     So, the resulting range of the coefficients are:
*
*       t: unsigned (0 <= t < 1) |range 0 to 0xFFFFFF unsigned
*       A:   signed Q24 , range = 250000 * 6 = 1500000 = 0x16E360 | 21 bits
*       B:   signed Q24 , range = 250000 *15 = 3750000 = 0x393870 | 22 bits
*       C:   signed Q24 , range = 250000 *10 = 2500000 = 0x1312D0 | 21 bits
*       F:   signed Q24 , range = 250000     =  250000 = 0x0ED090 | 20 bits
*
*    And for each curve, estimate its coefficients with:
*
*      void _calc_bezier_curve_coeffs(int32_t v0, int32_t v1, uint32_t av) {
*       // Calculate the Bézier coefficients
*       if (v1 < v0) {
*         A_negative = true;
*         bezier_A = 6 * (v0 - v1);
*         bezier_B = 15 * (v0 - v1);
*         bezier_C = 10 * (v0 - v1);
*       }
*       else {
*         A_negative = false;
*         bezier_A = 6 * (v1 - v0);
*         bezier_B = 15 * (v1 - v0);
*         bezier_C = 10 * (v1 - v0);
*       }
*       bezier_F = v0;
*      }
*
*    And for each point, evaluate the curve with the following sequence:
*
*      // unsigned multiplication of 24 bits x 24bits, return upper 16 bits
*      void umul24x24to16hi(uint16_t& r, uint24_t op1, uint24_t op2) {
*        r = (uint64_t(op1) * op2) >> 8;
*      }
*      // unsigned multiplication of 16 bits x 16bits, return upper 16 bits
*      void umul16x16to16hi(uint16_t& r, uint16_t op1, uint16_t op2) {
*        r = (uint32_t(op1) * op2) >> 16;
*      }
*      // unsigned multiplication of 16 bits x 24bits, return upper 24 bits
*      void umul16x24to24hi(uint24_t& r, uint16_t op1, uint24_t op2) {
*        r = uint24_t((uint64_t(op1) * op2) >> 16);
*      }
*
*      int32_t _eval_bezier_curve(uint32_t curr_step) {
*        // To save computing, the first step is always the initial speed
*        if (!curr_step)
*          return bezier_F;
*
*        uint16_t t;
*        umul24x24to16hi(t, bezier_AV, curr_step);   // t: Range 0 - 1^16 = 16 bits
*        uint16_t f = t;
*        umul16x16to16hi(f, f, t);                   // Range 16 bits (unsigned)
*        umul16x16to16hi(f, f, t);                   // Range 16 bits : f = t^3  (unsigned)
*        uint24_t acc = bezier_F;                    // Range 20 bits (unsigned)
*        if (A_negative) {
*          uint24_t v;
*          umul16x24to24hi(v, f, bezier_C);          // Range 21bits
*          acc -= v;
*          umul16x16to16hi(f, f, t);                 // Range 16 bits : f = t^4  (unsigned)
*          umul16x24to24hi(v, f, bezier_B);          // Range 22bits
*          acc += v;
*          umul16x16to16hi(f, f, t);                 // Range 16 bits : f = t^5  (unsigned)
*          umul16x24to24hi(v, f, bezier_A);          // Range 21bits + 15 = 36bits (plus sign)
*          acc -= v;
*        }
*        else {
*          uint24_t v;
*          umul16x24to24hi(v, f, bezier_C);          // Range 21bits
*          acc += v;
*          umul16x16to16hi(f, f, t);                 // Range 16 bits : f = t^4  (unsigned)
*          umul16x24to24hi(v, f, bezier_B);          // Range 22bits
*          acc -= v;
*          umul16x16to16hi(f, f, t);                 // Range 16 bits : f = t^5  (unsigned)
*          umul16x24to24hi(v, f, bezier_A);          // Range 21bits + 15 = 36bits (plus sign)
*          acc += v;
*        }
*        return acc;
*      }
*    These functions are translated to assembler for optimal performance.
*    Coefficient calculation takes 70 cycles. Bezier point evaluation takes 150 cycles.
*/


// For all the other 32bit CPUs
__INLINE void Stepper::_calc_bezier_curve_coeffs(const int32_t v0, const int32_t v1, const uint32_t av) {
	// Calculate the Bézier coefficients
	bezier_A =  768 * (v1 - v0);
	bezier_B = 1920 * (v0 - v1);
	bezier_C = 1280 * (v1 - v0);
	bezier_F =  128 * v0;
	bezier_AV = av;
}

__INLINE int32_t Stepper::_eval_bezier_curve(const uint32_t curr_step)
{
	uint32_t t = bezier_AV * curr_step;               // t: Range 0 - 1^32 = 32 bits
	uint64_t f = t;
	f *= t;                                           // Range 32*2 = 64 bits (unsigned)
	f >>= 32;                                         // Range 32 bits  (unsigned)
	f *= t;                                           // Range 32*2 = 64 bits  (unsigned)
	f >>= 32;                                         // Range 32 bits : f = t^3  (unsigned)
	int64_t acc = (int64_t) bezier_F << 31;           // Range 63 bits (signed)
	acc += ((uint32_t) f >> 1) * (int64_t) bezier_C;  // Range 29bits + 31 = 60bits (plus sign)
	f *= t;                                           // Range 32*2 = 64 bits
	f >>= 32;                                         // Range 32 bits : f = t^3  (unsigned)
	acc += ((uint32_t) f >> 1) * (int64_t) bezier_B;  // Range 29bits + 31 = 60bits (plus sign)
	f *= t;                                           // Range 32*2 = 64 bits
	f >>= 32;                                         // Range 32 bits : f = t^3  (unsigned)
	acc += ((uint32_t) f >> 1) * (int64_t) bezier_A;  // Range 28bits + 31 = 59bits (plus sign)
	acc >>= (31 + 7);                                 // Range 24bits (plus sign)
	return (int32_t) acc;
	
}
#endif // S_CURVE_ACCELERATION

/**
* Stepper Driver Interrupt
*
* Directly pulses the stepper motors at high frequency.
*/

void IsrStepperHandler()
{ 
	if (__HAL_TIM_GET_FLAG(&hStepperTim, TIM_FLAG_UPDATE) == SET)
		__HAL_TIM_CLEAR_FLAG(&hStepperTim, TIM_FLAG_UPDATE);
	Stepper::isr(); 
}
/*
void Stepper::StepperHandler() {
HAL_timer_isr_prologue(STEP_TIMER_NUM);

Stepper::isr();

HAL_timer_isr_epilogue(STEP_TIMER_NUM);
}
*/
#define STEP_MULTIPLY(A,B) MultiU32X24toH32(A, B)

void Stepper::isr()
{
	
	static uint32_t nextMainISR = 0;  // Interval until the next main Stepper Pulse phase (0 = Now)
	
	// Program timer compare for the maximum period, so it does NOT
	// flag an interrupt while this ISR is running - So changes from small
	// periods to big periods are respected and the timer does not reset to 0
	__HAL_TIM_SET_AUTORELOAD(&hStepperTim, 0xFFFF);
	if (__HAL_TIM_GET_COUNTER(&hStepperTim) >= 0xFFFF)
		hStepperTim.Instance->EGR |= TIM_EGR_UG; // Generate an immediate update interrupt
	
	// Count of ticks for the next ISR
	uint32_t next_isr_ticks = 0;
	
	// Limit the amount of iterations
	uint8_t max_loops = 10;
	
	// We need this variable here to be able to use it in the following loop
	uint32_t min_ticks;
	do
	{
		// Enable ISRs to reduce USART processing latency
		__enable_irq();
		
		if (!nextMainISR)
			pulse_phase_isr();                            // 0 = Do coordinated axes Stepper pulses
		
		// ^== Time critical. NOTHING besides pulse generation should be above here!!!
		
		if (!nextMainISR)
			nextMainISR = block_phase_isr();  // Manage acc/deceleration, get next block
		
		// Get the interval to the next ISR call
		const uint32_t interval = _MIN(
									   nextMainISR,                                       // Time until the next Pulse / Block phase
									   uint32_t(0xFFFF)                    // Come back in a very long time
										   );
		
		//
		// Compute remaining time for each ISR phase
		//     NEVER : The phase is idle
		//      Zero : The phase will occur on the next ISR call
		//  Non-zero : The phase will occur on a future ISR call
		//
		
		nextMainISR -= interval;
		
		/**
		* This needs to avoid a race-condition caused by interleaving
		* of interrupts required by both the LA and Stepper algorithms.
		*
		* Assume the following tick times for stepper pulses:
		*   Stepper ISR (S):  1 1000 2000 3000 4000
		*   Linear Adv. (E): 10 1010 2010 3010 4010
		*
		* The current algorithm tries to interleave them, giving:
		*  1:S 10:E 1000:S 1010:E 2000:S 2010:E 3000:S 3010:E 4000:S 4010:E
		*
		* Ideal timing would yield these delta periods:
		*  1:S  9:E  990:S   10:E  990:S   10:E  990:S   10:E  990:S   10:E
		*
		* But, since each event must fire an ISR with a minimum duration, the
		* minimum delta might be 900, so deltas under 900 get rounded up:
		*  900:S d900:E d990:S d900:E d990:S d900:E d990:S d900:E d990:S d900:E
		*
		* It works, but divides the speed of all motors by half, leading to a sudden
		* reduction to 1/2 speed! Such jumps in speed lead to lost steps (not even
		* accounting for double/quad stepping, which makes it even worse).
		*/
		
		// Compute the tick count for the next ISR
		next_isr_ticks += interval;
		
		/**
		* The following section must be done with global interrupts disabled.
		* We want nothing to interrupt it, as that could mess the calculations
		* we do for the next value to program in the period register of the
		* stepper timer and lead to skipped ISRs (if the value we happen to program
		* is less than the current count due to something preempting between the
		* read and the write of the new period value).
		*/
		__disable_irq();
		
		/**
		* Get the current tick value + margin
		* Assuming at least 6µs between calls to this ISR...
		* On AVR the ISR epilogue+prologue is estimated at 100 instructions - Give 8µs as margin
		* On ARM the ISR epilogue+prologue is estimated at 20 instructions - Give 1µs as margin
		*/
		min_ticks = __HAL_TIM_GET_COUNTER(&hStepperTim) + uint32_t(1 * (STEPPER_TIMER_TICKS_PER_US));
		
		/**
		* NB: If for some reason the stepper monopolizes the MPU, eventually the
		* timer will wrap around (and so will 'next_isr_ticks'). So, limit the
		* loop to 10 iterations. Beyond that, there's no way to ensure correct pulse
		* timing, since the MCU isn't fast enough.
		*/
		if (!--max_loops)
			next_isr_ticks = min_ticks;
		
		// Advance pulses if not enough time to wait for the next ISR
	} while (next_isr_ticks < min_ticks);
	
	// Now 'next_isr_ticks' contains the period to the next Stepper ISR - And we are
	// sure that the time has not arrived yet - Warrantied by the scheduler
	
	// Set the next ISR to fire at the proper time
	__HAL_TIM_SET_AUTORELOAD(&hStepperTim, uint32_t(next_isr_ticks));
	if (__HAL_TIM_GET_COUNTER(&hStepperTim) >= uint32_t(next_isr_ticks))
		hStepperTim.Instance->EGR |= TIM_EGR_UG; // Generate an immediate update interrupt
	
	// Don't forget to finally reenable interrupts
	__enable_irq();
}

/**
* This phase of the ISR should ONLY create the pulses for the steppers.
* This prevents jitter caused by the interval between the start of the
* interrupt and the start of the pulses. DON'T add any logic ahead of the
* call to this method that might cause variation in the timing. The aim
* is to keep pulse timing as regular as possible.
*/
void Stepper::pulse_phase_isr()
{
	
	// If we must abort the current block, do so!
	if (abort_current_block)
	{
		abort_current_block = false;
		if (current_block)
			discard_current_block();
	}
	
	// If there is no current block, do nothing
	if (!current_block)
		return;
	
	// Count of pending loops and events for this iteration
	const uint32_t pending_events = step_event_count - step_events_completed;
	uint8_t events_to_do = _MIN(pending_events, steps_per_isr);
	
	// Just update the value we will get at the end of the loop
	step_events_completed += events_to_do;
	
	// Take multiple steps per interrupt (For high speed moves)
#ifdef ISR_MULTI_STEPS
	bool firstStep = true;
	USING_TIMED_PULSE();
#endif
	bool step_needed;
	
	do
	{
		// Determine if a pulse is needed using Bresenham
		delta_error += advance_dividend;
		step_needed = (delta_error >= 0);
		if (step_needed)
		{
			count_position += count_direction;
			systemInfo.current_position_steps += count_direction;
			delta_error -= advance_divisor;
		}
		
		// Start an active pulse if needed
#ifdef ISR_MULTI_STEPS
		if (firstStep)
			firstStep = false;
		else
			AWAIT_LOW_PULSE();
#endif
		
		if (step_needed)
			HAL_GPIO_WritePin(Z_STEP_GPIO_Port, Z_STEP_Pin, (GPIO_PinState)1);
		
		// TODO: need to deal with MINIMUM_STEPPER_PULSE over i2s
#ifdef ISR_MULTI_STEPS
		START_HIGH_PULSE();
		AWAIT_HIGH_PULSE();
#endif
		
		if (step_needed)
			HAL_GPIO_WritePin(Z_STEP_GPIO_Port, Z_STEP_Pin, (GPIO_PinState)0);
		
#ifdef ISR_MULTI_STEPS
		if (events_to_do)
			START_LOW_PULSE();
#endif
		
	} while (--events_to_do);
}

// This is the last half of the stepper interrupt: This one processes and
// properly schedules blocks from the planner. This is executed after creating
// the step pulses, so it is not time critical, as pulses are already done.

uint32_t Stepper::block_phase_isr()
{
	
	// If no queued movements, just wait 1ms for the next block
	uint32_t interval = (STEPPER_TIMER_RATE) / 1000UL;
	
	// If there is a current block
	if (current_block)
	{
		
		// If current block is finished, reset pointer and finalize state
		if (step_events_completed >= step_event_count)
		{
			discard_current_block();
		}
		else
		{
			// Step events not completed yet...
			
			// Are we in acceleration phase ?
			if (step_events_completed <= accelerate_until)
			{ // Calculate new timer value
				
#ifdef S_CURVE_ACCELERATION
				// Get the next speed to use (Jerk limited!)
				uint32_t acc_step_rate = acceleration_time < current_block->acceleration_time ? _eval_bezier_curve(acceleration_time) : current_block->cruise_rate;
#else
				acc_step_rate = STEP_MULTIPLY(acceleration_time, current_block->acceleration_rate) + current_block->initial_rate;
				NOMORE(acc_step_rate, current_block->nominal_rate);
#endif
				
				// acc_step_rate is in steps/second
				
				// step_rate to timer interval and steps per stepper isr
				interval = calc_timer_interval(acc_step_rate, &steps_per_isr);
				acceleration_time += interval;
						
			}
			// Are we in Deceleration phase ?
			else if (step_events_completed > decelerate_after)
			{
				uint32_t step_rate;
				
#ifdef S_CURVE_ACCELERATION
				// If this is the 1st time we process the 2nd half of the trapezoid...
				if (!bezier_2nd_half)
				{
					// Initialize the Bézier speed curve
					_calc_bezier_curve_coeffs(current_block->cruise_rate, current_block->final_rate, current_block->deceleration_time_inverse);
					bezier_2nd_half = true;
					// The first point starts at cruise rate. Just save evaluation of the Bézier curve
					step_rate = current_block->cruise_rate;
				}
				else
				{
					// Calculate the next speed to use
					step_rate = deceleration_time < current_block->deceleration_time ? _eval_bezier_curve(deceleration_time) : current_block->final_rate;
				}
#else
				
				// Using the old trapezoidal control
				step_rate = STEP_MULTIPLY(deceleration_time, current_block->acceleration_rate);
				if (step_rate < acc_step_rate)
				{ // Still decelerating?
					step_rate = acc_step_rate - step_rate;
					NOLESS(step_rate, current_block->final_rate);
				}
				else
					step_rate = current_block->final_rate;
#endif
				
				// step_rate is in steps/second
				
				// step_rate to timer interval and steps per stepper isr
				interval = calc_timer_interval(step_rate, &steps_per_isr);
				deceleration_time += interval;
				
			}
			// Must be in cruise phase otherwise
			else
			{
				
				// Calculate the ticks_nominal for this nominal speed, if not done yet
				if (ticks_nominal < 0)
				{
					// step_rate to timer interval and loops for the nominal speed
					ticks_nominal = calc_timer_interval(current_block->nominal_rate, &steps_per_isr);
				}
				
				// The timer interval is just the nominal value for the nominal speed
				interval = ticks_nominal;
				
			}
		}
	}
	
	// If there is no current block at this point, attempt to pop one from the buffer
	// and prepare its movement
	if (!current_block)
	{
		
		// Anything in the buffer?
		if ((current_block = zPlanner.get_current_block()))
		{
			
			// Sync block? Sync the stepper counts and return
			while (TEST(current_block->flag, BLOCK_BIT_SYNC_POSITION))
			{
				_set_position(current_block->position);
				discard_current_block();
				
				// Try to get a new block
				if (!(current_block = zPlanner.get_current_block()))
					return interval; // No more queued movements!
			}
			
			// Flag all moving axes for proper endstop handling
			
#define Z_MOVE_TEST !!current_block->steps
			
			uint8_t axis_bits = 0;
			if (Z_MOVE_TEST) axis_bits = 1;
			//if (!!current_block->steps.e) SBI(axis_bits, E_AXIS);
			//if (!!current_block->steps.a) SBI(axis_bits, X_HEAD);
			//if (!!current_block->steps.b) SBI(axis_bits, Y_HEAD);
			//if (!!current_block->steps.c) SBI(axis_bits, Z_HEAD);
			axis_did_move = axis_bits;
			
			// No acceleration / deceleration time elapsed so far
			acceleration_time = deceleration_time = 0;
			
			
			// Based on the oversampling factor, do the calculations
			step_event_count = current_block->step_event_count;
			
			// Initialize Bresenham delta errors to 1/2
			delta_error = -int32_t(step_event_count);
			
			// Calculate Bresenham dividends and divisors
			advance_dividend = current_block->steps << 1;
			advance_divisor = step_event_count << 1;
			
			// No step events completed so far
			step_events_completed = 0;
			
			// Compute the acceleration and deceleration points
			accelerate_until = current_block->accelerate_until;
			decelerate_after = current_block->decelerate_after;
			
			if (current_block->direction_bits != last_direction_bits)
			{
				last_direction_bits = current_block->direction_bits;
				set_directions();
			}
			// At this point, we must ensure the movement about to execute isn't
			// trying to force the head against a limit switch. If using interrupt-
			// driven change detection, and already against a limit then no call to
			// the endstop_triggered method will be done and the movement will be
			// done against the endstop. So, check the limits here: If the movement
			// is against the limits, the block will be marked as to be killed, and
			// on the next call to this ISR, will be discarded.
			endstops.update();
			
			// Mark the time_nominal as not calculated yet
			ticks_nominal = -1;
			
#ifdef S_CURVE_ACCELERATION
			// Initialize the Bézier speed curve
			_calc_bezier_curve_coeffs(current_block->initial_rate, current_block->cruise_rate, current_block->acceleration_time_inverse);
			// We haven't started the 2nd half of the trapezoid
			bezier_2nd_half = false;
#else
			// Set as deceleration point the initial rate of the block
			acc_step_rate = current_block->initial_rate;
#endif
			
			// Calculate the initial timer interval
			interval = calc_timer_interval(current_block->initial_rate, &steps_per_isr);
		}
	}
	
	// Return the interval to wait
	return interval;
}

// Check if the given block is busy or not - Must not be called from ISR contexts
// The current_block could change in the middle of the read by an Stepper ISR, so
// we must explicitly prevent that!
bool Stepper::is_block_busy(const block_t* const block)
{
	block_t *vnew = current_block;
	
	// Return if the block is busy or not
	return block == vnew;
}

void Stepper::init()
{
	count_position = 0;
	
	// step pin
	HAL_GPIO_WritePin(Z_STEP_GPIO_Port, Z_STEP_Pin, (GPIO_PinState)0);
	// enable pin
	HAL_GPIO_WritePin(Z_ENA_GPIO_Port, Z_ENA_Pin, (GPIO_PinState)1);
	
	ENABLE_STEPPER_DRIVER_INTERRUPT();
	wake_up();
	sei();
	
	// Init direction bits for first moves
	last_direction_bits = cfgzMotor.invert_z_dir;
	set_directions();
	
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
void Stepper::_set_position(const int32_t c)
{
	count_position = c;
	systemInfo.current_position_steps = c;
}

/**
* Get a stepper's position in steps.
*/
int32_t Stepper::position()
{
	const int32_t v = count_position;
	return v;
}

// Set the current position in steps
void Stepper::set_position(const int32_t c)
{
	zPlanner.synchronize();
	const bool was_enabled = suspend();
	_set_position(c);
	if (was_enabled)
		wake_up();
}

void Stepper::set_axis_position(const int32_t v)
{
	zPlanner.synchronize();
	count_position = v;
}

// Signal endstops were triggered - This function can be called from
// an ISR context  (Temperature, Stepper or limits ISR), so we must
// be very careful here. If the interrupt being preempted was the
// Stepper ISR (this CAN happen with the endstop limits ISR) then
// when the stepper ISR resumes, we must be very sure that the movement
// is properly canceled
void Stepper::endstop_triggered()
{
	
	const bool was_enabled = suspend();
	endstops_trigsteps = count_position;
	
	// Discard the rest of the move if there is a current block
	quick_stop();
	
	if (was_enabled)
		wake_up();
}

int32_t Stepper::triggered_position()
{
	const int32_t v = endstops_trigsteps;
	return v;
}

