#ifndef __motor_H
#define __motor_H

#include "includes.h"
#include "main.h"



void		idle();



void		_zmotor_Pins_Init();



void			ZMOTOR_Init();
void			ZMOTOR_MotorEnable();
void			ZMOTOR_MotorDisable();
uint8_t			ZMOTOR_IsMotorEnabled();
void			ZMOTOR_MoveRelative(float dist, float speed);
void			ZMOTOR_MoveAbsolute(float pos, float speed);

void			ZMOTOR_SetCurrent(float current);
void			ZMOTOR_SetFullCurrent();
void			ZMOTOR_SetHoldCurrent();
uint8_t			ZMOTOR_GetEndstopState();
uint8_t			ZMOTOR_IsMoving();
void			ZMOTOR_SetPosition(float pos);
void			ZMOTOR_Stop();
float			ZMOTOR_GetCurrentPosition();
void			ZMOTOR_EnableEndstops();
void			ZMOTOR_DisableEndstops();

#endif /*__motor_H */

