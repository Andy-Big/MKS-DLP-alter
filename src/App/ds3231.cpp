#ifdef __MKSDLP_BOARD__


#include "ds3231.h"
#include "ext_i2c.h"



#define	DS3231_ADDR				(uint8_t)0xD0

extern I2C_HandleTypeDef		hExtI2C;

uint8_t							ds_state = 0;


uint8_t		DS3231_Init()
{
	EXTI2C_Init();

	if (EXTI2C_CheckRead(DS3231_ADDR, DS3231_REG_STATUS, &ds_state) == 0)
		return 0;
	
	// if clock has stoped
	if ((ds_state & DS3231_STATE_OSF_BIT) == DS3231_STATE_OSF_BIT)
		return 2;
	
	return 1;
}
//==============================================================================




void		DS3231_Deinit()
{
	EXTI2C_Deinit();
}
//==============================================================================




uint8_t		DS3231_StartOsc(uint8_t bbsqw)
{
	uint8_t		val;
	// enable EOSC and set BBSQW
	val = EXTI2C_ReadReg(DS3231_ADDR, DS3231_REG_CONTROL);
	val &= ~(DS3231_CTRL_EOSC_BIT);
	if (bbsqw == 0)
		val &= ~(DS3231_CTRL_BBSQW_BIT);
	else
		val |= DS3231_CTRL_BBSQW_BIT;
		
	if (EXTI2C_WriteReg(DS3231_ADDR, DS3231_REG_CONTROL, val) == 0)
		return 0;

	// reset OSF (set to 0)
	val = EXTI2C_ReadReg(DS3231_ADDR, DS3231_REG_STATUS);
	val &= ~(DS3231_STATE_OSF_BIT);
	if (EXTI2C_WriteReg(DS3231_ADDR, DS3231_REG_STATUS, val) == 0)
		return 0;
	
	return 1;
}
//==============================================================================




uint8_t		DS3231_GetTime(DATETIME_STRUCT *dtime)
{
	if (dtime == 0)
		return 0;

	DS_DATETIME		ds_time;
	memset(dtime, 0, sizeof(DATETIME_STRUCT));
	memset(&ds_time, 0, sizeof(DS_DATETIME));
	
	if (EXTI2C_ReadBuff(DS3231_ADDR, DS3231_REG_SECONDS, (uint8_t*)&ds_time, sizeof(DS_DATETIME)) == 0)
		return 0;

	dtime->seconds = (ds_time.seconds >> 4) * 10 + (ds_time.seconds & 0x0F);
	dtime->minutes = (ds_time.minutes >> 4) * 10 + (ds_time.minutes & 0x0F);
	if (ds_time.hours & DS3231_REG_HOURS_12_BIT)
	{
		dtime->hours = (ds_time.hours & 0x0F);
		dtime->hours += ((ds_time.hours >> 4) & 0x01) * 10;
		if (ds_time.hours & DS3231_REG_HOURS_PM_BIT)
			dtime->hours += 12;
	}
	else
	{
		dtime->hours = ((ds_time.hours >> 4) & 0x03) * 10 + (ds_time.hours & 0x0F);
	}
	dtime->weekday = (ds_time.week_day & 0x07);
	dtime->date = ((ds_time.date >> 4) & 0x03) * 10 + (ds_time.date & 0x0F);
	dtime->month = ((ds_time.month >> 4) & 0x01) * 10 + (ds_time.month & 0x0F);
	dtime->year = (ds_time.year >> 4) * 10 + (ds_time.year & 0x0F);

	return 1;
}
//==============================================================================




uint8_t		DS3231_SetTime(DATETIME_STRUCT *dtime, uint8_t ampm)
{
	if (dtime == 0)
		return 0;

	DS_DATETIME		ds_time;
	memset(&ds_time, 0, sizeof(DS_DATETIME));

	ds_time.seconds = ((dtime->seconds / 10) << 4) | (dtime->seconds % 10);
	ds_time.minutes = ((dtime->minutes / 10) << 4) | (dtime->minutes % 10);
	ds_time.hours = ((dtime->hours / 10) << 4) | (dtime->hours % 10);
	if (ampm)
	{
		if (ds_time.hours == 0)
			ds_time.hours = 12;
		if (ds_time.hours > 12)
		{
			ds_time.hours -= 12;
			ds_time.hours |= DS3231_REG_HOURS_PM_BIT;
		}
		ds_time.hours |= DS3231_REG_HOURS_12_BIT;
	}
	ds_time.week_day = dtime->weekday;
	ds_time.date = ((dtime->date / 10) << 4) | (dtime->date % 10);
	ds_time.month = ((dtime->month / 10) << 4) | (dtime->month % 10);
	ds_time.year = ((dtime->year / 10) << 4) | (dtime->year % 10);

	if (EXTI2C_WriteBuff(DS3231_ADDR, DS3231_REG_SECONDS, (uint8_t*)&ds_time, sizeof(DS_DATETIME)) == 0)
		return 0;

	return 1;
}
//==============================================================================




#endif  // __MKSDLP_BOARD__
