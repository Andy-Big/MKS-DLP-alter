#include "sys_timer.h"
#include "touch.h"


SYSTIMERS_STRUCT	Timers[TIMER_MAXTIMERS];






// Redefinition HAL 1ms timer
void		HAL_IncTick(void)
{
	uwTick += uwTickFreq;
	
	// Read Touch each 16 ms
	if ((uwTick & 0x0000000F) == 0)
		_touch_ReadCoords();

	for (uint8_t i = 0; i < TIMER_MAXTIMERS; i++)
	{
		if (Timers[i].active && Timers[i].msecs)
			Timers[i].msecs--;
	}
}

//==============================================================================




void		SYSTIMER_Init()
{
	uint8_t tim = 0;
	for (tim = 0; tim < TIMER_MAXTIMERS; tim++)
	{
		Timers[tim].active = 0;
		Timers[tim].msecs = 0;
	}
}
//==============================================================================	



// Start new countdown timer with starting value -msecs- and return timer number
uint8_t		SYSTIMER_NewCountDown(uint32_t msecs)
{
	uint8_t tim = 0;
	for (tim = 0; tim < TIMER_MAXTIMERS; tim++)
		if (!Timers[tim].active)
			break;
	if (tim < TIMER_MAXTIMERS)
	{
		Timers[tim].active = 1;
		Timers[tim].msecs = msecs;
		return tim;
	}
	else
		return INVALID_TIMER;
}
//==============================================================================	



// Stop and delete countdown timer by its number -timer-
void		SYSTIMER_DeleteCountDown(uint8_t timer)
{
	if (timer < TIMER_MAXTIMERS)
	{
		Timers[timer].active = 0;
		Timers[timer].msecs = 0;
	}
}
//==============================================================================	



// Return remain value countdown timer by its number -timer-
uint32_t		SYSTIMER_GetCountDown(uint8_t timer)
{
	if (timer < TIMER_MAXTIMERS)
	{
		if (Timers[timer].active)
			return Timers[timer].msecs;
	}
	return 0;
}
//==============================================================================	



// Set new value in countdown timer by its number -timer-
void		SYSTIMER_SetCountDown(uint8_t timer, uint32_t msecs)
{
	if (timer < TIMER_MAXTIMERS && Timers[timer].active)
		Timers[timer].msecs = msecs;
}
//==============================================================================	



