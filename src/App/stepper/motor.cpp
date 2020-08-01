
#include "motor.h"

#include "z_planner.h"
#include "z_endstops.h"
#include "z_stepper.h"





MOTOR_STATE			zmotorState;





void		_zmotor_Pins_Init()
{
	// Pins configure
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	__HAL_RCC_GPIOE_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOH_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	HAL_GPIO_WritePin(Z_DIR_GPIO_Port, Z_DIR_Pin, GPIO_PIN_SET);

	HAL_GPIO_WritePin(GPIOA, Z_STEP_Pin|Z_ENA_Pin, GPIO_PIN_SET);

	GPIO_InitStruct.Pin = Z_MIN_Pin|Z_MAX_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = Z_DIR_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
	HAL_GPIO_Init(Z_DIR_GPIO_Port, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = Z_STEP_Pin|Z_ENA_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

}
//==============================================================================





void		ZMOTOR_Init()
{
	memset(&zmotorState, 0, sizeof(MOTOR_STATE));
	
	_zmotor_Pins_Init();

	zPlanner.axis_steps_per_mm = 		cfgzMotor.axis_steps_per_mm;
	zPlanner.max_feedrate_mm_s = 		cfgzMotor.max_feedrate_mm_s;
	zPlanner.max_acceleration_mm_per_s2 =	cfgzMotor.max_acceleration_mm_per_s2;
	zPlanner.acceleration = 				cfgzMotor.acceleration;
	zPlanner.retract_acceleration = 		cfgzMotor.retract_acceleration;
	zPlanner.travel_acceleration = 		cfgzMotor.travel_acceleration;
	zPlanner.min_feedrate_mm_s = 		cfgzMotor.min_feedrate_mm_s;
	zPlanner.min_travel_feedrate_mm_s = 	cfgzMotor.min_travel_feedrate_mm_s;
	zPlanner.max_jerk = 					cfgzMotor.max_jerk;

	
	zPlanner.init();
	zPlanner.refresh_positioning();
	zStepper.init();

	// Z motor reference voltage out PWM
	TIM_ZRef_Init();
	HAL_TIM_PWM_Start(&hZRefTim, TIM_CHANNEL_1);
	hZRefTim.Instance->CCR1 = 0;
	hZRefTim.Instance->CCR1 = 500*0.364;
	HAL_TIM_Base_MspInit(&hZRefTim);
	
	// Stepper timer
	TIM_ZStepper_Init();
	HAL_TIM_Base_MspInit(&hStepperTim);
	HAL_TIM_Base_Start_IT(&hStepperTim);
	// Set motor current
	TIM5->CCR1 = cfgzMotor.current_vref < 1000 ? (uint32_t)(cfgzMotor.current_vref * 0.364) : 364;
	
	
}
//==============================================================================




void			ZMOTOR_MotorEnable()
{
	HAL_GPIO_WritePin(Z_ENA_GPIO_Port, Z_ENA_Pin, (GPIO_PinState)0);
}
//==============================================================================




void			ZMOTOR_MotorDisable()
{
	HAL_GPIO_WritePin(Z_ENA_GPIO_Port, Z_ENA_Pin, (GPIO_PinState)1);
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




