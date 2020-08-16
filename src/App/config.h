#ifndef __config_H
#define __config_H

#include "includes.h"
#include "main.h"





typedef enum
{
	PST_IDLE = 0,
	PST_HOMING_PREUP,
	PST_HOMING_FAST,
	PST_HOMING_UP,
	PST_HOMING_SLOW,
	PST_FREEMOVING_UP,
	PST_FREEMOVING_DOWN,
	
} PRINTER_STATE;




typedef struct
{
	uint8_t			invert_z_dir;
	int8_t			z_home_dir;
	float			z_home_pos;
	float			z_zero_pos;
	float			z_min_pos;
	float			z_max_pos;
	uint8_t			z_min_endstop_inverting;
	uint8_t			z_max_endstop_inverting;
	int16_t			current_vref_z;
	float			z_pause_pos;

	float			axis_steps_per_mm;
	float			max_feedrate_mm_s;
	unsigned long	max_acceleration_mm_per_s2;
	float			min_feedrate_mm_s;
	float			min_travel_feedrate_mm_s;
	float			max_jerk;

	float			acceleration;
	float			feedrate;
	float			travel_acceleration;
	float			travel_feedrate;
	float			homing_feedrate_z;
	
	float			current_vref;
	float			current_hold_vref;
	uint32_t		hold_time;		// time after the last movement until the motor current decreases by half, in msec

} MOTOR_CONFIG;



typedef struct
{
	PRINTER_STATE	printer_state;
	int32_t			current_position_steps;
	float			target_position;
	uint8_t			is_printing;
	uint8_t			position_known;
	uint8_t			zmotor_enabled;

} SYSTEM_INFO;




extern SYSTEM_INFO			systemInfo;
extern MOTOR_CONFIG			cfgzMotor;





void			CFG_Init();

	

#endif /*__config_H */

