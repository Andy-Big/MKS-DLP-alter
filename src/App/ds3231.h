#ifndef __ds3231_H
#define __ds3231_H


#ifdef __MKSDLP_BOARD__


#include "includes.h"
#include "main.h"
#include "datetime.h"


#define DS3231_REG_SECONDS			(uint8_t)0x00
#define DS3231_REG_MINUTES			(uint8_t)0x01
#define DS3231_REG_HOURS			(uint8_t)0x02
#define DS3231_REG_HOURS_12_BIT		(uint8_t)(1 << 6)
#define DS3231_REG_HOURS_PM_BIT		(uint8_t)(1 << 5)
#define DS3231_REG_WEEKDAY			(uint8_t)0x03
#define DS3231_REG_DATE				(uint8_t)0x04
#define DS3231_REG_MONTH			(uint8_t)0x05
#define DS3231_REG_YEAR				(uint8_t)0x06
#define DS3231_REG_AL1SECONDS		(uint8_t)0x07
#define DS3231_REG_AL1MINUTES		(uint8_t)0x08
#define DS3231_REG_AL1HOURS			(uint8_t)0x09
#define DS3231_REG_AL1DATE			(uint8_t)0x0A
#define DS3231_REG_AL2MINUTES		(uint8_t)0x0B
#define DS3231_REG_AL2HOURS			(uint8_t)0x0C
#define DS3231_REG_AL2DATE			(uint8_t)0x0D
#define DS3231_REG_CONTROL			(uint8_t)0x0E
#define DS3231_REG_STATUS			(uint8_t)0x0F
#define DS3231_REG_AGIGNG			(uint8_t)0x10
#define DS3231_REG_TEMP_MSB			(uint8_t)0x11
#define DS3231_REG_TEMP_LSB			(uint8_t)0x12

#define DS3231_CTRL_EOSC_BIT		(uint8_t)(1 << 7)
#define DS3231_CTRL_BBSQW_BIT		(uint8_t)(1 << 6)
#define DS3231_CTRL_CONV_BIT		(uint8_t)(1 << 5)
#define DS3231_CTRL_RS2_BIT			(uint8_t)(1 << 4)
#define DS3231_CTRL_RS1_BIT			(uint8_t)(1 << 3)
#define DS3231_CTRL_INTCN_BIT		(uint8_t)(1 << 2)
#define DS3231_CTRL_A2IE_BIT		(uint8_t)(1 << 1)
#define DS3231_CTRL_A1IE_BIT		(uint8_t)(1 << 0)

#define DS3231_STATE_OSF_BIT		(uint8_t)(1 << 7)
#define DS3231_STATE_EN32_BIT		(uint8_t)(1 << 3)
#define DS3231_STATE_BSY_BIT		(uint8_t)(1 << 2)
#define DS3231_STATE_A2F_BIT		(uint8_t)(1 << 1)
#define DS3231_STATE_A1F_BIT		(uint8_t)(1 << 0)


typedef struct
{
	uint8_t		seconds;
	uint8_t		minutes;
	uint8_t		hours;
	uint8_t		week_day;
	uint8_t		date;
	uint8_t		month;
	uint8_t		year;
} DS_DATETIME;


uint8_t		DS3231_Init();
void		DS3231_Deinit();

uint8_t		DS3231_StartOsc(uint8_t bbsqw = 0);
uint8_t		DS3231_GetTime(DATETIME_STRUCT *dtime);
uint8_t		DS3231_SetTime(DATETIME_STRUCT *dtime, uint8_t ampm = 0);

	
#endif  // __MKSDLP_BOARD__

#endif  // __ds3231_H

