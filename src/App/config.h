#ifndef __config_H
#define __config_H

#include "includes.h"
#include "main.h"




#define Z_MIN_Pin			GPIO_PIN_0
#define Z_MIN_GPIO_Port		GPIOC

#define Z_MAX_Pin			GPIO_PIN_2
#define Z_MAX_GPIO_Port		GPIOC

#define Z_ENA_Pin			GPIO_PIN_2
#define Z_ENA_GPIO_Port		GPIOA

#define Z_DIR_Pin			GPIO_PIN_1
#define Z_DIR_GPIO_Port		GPIOC

#define Z_STEP_Pin			GPIO_PIN_1
#define Z_STEP_GPIO_Port	GPIOA








typedef struct
{
	uint8_t			invert_z_dir;
	int8_t			z_home_dir;
	float			z_min_pos;
	float			z_max_pos;
	uint8_t			min_software_endstops;
	uint8_t			max_software_endstops;
	float			homing_feedrate_z;
	uint8_t			z_min_endstop_inverting;
	uint8_t			z_max_endstop_inverting;
	int16_t			current_vref_z;
	float			z_pause_pos;

	float			axis_steps_per_mm;
	float			max_feedrate_mm_s;
	unsigned long	max_acceleration_mm_per_s2;
	float			acceleration;
	float			travel_acceleration;
	float			min_feedrate_mm_s;
	float			min_travel_feedrate_mm_s;
	float			max_jerk;
	float			current_vref;

} MOTOR_CONFIG;



typedef struct
{
	int32_t			current_position_steps;
	float			target_position;
	uint8_t			is_printing;
	uint8_t			position_known;

} SYSTEM_STATE;




extern SYSTEM_STATE			systemState;
extern MOTOR_CONFIG			cfgzMotor;





void			CFG_Init();

	

#endif /*__config_H */

