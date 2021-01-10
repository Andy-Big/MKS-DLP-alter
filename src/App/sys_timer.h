#ifndef __sys_timer_H
#define __sys_timer_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "includes.h"
#include "main.h"


#define BUZZER_On()				BUZZER_GPIO_Port->BSRR = BUZZER_Pin						// Set pin
#define BUZZER_Off()			BUZZER_GPIO_Port->BSRR = (uint32_t)BUZZER_Pin << 16U	// Reset pin

#define UVLED_On()				LED_UV_GPIO_Port->BSRR = LED_UV_Pin						// Set pin
#define UVLED_Off()				LED_UV_GPIO_Port->BSRR = (uint32_t)LED_UV_Pin << 16U	// Reset pin

#define UVFAN_On()				FAN_LED_GPIO_Port->BSRR = FAN_LED_Pin					// Set pin
#define UVFAN_Off()				FAN_LED_GPIO_Port->BSRR = (uint32_t)FAN_LED_Pin << 16U	// Reset pin

#define MBFAN_On()				FAN_MB_GPIO_Port->BSRR = FAN_MB_Pin						// Set pin
#define MBFAN_Off()				FAN_MB_GPIO_Port->BSRR = (uint32_t)FAN_MB_Pin << 16U	// Reset pin



	

#define TIMER_MAXTIMERS		(uint8_t)32
#define INVALID_TIMER		(uint8_t)0xFF
#define TIMER_DISABLE		(uint32_t)0xFFFFFFFF

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


void		BUZZ_TimerOn(uint32_t time);
void		UVLED_TimerOn(uint32_t time);
uint32_t	UVLED_TimerState();
void		UVPAUSE_TimerOn(uint32_t time);
uint32_t	UVPAUSE_TimerState();
void		UVFAN_TimerOn(uint32_t time);
uint32_t	UVFAN_TimerState();


#ifdef __cplusplus
}
#endif

#endif /*__sys_timer_H */

