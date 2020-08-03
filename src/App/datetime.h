#ifndef __datetime_H
#define __datetime_H


#include "main.h"


typedef struct
{
	uint8_t		year;
	uint8_t		month;
	uint8_t		date;
	uint8_t		weekday;
	uint8_t		hours;
	uint8_t		minutes;
	uint8_t		seconds;
} DATETIME_STRUCT;

uint8_t		DTIME_GetSeconds();
uint8_t		DTIME_GetMinutes();
uint8_t		DTIME_GetHours();

void		DTIME_GetDateTime(DATETIME_STRUCT *dt);
void		RTCTIMER_SetCurrentDateTime(DATETIME_STRUCT *datetime);
uint32_t	RTCTIMER_TimeToSeconds(DATETIME_STRUCT *datetime);
void		RTCTIMER_SecondsToTime(uint32_t secs, DATETIME_STRUCT *datetime);
void		RTCTIMER_SetWeekDay(DATETIME_STRUCT *datetime);
void		RTCTIMER_TimeAddSeconds(DATETIME_STRUCT *datetime, uint32_t seconds);
void		RTCTIMER_TimeAddMinutes(DATETIME_STRUCT *datetime, uint32_t minutes);
void		RTCTIMER_TimeAddHours(DATETIME_STRUCT *datetime, uint32_t hours);






#endif /*__datetime_H */

