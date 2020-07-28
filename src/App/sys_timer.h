#ifndef __sys_timer_H
#define __sys_timer_H


#include "includes.h"
#include "main.h"


#define TIMER_MAXTIMERS		(uint8_t)32
#define INVALID_TIMER		(uint8_t)0xFF

typedef struct
{
	uint32_t	msecs;
	uint8_t		active;
} SYSTIMERS_STRUCT;

void		SYSTIMER_Init();

// Start new countdown timer with starting value -msecs- and return timer number
uint8_t		SYSTIMER_NewCountDown(uint32_t msecs);

// Stop and delete countdown timer by its number -timer-
void		SYSTIMER_DeleteCountDown(uint8_t timer);

// Return remain value countdown timer by its number -timer-
uint32_t	SYSTIMER_GetCountDown(uint8_t timer);

// Set new value in countdown timer by its number -timer-
void		SYSTIMER_SetCountDown(uint8_t timer, uint32_t msecs);





#endif /*__sys_timer_H */

