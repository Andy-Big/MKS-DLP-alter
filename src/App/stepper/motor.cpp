
#include "motor.h"

#include "config.h"
#include "z_planner.h"
#include "z_endstops.h"
#include "z_stepper.h"



extern uint8_t			zHoldTimer;
extern uint8_t			zDisTimer;




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
	zPlanner.settings.axis_steps_per_mm = 			cfgzMotor.steps_per_mm;
	zPlanner.settings.max_feedrate_mm_s = 			cfgzMotor.max_feedrate;
	zPlanner.settings.max_acceleration_mm_per_s2 =	cfgzMotor.max_acceleration;
	zPlanner.settings.acceleration = 				cfgzMotor.acceleration;
	zPlanner.settings.travel_acceleration = 		cfgzMotor.travel_acceleration;
	zPlanner.settings.min_feedrate_mm_s = 			cfgzMotor.min_feedrate;
	zPlanner.settings.min_travel_feedrate_mm_s = 	cfgzMotor.min_travel_feedrate;
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
	zEndstops.enable();
	
}
//==============================================================================




void			ZMOTOR_MotorEnable()
{
	HAL_GPIO_WritePin(Z_ENA_GPIO_Port, Z_ENA_Pin, (GPIO_PinState)0);
	TIM5->CCR1 = (uint32_t)(cfgzMotor.current_vref * 0.364);
	systemInfo.zmotor_enabled = 1;
}
//==============================================================================




void			ZMOTOR_MotorDisable()
{
	HAL_GPIO_WritePin(Z_ENA_GPIO_Port, Z_ENA_Pin, (GPIO_PinState)1);
	// Set motor current
	TIM5->CCR1 = 1;
	systemInfo.zmotor_enabled = 0;
	systemInfo.position_known = 0;
}
//==============================================================================




uint8_t			ZMOTOR_IsMotorEnabled()
{
	return systemInfo.zmotor_enabled;
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




void			ZMOTOR_SetCurrent(float current)
{
	if (current > 1000)
		current = 1000;
	cfgzMotor.current_vref = current;
	TIM5->CCR1 = (uint32_t)(cfgzMotor.current_vref * 0.364);
}
//==============================================================================




void			ZMOTOR_SetFullCurrent()
{
	TIM5->CCR1 = (uint32_t)(cfgzMotor.current_vref * 0.364);
}
//==============================================================================




void			ZMOTOR_SetHoldCurrent()
{
	TIM5->CCR1 = (uint32_t)(cfgzMotor.current_hold_vref * 0.364);
}
//==============================================================================




uint8_t			ZMOTOR_GetEndstopState()
{
	return zEndstops.state();
}
//==============================================================================




uint8_t			ZMOTOR_IsMoving()
{
	return (zStepper.axis_is_moving() || zPlanner.has_blocks_queued());
}
//==============================================================================




void			ZMOTOR_SetPosition(float pos)
{
	zPlanner.set_position_mm(pos);
}
//==============================================================================




void			ZMOTOR_Stop()
{
	zPlanner.quick_stop();
}
//==============================================================================




float			ZMOTOR_GetCurrentPosition()
{
	return (float)systemInfo.current_position_steps / (float)cfgzMotor.steps_per_mm;
}
//==============================================================================




void			ZMOTOR_EnableEndstops()
{
	zEndstops.enable();
}
//==============================================================================




void			ZMOTOR_DisableEndstops()
{
	zEndstops.disable();
}
//==============================================================================




void			ZMOTOR_StartHoming()
{
	systemInfo.position_known = 0;

	if (systemInfo.zmotor_enabled == 0)
		ZMOTOR_MotorEnable();
	
	ZMOTOR_SetFullCurrent();
	SYSTIMER_SetCountDown(zHoldTimer, TIMER_DISABLE);
	SYSTIMER_SetCountDown(zDisTimer, TIMER_DISABLE);
	
	ZMOTOR_EnableEndstops();

	ZMOTOR_SetPosition(0);
	systemInfo.target_position = 0;

	if (cfgzMotor.home_dir == -1)
	{
		if (ZMOTOR_GetEndstopState() & (1 << Z_MIN))
		{
			systemInfo.target_position += 20;
			ZMOTOR_MoveAbsolute(systemInfo.target_position, cfgzMotor.homing_feedrate_fast);
		}
	}
	else
	{
		if (ZMOTOR_GetEndstopState() & (1 << Z_MAX))
		{
			systemInfo.target_position -= 20;
			ZMOTOR_MoveAbsolute(systemInfo.target_position, cfgzMotor.homing_feedrate_fast);
		}
	}
	systemInfo.printer_state = PST_HOMING_PREUP;
}
//==============================================================================




