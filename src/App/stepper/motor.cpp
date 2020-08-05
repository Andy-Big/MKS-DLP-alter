
#include "motor.h"

#include "config.h"
#include "z_planner.h"
#include "z_endstops.h"
#include "z_stepper.h"







void		idle()
{
	static uint32_t		idlecount = 0;
	idlecount++;
}








void		_zmotor_Pins_Init()
{

}
//==============================================================================





void		ZMOTOR_Init()
{

//*
	zPlanner.settings.axis_steps_per_mm = 			cfgzMotor.axis_steps_per_mm;
	zPlanner.settings.max_feedrate_mm_s = 			cfgzMotor.max_feedrate_mm_s;
	zPlanner.settings.max_acceleration_mm_per_s2 =	cfgzMotor.max_acceleration_mm_per_s2;
	zPlanner.settings.acceleration = 				cfgzMotor.acceleration;
	zPlanner.settings.travel_acceleration = 		cfgzMotor.travel_acceleration;
	zPlanner.settings.min_feedrate_mm_s = 			cfgzMotor.min_feedrate_mm_s;
	zPlanner.settings.min_travel_feedrate_mm_s = 	cfgzMotor.min_travel_feedrate_mm_s;
	zPlanner.settings.max_jerk = 					cfgzMotor.max_jerk;
/**/
/*
	zPlanner.axis_steps_per_mm = 			cfgzMotor.axis_steps_per_mm;
	zPlanner.max_feedrate_mm_s = 			cfgzMotor.max_feedrate_mm_s;
	zPlanner.max_acceleration_mm_per_s2 =	cfgzMotor.max_acceleration_mm_per_s2;
	zPlanner.acceleration = 				cfgzMotor.acceleration;
	zPlanner.travel_acceleration = 		cfgzMotor.travel_acceleration;
	zPlanner.min_feedrate_mm_s = 			cfgzMotor.min_feedrate_mm_s;
	zPlanner.min_travel_feedrate_mm_s = 	cfgzMotor.min_travel_feedrate_mm_s;
	zPlanner.max_jerk = 					cfgzMotor.max_jerk;
/**/
	
	zEndstops.init();
	zPlanner.init();
	zPlanner.refresh_positioning();
	zStepper.init();

	// Z motor reference voltage out PWM
	TIM_ZRef_Init();
	HAL_TIM_PWM_Start(&hZRefTim, TIM_CHANNEL_1);
	hZRefTim.Instance->CCR1 = 0;
	HAL_TIM_Base_MspInit(&hZRefTim);
	
	// Stepper timer
	TIM_ZStepper_Init();
	HAL_TIM_Base_MspInit(&hStepperTim);
	HAL_TIM_Base_Start_IT(&hStepperTim);

	// Set motor current
	TIM5->CCR1 = cfgzMotor.current_vref < 1000 ? (uint32_t)(cfgzMotor.current_vref * 0.364) : 364;
	
	ZMOTOR_MotorDisable();
	
}
//==============================================================================




void			ZMOTOR_MotorEnable()
{
	HAL_GPIO_WritePin(Z_ENA_GPIO_Port, Z_ENA_Pin, (GPIO_PinState)0);
	// Set motor current
	TIM5->CCR1 = cfgzMotor.current_vref < 1000 ? (uint32_t)(cfgzMotor.current_vref * 0.364) : 364;
}
//==============================================================================




void			ZMOTOR_MotorDisable()
{
	HAL_GPIO_WritePin(Z_ENA_GPIO_Port, Z_ENA_Pin, (GPIO_PinState)1);
	// Set motor current
	TIM5->CCR1 = 1;
}
//==============================================================================




void			ZMOTOR_MoveRelative(float dist, float speed)
{
}
//==============================================================================




void			ZMOTOR_MoveAbsolute(float pos, float speed)
{
	zPlanner.buffer_segment(pos, speed);
}
//==============================================================================




