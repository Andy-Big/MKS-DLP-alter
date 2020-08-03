/*******************************************************************************
*
*     Base user interface for LCD with touch
*
********************************************************************************/

#include "datetime.h"


#define _TBIAS_DAYS		((70 * (uint32_t)365) + 17)
#define _TBIAS_SECS		(_TBIAS_DAYS * (xtime_t)86400)
#define	_TBIAS_YEAR		1900
#define MONTAB(year)		((((year) & 03) || ((year) == 0)) ? mos : lmos)

const int16_t	lmos[] = {0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335};
const int16_t	mos[] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};

#define	Daysto32(year, mon)	(((year - 1) / 4) + MONTAB(year)[mon])



	
uint8_t		DTIME_GetSeconds()
{
	uint32_t 	val = RTC->TR;
	val = (uint8_t)(val & (RTC_TR_ST | RTC_TR_SU));
	val = (uint8_t)RTC_Bcd2ToByte(val);

	return val;
}
//==============================================================================



uint8_t		DTIME_GetMinutes()
{
	uint32_t 	val = RTC->TR;
	val = (uint8_t)((val & (RTC_TR_MNT | RTC_TR_MNU)) >> 8U);
	val = (uint8_t)RTC_Bcd2ToByte(val);

	return val;
}
//==============================================================================



uint8_t		DTIME_GetHours()
{
	uint32_t 	val = RTC->TR;
	val = (uint8_t)((val & (RTC_TR_HT | RTC_TR_HU)) >> 16U);
	val = (uint8_t)RTC_Bcd2ToByte(val);

	return val;
}
//==============================================================================



void		DTIME_GetDateTime(DATETIME_STRUCT *dt)
{
	uint32_t 	time = RTC->TR;
	uint32_t	date = RTC->DR;
	
	dt->year = (uint8_t)((date & (RTC_DR_YT | RTC_DR_YU)) >> 16U);
	dt->month = (uint8_t)((date & (RTC_DR_MT | RTC_DR_MU)) >> 8U);
	dt->date = (uint8_t)(date & (RTC_DR_DT | RTC_DR_DU));
	dt->weekday = (uint8_t)((date & (RTC_DR_WDU)) >> 13U);
	/* Convert the date structure parameters to Binary format */
	dt->year = (uint8_t)RTC_Bcd2ToByte(dt->year);
	dt->month = (uint8_t)RTC_Bcd2ToByte(dt->month);
	dt->date = (uint8_t)RTC_Bcd2ToByte(dt->date);

	dt->hours = (uint8_t)((time & (RTC_TR_HT | RTC_TR_HU)) >> 16U);
	dt->minutes = (uint8_t)((time & (RTC_TR_MNT | RTC_TR_MNU)) >> 8U);
	dt->seconds = (uint8_t)(time & (RTC_TR_ST | RTC_TR_SU));
	/* Convert the time structure parameters to Binary format */
	dt->hours = (uint8_t)RTC_Bcd2ToByte(dt->hours);
	dt->minutes = (uint8_t)RTC_Bcd2ToByte(dt->minutes);
	dt->seconds = (uint8_t)RTC_Bcd2ToByte(dt->seconds);


	return;
}
//==============================================================================



void		DTIME_SetCurrentDateTime(DATETIME_STRUCT *datetime)
{
	uint32_t	date = 0, time = 0;

	time = (uint32_t)(((uint32_t)RTC_ByteToBcd2(datetime->hours) << 16U) | ((uint32_t)RTC_ByteToBcd2(datetime->minutes) << 8U) | ((uint32_t)RTC_ByteToBcd2(datetime->seconds)));
	date = (((uint32_t)RTC_ByteToBcd2(datetime->year) << 16U) | ((uint32_t)RTC_ByteToBcd2(datetime->month) << 8U) | ((uint32_t)RTC_ByteToBcd2(datetime->date)) | ((uint32_t)datetime->weekday << 13U));
	
	// Unblock write protection RTC registers
	RTC->WPR = 0xCAU;
	RTC->WPR = 0x53U;
	
	uint32_t tickstart = 0;
	/* Check if the Initialization mode is set */
	if((RTC->ISR & RTC_ISR_INITF) == (uint32_t)RESET)
	{
		/* Set the Initialization mode */
		RTC->ISR = (uint32_t)RTC_INIT_MASK;

		/* Get tick */
		tickstart = HAL_GetTick();

		/* Wait till RTC is in INIT state and if Time out is reached exit */
		while((RTC->ISR & RTC_ISR_INITF) == (uint32_t)RESET)
		{
			if((HAL_GetTick() - tickstart ) > 1000)
				return;
		}
	}

	RTC->DR = (uint32_t)(date & RTC_DR_RESERVED_MASK);
	RTC->TR = (uint32_t)(time & RTC_TR_RESERVED_MASK);

	/* Exit Initialization mode */
	RTC->ISR &= (uint32_t)~RTC_ISR_INIT;

	/* If  CR_BYPSHAD bit = 0, wait for synchro else this check is not needed */
	if((RTC->CR & RTC_CR_BYPSHAD) == RESET)
	{
		tickstart = 0;
		/* Clear RSF flag */
		RTC->ISR &= (uint32_t)RTC_RSF_MASK;
		/* Get tick */
		tickstart = HAL_GetTick();
		/* Wait the registers to be synchronised */
		while((RTC->ISR & RTC_ISR_RSF) == (uint32_t)RESET)
		{
			if((HAL_GetTick() - tickstart ) > 1000)
			{
				break;
			}
		}
	}


	// Block write protection RTC registers
	RTC->WPR = 0xFFU;
}
//==============================================================================	




uint32_t	DTIME_TimeToSeconds(DATETIME_STRUCT *datetime)
{
	int32_t		days;
	uint32_t	secs;
	int32_t		mon, year;

	if (datetime->month == 0 || datetime->date == 0)
		return 0;

	/* Calculate number of days. */
	mon = datetime->month - 1;
	year = datetime->year + 2000 - _TBIAS_YEAR;
	days  = Daysto32(year, mon) - 1;
	days += 365 * year;
	days += datetime->date;
	days -= _TBIAS_DAYS;

	/* Calculate number of seconds. */
	secs  = 3600 * datetime->hours;
	secs += 60 * datetime->minutes;
	secs += datetime->seconds;

	secs += ((uint32_t)days * 86400);

	return secs;
}
//==============================================================================	




void		DTIME_SecondsToTime(uint32_t secs, DATETIME_STRUCT *datetime)
{
	int32_t			days = _TBIAS_DAYS;
	int32_t			mon;
	int32_t			year;
	int32_t			i;
	const int16_t*	pm;

	days = secs / 86400 + 4;
	days = (days % 7) + 1;
	datetime->weekday = days;
	/* days, hour, min, sec */
	days = _TBIAS_DAYS + secs / 86400;
	secs = secs % 86400;
	datetime->hours = secs / 3600;
	secs %= 3600;
	datetime->minutes = secs / 60;
	datetime->seconds = secs % 60;

		/* determine year */
	for (year = days / 365; days < (i = Daysto32(year, 0) + 365 * year); )
	{
		--year;
	}
	days -= i;
	datetime->year = year + _TBIAS_YEAR - 2000;

		/* determine month */
	pm = MONTAB(year);
	for (mon = 12; days < pm[--mon]; );
	datetime->month = mon + 1;
	datetime->date = days - pm[mon] + 1;
}
//==============================================================================	




void		DTIME_SetWeekDay(DATETIME_STRUCT *datetime)
{
	int32_t		days;
	int32_t		mon, year;
	
	if (datetime->month == 0 || datetime->date == 0)
		return;

	/* Calculate number of days. */
	mon = datetime->month - 1;
	year = datetime->year + 2000 - _TBIAS_YEAR;
	days  = Daysto32(year, mon) - 1;
	days += 365 * year;
	days += datetime->date;
	days -= _TBIAS_DAYS;

	days = (days + 4) % 7;
	datetime->weekday = days + 1;

	return;
}
//==============================================================================	




void		DTIME_TimeAddSeconds(DATETIME_STRUCT *datetime, uint32_t seconds)
{
	uint32_t secs = DTIME_TimeToSeconds(datetime) + seconds;
	DTIME_SecondsToTime(secs, datetime);
}
//==============================================================================	




void		DTIME_TimeAddMinutes(DATETIME_STRUCT *datetime, uint32_t minutes)
{
	uint32_t secs = DTIME_TimeToSeconds(datetime) + minutes * 60;
	DTIME_SecondsToTime(secs, datetime);
}
//==============================================================================	




void		DTIME_TimeAddHours(DATETIME_STRUCT *datetime, uint32_t hours)
{
	uint32_t secs = DTIME_TimeToSeconds(datetime) + hours * 3600;
	DTIME_SecondsToTime(secs, datetime);
}
//==============================================================================	




