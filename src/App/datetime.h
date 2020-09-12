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

void		DTIME_GetCurrentDatetime(DATETIME_STRUCT *dt);
uint32_t	DTIME_GetCurrentUnixtime();
void		DTIME_SetCurrentDatetime(DATETIME_STRUCT *datetime);
void		DTIME_SetCurrentUnixtime(uint32_t unixtime);
uint32_t	DTIME_DatetimeToUnixtime(DATETIME_STRUCT *datetime);
void		DTIME_UnixtimeToDatetime(uint32_t secs, DATETIME_STRUCT *datetime);
void		DTIME_SetWeekDay(DATETIME_STRUCT *datetime);
void		DTIME_DatetimeAddSeconds(DATETIME_STRUCT *datetime, uint32_t seconds);
void		DTIME_DatetimeAddMinutes(DATETIME_STRUCT *datetime, uint32_t minutes);
void		DTIME_DatetimeAddHours(DATETIME_STRUCT *datetime, uint32_t hours);






#endif /*__datetime_H */

