
#include "config.h"
#include "eeprom.h"





MOTOR_CONFIG			cfgzMotor;
SYSTEM_INFO				systemInfo;
GLOBAL_CONFIG			cfgConfig;
WORK_TIME				cfgTimers;









void			CFG_Init()
{
	memset(&systemInfo, 0, sizeof(SYSTEM_INFO));
	
	uint16_t	crc = 0;
	uint8_t		*data;

	// motor settings
	data = (uint8_t*)&cfgzMotor;
	if (EEPROM_ReadMemBuff(EEPR_ADDR_MOTORCONFIG, data, sizeof(MOTOR_CONFIG)) == 0)
	{
		CFG_SetMotorDefault();
		EEPROM_WriteMemBuff(EEPR_ADDR_MOTORCONFIG, (uint8_t*)&cfgzMotor, sizeof(MOTOR_CONFIG));
	}
	else
	{
		// calculate crc
		crc = 0;
		for (uint16_t i = 0; i < sizeof(MOTOR_CONFIG) - 2; i++)
		{
			crc += data[i];
		}
		if (cfgzMotor.cfg_version != FW_VERSION || crc != cfgzMotor.cfg_crc)
		{
			switch (cfgzMotor.cfg_version)
			{
				
				default:
					CFG_SetMotorDefault();
			}
			CFG_SaveMotor();
		}
	}

	// global settings
	data = (uint8_t*)&cfgConfig;
	if (EEPROM_ReadMemBuff(EEPR_ADDR_GLOBALCONFIG, data, sizeof(GLOBAL_CONFIG)) == 0)
	{
		CFG_SetConfigDefault();
		EEPROM_WriteMemBuff(EEPR_ADDR_GLOBALCONFIG, (uint8_t*)&cfgConfig, sizeof(GLOBAL_CONFIG));
	}
	else
	{
		// calculate crc
		crc = 0;
		for (uint16_t i = 0; i < sizeof(GLOBAL_CONFIG) - 2; i++)
		{
			crc += data[i];
		}
		if (cfgConfig.cfg_version != FW_VERSION || crc != cfgConfig.cfg_crc)
		{
			switch (cfgConfig.cfg_version)
			{
				
				default:
					CFG_SetConfigDefault();
			}
			CFG_SaveConfig();
		}
	}

	// work timers
	data = (uint8_t*)&cfgTimers;
	if (EEPROM_ReadMemBuff(EEPR_ADDR_WORKTIME, data, sizeof(WORK_TIME)) == 0)
	{
		memset(&cfgTimers, 0, sizeof(WORK_TIME));
		EEPROM_WriteMemBuff(EEPR_ADDR_WORKTIME, (uint8_t*)&cfgTimers, sizeof(WORK_TIME));
	}
	else
	{
		// calculate crc
		crc = 0;
		for (uint16_t i = 0; i < sizeof(WORK_TIME) - 2; i++)
		{
			crc += data[i];
		}
		if (cfgTimers.cfg_version != FW_VERSION || crc != cfgTimers.cfg_crc)
		{
			switch (cfgTimers.cfg_version)
			{
				
				default:
					CFG_SetTimersDefault();
			}
			CFG_SaveTimers();
		}
	}

}
//==============================================================================




void			CFG_SetMotorDefault()
{
	uint16_t	crc = 0;
	uint8_t		*data;
	data = (uint8_t*)&cfgzMotor;
	
	memset(data, 0, sizeof(MOTOR_CONFIG));

	cfgzMotor.cfg_version = FW_VERSION;
	
	cfgzMotor.invert_z_dir = 1;
	cfgzMotor.z_home_dir = -1;
	cfgzMotor.z_home_pos = 0;
	cfgzMotor.z_min_pos = -3;
	cfgzMotor.z_max_pos = 180;
	cfgzMotor.z_min_endstop_inverting = 1;
	cfgzMotor.z_max_endstop_inverting = 1;
	
	cfgzMotor.axis_steps_per_mm = 1600;
	cfgzMotor.max_feedrate_mm_s = 100;
	cfgzMotor.max_acceleration_mm_per_s2 = 100;

	cfgzMotor.homing_feedrate_z = 1;
	cfgzMotor.acceleration = 0.7;
	cfgzMotor.feedrate = 5;
	cfgzMotor.travel_acceleration = 40;
	cfgzMotor.travel_feedrate = 30;
	cfgzMotor.min_feedrate_mm_s = 0.01;
	cfgzMotor.min_travel_feedrate_mm_s = 0.01;
	cfgzMotor.max_jerk = 0.05;
	cfgzMotor.current_vref = 800;
	cfgzMotor.current_hold_vref = 300;
	cfgzMotor.hold_time = 10;
	
	for (uint16_t i = 0; i < sizeof(MOTOR_CONFIG) - 2; i++)
	{
		crc += data[i];
	}
	cfgzMotor.cfg_crc = crc;
}
//==============================================================================




void			CFG_SaveMotor()
{
	uint16_t	crc = 0;
	uint8_t		*data;
	data = (uint8_t*)&cfgzMotor;
	
	for (uint16_t i = 0; i < sizeof(MOTOR_CONFIG) - 2; i++)
	{
		crc += data[i];
	}
	cfgzMotor.cfg_crc = crc;
	
	EEPROM_WriteMemBuff(EEPR_ADDR_MOTORCONFIG, (uint8_t*)&cfgzMotor, sizeof(MOTOR_CONFIG));
	
}
//==============================================================================




void			CFG_SetConfigDefault()
{
	uint16_t	crc = 0;
	uint8_t		*data;
	data = (uint8_t*)&cfgConfig;

	memset(data, 0, sizeof(GLOBAL_CONFIG));
	cfgConfig.cfg_version = FW_VERSION;
	cfgConfig.language = 0;		// english default
	cfgConfig.z_zero_pos = cfgzMotor.z_home_pos;
	cfgConfig.z_pause_pos = 5.0;
	
	for (uint16_t i = 0; i < sizeof(GLOBAL_CONFIG) - 2; i++)
	{
		crc += data[i];
	}
	cfgConfig.cfg_crc = crc;
}
//==============================================================================




void			CFG_SaveConfig()
{
	uint16_t	crc = 0;
	uint8_t		*data;
	data = (uint8_t*)&cfgConfig;
	
	for (uint16_t i = 0; i < sizeof(GLOBAL_CONFIG) - 2; i++)
	{
		crc += data[i];
	}
	cfgConfig.cfg_crc = crc;
	
	EEPROM_WriteMemBuff(EEPR_ADDR_GLOBALCONFIG, (uint8_t*)&cfgConfig, sizeof(GLOBAL_CONFIG));
	
}
//==============================================================================




void			CFG_SetTimersDefault()
{
	uint16_t	crc = 0;
	uint8_t		*data;
	data = (uint8_t*)&cfgTimers;

	memset(&cfgTimers, 0, sizeof(WORK_TIME));
	cfgTimers.cfg_version = FW_VERSION;

	for (uint16_t i = 0; i < sizeof(WORK_TIME) - 2; i++)
	{
		crc += data[i];
	}
	cfgTimers.cfg_crc = crc;
}
//==============================================================================




void			CFG_SaveTimers()
{
	uint16_t	crc = 0;
	uint8_t		*data;
	data = (uint8_t*)&cfgTimers;
	
	for (uint16_t i = 0; i < sizeof(WORK_TIME) - 2; i++)
	{
		crc += data[i];
	}
	cfgTimers.cfg_crc = crc;
	
	EEPROM_WriteMemBuff(EEPR_ADDR_WORKTIME, (uint8_t*)&cfgTimers, sizeof(WORK_TIME));
	
}
//==============================================================================




