#ifndef __motor_H
#define __motor_H

#include "includes.h"
#include "main.h"
#include "config.h"




typedef struct
{
	float			current_position;
	uint8_t			is_printing;
} MOTOR_STATE;




extern MOTOR_STATE		zmotorState;


void		_zmotor_Pins_Init();



void			ZMOTOR_Init();
void			ZMOTOR_MotorEnable();
void			ZMOTOR_MotorDisable();
void			ZMOTOR_MoveRelative(float dist, float speed);
void			ZMOTOR_MoveAbsolute(float pos, float speed);
	

#endif /*__motor_H */

