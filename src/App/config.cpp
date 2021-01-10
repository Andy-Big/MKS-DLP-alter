#include "config.h"
#include "eeprom.h"
#include "languages.h"
#include "tgui_messagebox.h"
#include "z_planner.h"
#include "z_endstops.h"
#include "z_stepper.h"




MOTOR_CONFIG			cfgMotor;
SYSTEM_INFO				systemInfo;
GLOBAL_CONFIG			cfgConfig;
WORK_TIME				cfgTimers;
char					cfgCFileName[512];
TCHAR					cfgTFileName[512];

extern char				msg[512];
extern FIL				ufile;
extern FIL				sfile;








void			CFG_Init()
{
	memset(&systemInfo, 0, sizeof(SYSTEM_INFO));
	
	uint16_t	crc = 0;
	uint8_t		*data;
	uint8_t 	need_update = 0;

	// motor settings
	need_update = 0;
	data = (uint8_t*)&cfgMotor;
	if (EEPROM_ReadMemBuff(EEPR_ADDR_MOTORCONFIG, data, sizeof(MOTOR_CONFIG)) == 0)
	{
		CFG_SetMotorDefault();
		EEPROM_WriteMemBuff(EEPR_ADDR_MOTORCONFIG, (uint8_t*)&cfgMotor, sizeof(MOTOR_CONFIG));
	}
	else
	{
		// check version
		if (cfgMotor.cfg_version < FW_VERSION)
		{
			CFG_RefreshMotor();
			need_update = 1;
		}
		if (need_update)
		{
			CFG_SaveMotor();
		}
		else
		{
			// check crc
			crc = CFG_MotorCalculateCRC();
			if (crc != cfgMotor.cfg_crc)
			{
				CFG_SetMotorDefault();
				CFG_SaveMotor();
			}
		}
	}

	
	// global settings
	need_update = 0;
	data = (uint8_t*)&cfgConfig;
	if (EEPROM_ReadMemBuff(EEPR_ADDR_GLOBALCONFIG, data, sizeof(GLOBAL_CONFIG)) == 0)
	{
		CFG_SetConfigDefault();
		EEPROM_WriteMemBuff(EEPR_ADDR_GLOBALCONFIG, (uint8_t*)&cfgConfig, sizeof(GLOBAL_CONFIG));
	}
	else
	{
		// check version
		if (cfgConfig.cfg_version < 0x0002)
		{
			cfgConfig.use_ext_clock = 0;
			need_update = 1;
		}
		if (cfgConfig.cfg_version < 0x0003)
		{
			cfgConfig.touch_cal[0] = 1;
			cfgConfig.touch_cal[1] = 0;
			cfgConfig.touch_cal[2] = 0;
			cfgConfig.touch_cal[3] = 0;
			cfgConfig.touch_cal[4] = 1;
			cfgConfig.touch_cal[5] = 0;
			need_update = 1;
		}
		if (cfgConfig.cfg_version < 0x0006)
		{
			cfgConfig.mb_fan_mode = 1;
			need_update = 1;
		}
		if (cfgConfig.cfg_version < FW_VERSION)
		{
			need_update = 1;
		}
		if (need_update)
		{
			CFG_RefreshConfig();
			CFG_SaveConfig();
		}
		else
		{
			// check crc
			crc = CFG_ConfigCalculateCRC();
			if (crc != cfgConfig.cfg_crc)
			{
				CFG_SetConfigDefault();
				CFG_SaveConfig();
			}
		}
	}

	// work timers
	need_update = 0;
	data = (uint8_t*)&cfgTimers;
	if (EEPROM_ReadMemBuff(EEPR_ADDR_WORKTIME, data, sizeof(WORK_TIME)) == 0)
	{
		memset(&cfgTimers, 0, sizeof(WORK_TIME));
		EEPROM_WriteMemBuff(EEPR_ADDR_WORKTIME, (uint8_t*)&cfgTimers, sizeof(WORK_TIME));
	}
	else
	{
		// check version
		if (cfgTimers.cfg_version < FW_VERSION)
		{
			need_update = 1;
		}
		if (need_update)
		{
			CFG_RefreshTimers();
			CFG_SaveTimers();
		}
		else
		{
			// check crc
			crc = CFG_TimersCalculateCRC();
			if (crc != cfgTimers.cfg_crc)
			{
				CFG_SetTimersDefault();
				CFG_SaveTimers();
			}
		}
	}

}
//==============================================================================




uint16_t		CFG_MotorCalculateCRC()
{
	uint8_t 	*data = (uint8_t*)&cfgMotor;
	uint16_t	crc = 0;
	for (uint16_t i = 2; i < sizeof(MOTOR_CONFIG); i++)
	{
		crc += data[i];
	}
	return crc;
}
//==============================================================================




uint16_t		CFG_ConfigCalculateCRC()
{
	uint8_t 	*data = (uint8_t*)&cfgConfig;
	uint16_t	crc = 0;
	uint32_t	dlen = sizeof(GLOBAL_CONFIG);
	switch(cfgConfig.cfg_version)
	{
		case 0x0002:
			dlen = sizeof(GLOBAL_CONFIG_V002);
	}
	for (uint16_t i = 2; i < dlen; i++)
	{
		crc += data[i];
	}
	return crc;
}
//==============================================================================




uint16_t		CFG_TimersCalculateCRC()
{
	uint8_t 	*data = (uint8_t*)&cfgTimers;
	uint16_t	crc = 0;
	for (uint16_t i = 2; i < sizeof(WORK_TIME); i++)
	{
		crc += data[i];
	}
	return crc;
}
//==============================================================================




void			CFG_SetMotorDefault()
{
	memset(&cfgMotor, 0, sizeof(MOTOR_CONFIG));

	cfgMotor.cfg_version = FW_VERSION;
	
	cfgMotor.invert_dir = 1;
	cfgMotor.home_dir = -1;
	cfgMotor.home_pos = 0;
	cfgMotor.min_pos = -3;
	cfgMotor.max_pos = 180;
	cfgMotor.min_endstop_inverting = 1;
	cfgMotor.max_endstop_inverting = 1;
	
	cfgMotor.steps_per_mm = 1600;
	cfgMotor.max_feedrate = 100;
	cfgMotor.max_acceleration = 100;

	cfgMotor.homing_feedrate_fast = 6;
	cfgMotor.homing_feedrate_slow = 1;
	cfgMotor.acceleration = 0.7;
	cfgMotor.feedrate = 5;
	cfgMotor.travel_acceleration = 20;
	cfgMotor.travel_feedrate = 20;
	cfgMotor.min_feedrate = 0.01;
	cfgMotor.min_travel_feedrate = 0.01;
	cfgMotor.max_jerk = 0.05;
	cfgMotor.current_vref = 800;
	cfgMotor.current_hold_vref = 300;
	cfgMotor.hold_time = 30000;
	cfgMotor.off_time = 1800000;
	
	cfgMotor.cfg_crc = CFG_MotorCalculateCRC();
}
//==============================================================================




void			CFG_RefreshMotor()
{
	cfgMotor.cfg_version = FW_VERSION;
	
	cfgMotor.cfg_crc = CFG_MotorCalculateCRC();
}
//==============================================================================




void			CFG_SaveMotor()
{
	cfgMotor.cfg_crc = CFG_MotorCalculateCRC();
	
	EEPROM_WriteMemBuff(EEPR_ADDR_MOTORCONFIG, (uint8_t*)&cfgMotor, sizeof(MOTOR_CONFIG));
	
}
//==============================================================================




void			CFG_SetConfigDefault()
{
	memset(&cfgConfig, 0, sizeof(GLOBAL_CONFIG));
	cfgConfig.cfg_version = FW_VERSION;
	cfgConfig.language = 0;		// english default
	cfgConfig.zero_pos = cfgMotor.home_pos;
	cfgConfig.pause_lift = 40.0;
	cfgConfig.end_lift = 40.0;
	cfgConfig.buzzer = 1;
	cfgConfig.buzzer_msg = 800;
	cfgConfig.buzzer_touch = 50;
	cfgConfig.screensaver_time = 600000;
	cfgConfig.screensaver_type = 1;
	cfgConfig.display_rotate = 0;
	cfgConfig.use_ext_clock = 0;

	cfgConfig.touch_cal[0] = 1;
	cfgConfig.touch_cal[1] = 0;
	cfgConfig.touch_cal[2] = 0;
	cfgConfig.touch_cal[3] = 0;
	cfgConfig.touch_cal[4] = 1;
	cfgConfig.touch_cal[5] = 0;

	cfgConfig.cfg_crc = CFG_ConfigCalculateCRC();
}
//==============================================================================




void			CFG_RefreshConfig()
{
	cfgConfig.cfg_version = FW_VERSION;

	cfgConfig.cfg_crc = CFG_ConfigCalculateCRC();
}
//==============================================================================




void			CFG_SaveConfig()
{
	cfgConfig.cfg_crc = CFG_ConfigCalculateCRC();
	
	EEPROM_WriteMemBuff(EEPR_ADDR_GLOBALCONFIG, (uint8_t*)&cfgConfig, sizeof(GLOBAL_CONFIG));
	
}
//==============================================================================




void			CFG_SetTimersDefault()
{
	memset(&cfgTimers, 0, sizeof(WORK_TIME));
	cfgTimers.cfg_version = FW_VERSION;

	cfgTimers.cfg_crc = CFG_TimersCalculateCRC();
}
//==============================================================================




void			CFG_RefreshTimers()
{
	cfgTimers.cfg_version = FW_VERSION;

	cfgTimers.cfg_crc = CFG_TimersCalculateCRC();
}
//==============================================================================




void			CFG_SaveTimers()
{
	cfgTimers.cfg_crc = CFG_TimersCalculateCRC();
	
	EEPROM_WriteMemBuff(EEPR_ADDR_WORKTIME, (uint8_t*)&cfgTimers, sizeof(WORK_TIME));
	
}
//==============================================================================




void			_cfg_GetParamName(char *src, char *dest, uint16_t maxlen)
{
	if (src == NULL || dest == NULL)
		return;
	
	char *string = src;
	// skip spaces
	while (*string != 0 && maxlen > 0 && (*string == ' ' || *string == '\t' || *string == '\r' || *string > 126))
	{
		string++;
		maxlen--;
	}
	// until first space symbol
	while (maxlen > 0 && *string != 0 && *string != ' ' && *string != '\t' && *string != '\r' && *string != '\n' && *string != '=')
	{
		*dest = *string;
		dest++;
		string++;
		maxlen--;
	}
	
	if (maxlen == 0)
		dest--;
	
	*dest = 0;
	return;
}
//==============================================================================




void			_cfg_GetParamValue(char *src, PARAM_VALUE *val)
{
	val->type = PARAMVAL_NONE;
	val->float_val = 0;
	val->int_val = 0;
	val->uint_val = 0;
	val->char_val = (char*)"";
	
	if (src == NULL)
		return;
	if (val == NULL)
		return;
	
	char *string = src;
	// search '='
	while (*string > 0 && *string != '=')
		string++;
	if (*string == 0)
		return;
	
	// skip '='
	string++;
	// skip spaces
	while (*string != 0 && (*string == ' ' || *string == '\t' || *string == '\r'))
		string++;
	if (*string == 0)
		return;

	// check param if it numeric
	if ((*string > 47 && *string < 58) || *string == '.' || (*string == '-' && (*(string+1) > 47 && *(string+1) < 58) || *(string+1) == '.'))
	{
		val->type = PARAMVAL_NUMERIC;
		val->float_val = (float)atof(string);
		val->int_val = atoi(string);
		val->uint_val = strtoul(string, NULL, 10);
	}
	else
	{
		val->type = PARAMVAL_STRING;
		val->char_val = string;
	}
	
	return;
}
//==============================================================================




void			CFG_LoadFromFile(void *par1, void *par2)
{
	sprintf(msg, LANG_GetString(LSTR_MSG_CFGFILE_LOADING), cfgCFileName);
	TGUI_MessageBoxWait(LANG_GetString(LSTR_WAIT), msg);

	UTF8ToUnicode_Str(cfgTFileName, cfgCFileName, sizeof(cfgTFileName)/2);
	if (f_open(&ufile, cfgTFileName, FA_OPEN_EXISTING | FA_READ) != FR_OK)
	{
		if (tguiActiveScreen == (TG_SCREEN*)&tguiMsgBox)
			tguiActiveScreen = (TG_SCREEN*)((TG_MSGBOX*)tguiActiveScreen)->prevscreen;
		TGUI_MessageBoxOk(LANG_GetString(LSTR_ERROR), LANG_GetString(LSTR_MSG_FILE_OPEN_ERROR));
		BUZZ_TimerOn(cfgConfig.buzzer_msg);
		return;
	}

	uint16_t		cnt = 0;
	UINT		readed = 0, totalreaded = 0;
	char			*string = msg;
	char			lexem[128];
	PARAM_VALUE		pval;
	CFGREAD_STATE	rdstate = CFGR_GENERAL;
	int16_t			numstr = 0;
	
	while (1)
	{
		// read one string
		cnt = 0;
		readed = 0;
		string = msg;
		while (cnt < sizeof(msg))
		{
			if (f_read(&ufile, string, 1, &readed) != FR_OK || readed == 0 || *string == '\n')
			{
				*string = 0;
				break;
			}
			cnt++;
			string++;
			totalreaded += readed;
		}
		if (cnt == sizeof(msg))
		{
			string--;
			*string = 0;
		}
		numstr++;
		string = msg;
		
		// trim spaces/tabs at begin and end
		strtrim(string);
		
		// if string is empty
		if (*string == 0)
		{
			// if end of file
			if (readed == 0)
				break;
			else
				continue;
		}
		
		// upper all letters
		strupper_utf(string);
		
		// get parameter name
		_cfg_GetParamName(string, lexem, sizeof(lexem));
		
		// skip comments
		if (*lexem == '#')
			continue;
		
		// check if here section name
		if (*lexem == '[')
		{
			if (strcmp(lexem, (char*)"[ZMOTOR]") == 0)
			{
				rdstate = CFGR_ZMOTOR;
				continue;
			}
			else if (strcmp(lexem, (char*)"[GENERAL]") == 0)
			{
				rdstate = CFGR_GENERAL;
				continue;
			}
			else if (strcmp(lexem, (char*)"[PRINTING]") == 0)
			{
				rdstate = CFGR_PRINTING;
				continue;
			}
			else
			{
				rdstate = CFGR_ERROR;
				string = LANG_GetString(LSTR_MSG_UNKNOWN_SECTNAME_IN_CFG);
				sprintf(msg, string, numstr);
				break;
			}
		}
		
		// get parameter value
		_cfg_GetParamValue(string, &pval);
		if (pval.type == PARAMVAL_NONE)
		{
			rdstate = CFGR_ERROR;
			string = LANG_GetString(LSTR_MSG_INVALID_PARAMVAL_IN_CFG);
			sprintf(msg, string, numstr);
			break;
		}
		
		// check and setup parameter
		switch (rdstate)
		{
			case CFGR_ZMOTOR:
				rdstate = CFGR_ERROR;
				if (*lexem == 'A')
				{
					if (strcmp(lexem, (char*)"ACCELERATION") == 0)
					{
						if (pval.type != PARAMVAL_NUMERIC)
						{
							string = LANG_GetString(LSTR_MSG_INVALID_PARAMVAL_IN_CFG);
							sprintf(msg, string, numstr);
							break;
						}
						if (pval.float_val < 0.1)
							pval.float_val = 0.1;
						if (pval.float_val > cfgMotor.max_acceleration)
							pval.float_val = cfgMotor.max_acceleration;
						cfgMotor.acceleration = pval.float_val;
						zPlanner.settings.acceleration = cfgMotor.acceleration;
						rdstate = CFGR_ZMOTOR;
						break;
					}
				} else
				if (*lexem == 'C')
				{
					if (strcmp(lexem, (char*)"CURRENT_HOLD_VREF") == 0)
					{
						if (pval.type != PARAMVAL_NUMERIC)
						{
							string = LANG_GetString(LSTR_MSG_INVALID_PARAMVAL_IN_CFG);
							sprintf(msg, string, numstr);
							break;
						}
						if (pval.uint_val < 100)
							pval.uint_val = 100;
						if (pval.uint_val > 1000)
							pval.uint_val = 1000;
						cfgMotor.current_hold_vref = pval.uint_val;
						rdstate = CFGR_ZMOTOR;
						break;
					}
					if (strcmp(lexem, (char*)"CURRENT_VREF") == 0)
					{
						if (pval.type != PARAMVAL_NUMERIC)
						{
							string = LANG_GetString(LSTR_MSG_INVALID_PARAMVAL_IN_CFG);
							sprintf(msg, string, numstr);
							break;
						}
						if (pval.uint_val < 100)
							pval.uint_val = 100;
						if (pval.uint_val > 1000)
							pval.uint_val = 1000;
						cfgMotor.current_vref = pval.uint_val;
						rdstate = CFGR_ZMOTOR;
						break;
					}
				} else
				if (*lexem == 'F')
				{
					if (strcmp(lexem, (char*)"FEEDRATE") == 0)
					{
						if (pval.type != PARAMVAL_NUMERIC)
						{
							string = LANG_GetString(LSTR_MSG_INVALID_PARAMVAL_IN_CFG);
							sprintf(msg, string, numstr);
							break;
						}
						if (pval.float_val < 0.1)
							pval.float_val = 0.1;
						if (pval.float_val > cfgMotor.max_feedrate)
							pval.float_val = cfgMotor.max_feedrate;
						cfgMotor.feedrate = pval.float_val;
						rdstate = CFGR_ZMOTOR;
						break;
					}
				} else
				if (*lexem == 'H')
				{
					if (strcmp(lexem, (char*)"HOLD_TIME") == 0)
					{
						if (pval.type != PARAMVAL_NUMERIC)
						{
							string = LANG_GetString(LSTR_MSG_INVALID_PARAMVAL_IN_CFG);
							sprintf(msg, string, numstr);
							break;
						}
						if (pval.uint_val == 0)
							pval.uint_val = TIMER_DISABLE;
						else if (pval.uint_val > 100000)
							pval.uint_val = 100000;
						cfgMotor.hold_time = pval.uint_val * 1000;
						rdstate = CFGR_ZMOTOR;
						break;
					}
					if (strcmp(lexem, (char*)"HOME_DIRECTION") == 0)
					{
						if (pval.type != PARAMVAL_NUMERIC)
						{
							string = LANG_GetString(LSTR_MSG_INVALID_PARAMVAL_IN_CFG);
							sprintf(msg, string, numstr);
							break;
						}
						if (pval.int_val != -1.0 && pval.int_val != 1.0)
							pval.int_val = -1;
						cfgMotor.home_dir = pval.int_val;
						rdstate = CFGR_ZMOTOR;
						break;
					}
					if (strcmp(lexem, (char*)"HOME_POS") == 0)
					{
						if (pval.type != PARAMVAL_NUMERIC)
						{
							string = LANG_GetString(LSTR_MSG_INVALID_PARAMVAL_IN_CFG);
							sprintf(msg, string, numstr);
							break;
						}
						cfgMotor.home_pos = pval.float_val;
						rdstate = CFGR_ZMOTOR;
						break;
					}
					if (strcmp(lexem, (char*)"HOMING_FEEDRATE_FAST") == 0)
					{
						if (pval.type != PARAMVAL_NUMERIC)
						{
							string = LANG_GetString(LSTR_MSG_INVALID_PARAMVAL_IN_CFG);
							sprintf(msg, string, numstr);
							break;
						}
						if (pval.float_val < 0.1)
							pval.float_val = 0.1;
						if (pval.float_val > 40)
							pval.float_val = 40;
						cfgMotor.homing_feedrate_fast = pval.float_val;
						rdstate = CFGR_ZMOTOR;
						break;
					}
					if (strcmp(lexem, (char*)"HOMING_FEEDRATE_SLOW") == 0)
					{
						if (pval.type != PARAMVAL_NUMERIC)
						{
							string = LANG_GetString(LSTR_MSG_INVALID_PARAMVAL_IN_CFG);
							sprintf(msg, string, numstr);
							break;
						}
						if (pval.float_val < 0.1)
							pval.float_val = 0.1;
						if (pval.float_val > 40)
							pval.float_val = 40;
						cfgMotor.homing_feedrate_slow = pval.float_val;
						rdstate = CFGR_ZMOTOR;
						break;
					}
				} else
				if (*lexem == 'I')
				{
					if (strcmp(lexem, (char*)"INVERT_DIR") == 0)
					{
						if (pval.type != PARAMVAL_NUMERIC)
						{
							string = LANG_GetString(LSTR_MSG_INVALID_PARAMVAL_IN_CFG);
							sprintf(msg, string, numstr);
							break;
						}
						if (pval.int_val < 0 || pval.int_val > 1)
							pval.int_val = 1;
						cfgMotor.invert_dir = pval.int_val;
						rdstate = CFGR_ZMOTOR;
						break;
					}
				} else
				if (*lexem == 'M')
				{
					if (strcmp(lexem, (char*)"MAX_ENDSTOP_INVERTING") == 0)
					{
						if (pval.type != PARAMVAL_NUMERIC)
						{
							string = LANG_GetString(LSTR_MSG_INVALID_PARAMVAL_IN_CFG);
							sprintf(msg, string, numstr);
							break;
						}
						if (pval.int_val < 0 || pval.int_val > 1)
							pval.int_val = 1;
						cfgMotor.max_endstop_inverting = pval.int_val;
						rdstate = CFGR_ZMOTOR;
						break;
					}
					if (strcmp(lexem, (char*)"MAX_POS") == 0)
					{
						if (pval.type != PARAMVAL_NUMERIC)
						{
							string = LANG_GetString(LSTR_MSG_INVALID_PARAMVAL_IN_CFG);
							sprintf(msg, string, numstr);
							break;
						}
						cfgMotor.max_pos = pval.float_val;
						rdstate = CFGR_ZMOTOR;
						break;
					}
					if (strcmp(lexem, (char*)"MIN_ENDSTOP_INVERTING") == 0)
					{
						if (pval.type != PARAMVAL_NUMERIC)
						{
							string = LANG_GetString(LSTR_MSG_INVALID_PARAMVAL_IN_CFG);
							sprintf(msg, string, numstr);
							break;
						}
						if (pval.int_val < 0 || pval.int_val > 1)
							pval.int_val = 1;
						cfgMotor.min_endstop_inverting = pval.int_val;
						rdstate = CFGR_ZMOTOR;
						break;
					}
					if (strcmp(lexem, (char*)"MIN_POS") == 0)
					{
						if (pval.type != PARAMVAL_NUMERIC)
						{
							string = LANG_GetString(LSTR_MSG_INVALID_PARAMVAL_IN_CFG);
							sprintf(msg, string, numstr);
							break;
						}
						cfgMotor.min_pos = pval.float_val;
						rdstate = CFGR_ZMOTOR;
						break;
					}
				} else
				if (*lexem == 'O')
				{
					if (strcmp(lexem, (char*)"OFF_TIME") == 0)
					{
						if (pval.type != PARAMVAL_NUMERIC)
						{
							string = LANG_GetString(LSTR_MSG_INVALID_PARAMVAL_IN_CFG);
							sprintf(msg, string, numstr);
							break;
						}
						if (pval.uint_val > 100000)
							pval.uint_val = 100000;
						else if (pval.uint_val < cfgMotor.hold_time)
							pval.uint_val = cfgMotor.hold_time + 1000;
						else if (pval.uint_val == 0)
							pval.uint_val = TIMER_DISABLE;
						cfgMotor.off_time = pval.int_val * 60000;
						rdstate = CFGR_ZMOTOR;
						break;
					}
				} else
				if (*lexem == 'S')
				{
					if (strcmp(lexem, (char*)"STEPS_PER_MM") == 0)
					{
						if (pval.type != PARAMVAL_NUMERIC)
						{
							string = LANG_GetString(LSTR_MSG_INVALID_PARAMVAL_IN_CFG);
							sprintf(msg, string, numstr);
							break;
						}
						if (pval.uint_val < 1)
							pval.uint_val = 1;
						if (pval.uint_val > 200000)
							pval.uint_val = 200000;
						cfgMotor.steps_per_mm = pval.uint_val;
						zPlanner.settings.axis_steps_per_mm = cfgMotor.steps_per_mm;
						zPlanner.refresh_positioning();
						rdstate = CFGR_ZMOTOR;
						break;
					}
				} else
				if (*lexem == 'T')
				{
					if (strcmp(lexem, (char*)"TRAVEL_ACCELERATION") == 0)
					{
						if (pval.type != PARAMVAL_NUMERIC)
						{
							string = LANG_GetString(LSTR_MSG_INVALID_PARAMVAL_IN_CFG);
							sprintf(msg, string, numstr);
							break;
						}
						if (pval.float_val < 0.1)
							pval.float_val = 0.1;
						if (pval.float_val > cfgMotor.max_acceleration)
							pval.float_val = cfgMotor.max_acceleration;
						cfgMotor.travel_acceleration = pval.float_val;
						zPlanner.settings.travel_acceleration = cfgMotor.travel_acceleration;
						rdstate = CFGR_ZMOTOR;
						break;
					}
					if (strcmp(lexem, (char*)"TRAVEL_FEEDRATE") == 0)
					{
						if (pval.type != PARAMVAL_NUMERIC)
						{
							string = LANG_GetString(LSTR_MSG_INVALID_PARAMVAL_IN_CFG);
							sprintf(msg, string, numstr);
							break;
						}
						if (pval.float_val < 0.1)
							pval.float_val = 0.1;
						if (pval.float_val > cfgMotor.max_feedrate)
							pval.float_val = cfgMotor.max_feedrate;
						cfgMotor.travel_feedrate = pval.float_val;
						rdstate = CFGR_ZMOTOR;
						break;
					}
				}

				string = LANG_GetString(LSTR_MSG_UNKNOWN_PARAMNAME_IN_CFG);
				sprintf(msg, string, numstr);
				break;

			case CFGR_GENERAL:
				rdstate = CFGR_ERROR;
				if (*lexem == 'B')
				{
					if (strcmp(lexem, (char*)"BUZZER_MSG_DURATION") == 0)
					{
						if (pval.type != PARAMVAL_NUMERIC)
						{
							string = LANG_GetString(LSTR_MSG_INVALID_PARAMVAL_IN_CFG);
							sprintf(msg, string, numstr);
							break;
						}
						if (pval.uint_val > 15000)
							pval.uint_val = 15000;
						cfgConfig.buzzer_msg = pval.uint_val;
						rdstate = CFGR_GENERAL;
						break;
					}
					if (strcmp(lexem, (char*)"BUZZER_TOUCH_DURATION") == 0)
					{
						if (pval.type != PARAMVAL_NUMERIC)
						{
							string = LANG_GetString(LSTR_MSG_INVALID_PARAMVAL_IN_CFG);
							sprintf(msg, string, numstr);
							break;
						}
						if (pval.uint_val > 15000)
							pval.uint_val = 15000;
						cfgConfig.buzzer_touch = pval.uint_val;
						rdstate = CFGR_GENERAL;
						break;
					}
				} else
				if (*lexem == 'G')
				{
					if (strcmp(lexem, (char*)"GUI_LANGUAGE") == 0)
					{
						if (pval.type != PARAMVAL_NUMERIC)
						{
							string = LANG_GetString(LSTR_MSG_INVALID_PARAMVAL_IN_CFG);
							sprintf(msg, string, numstr);
							break;
						}
						if (pval.uint_val > 1)
							pval.uint_val = 1;
						cfgConfig.language = (uint8_t)pval.uint_val;
						LANG_SetLanguage(cfgConfig.language);
						rdstate = CFGR_GENERAL;
						break;
					}
				} else
				if (*lexem == 'R')
				{
					if (strcmp(lexem, (char*)"ROTATE_DISPLAY") == 0)
					{
						if (pval.type != PARAMVAL_NUMERIC)
						{
							string = LANG_GetString(LSTR_MSG_INVALID_PARAMVAL_IN_CFG);
							sprintf(msg, string, numstr);
							break;
						}
						if (pval.uint_val > 0)
						{
							cfgConfig.display_rotate = 1;
							LCD_WriteCmd(0x0036);
							LCD_WriteRAM(0x0078);
						}
						else
						{
							cfgConfig.display_rotate = 0;
							LCD_WriteCmd(0x0036);
							LCD_WriteRAM(0x00B8);
						}
						rdstate = CFGR_GENERAL;
						break;
					}
				} else
				if (*lexem == 'S')
				{
					if (strcmp(lexem, (char*)"SCREENSAVER_TIME") == 0)
					{
						if (pval.type != PARAMVAL_NUMERIC)
						{
							string = LANG_GetString(LSTR_MSG_INVALID_PARAMVAL_IN_CFG);
							sprintf(msg, string, numstr);
							break;
						}
						if (pval.uint_val > 15000)
							cfgConfig.screensaver_time = 15000 * 60000;
						else if (pval.uint_val == 0)
							pval.uint_val = TIMER_DISABLE;
						else
							cfgConfig.screensaver_time = pval.uint_val * 60000;
						rdstate = CFGR_GENERAL;
						break;
					}
				} else
				if (*lexem == 'U')
				{
					if (strcmp(lexem, (char*)"USE_EXTERNAL_CLOCK") == 0)
					{
						if (pval.type != PARAMVAL_NUMERIC)
						{
							string = LANG_GetString(LSTR_MSG_INVALID_PARAMVAL_IN_CFG);
							sprintf(msg, string, numstr);
							break;
						}
						if (pval.uint_val > 0)
							cfgConfig.use_ext_clock = 1;
						else
							cfgConfig.use_ext_clock = 0;
						rdstate = CFGR_GENERAL;
						break;
					}
				}

				string = LANG_GetString(LSTR_MSG_UNKNOWN_PARAMNAME_IN_CFG);
				sprintf(msg, string, numstr);
				break;

			case CFGR_PRINTING:
				rdstate = CFGR_ERROR;
				if (*lexem == 'L')
				{
					if (strcmp(lexem, (char*)"LIFT_ON_PAUSE") == 0)
					{
						if (pval.type != PARAMVAL_NUMERIC)
						{
							string = LANG_GetString(LSTR_MSG_INVALID_PARAMVAL_IN_CFG);
							sprintf(msg, string, numstr);
							break;
						}
						if (pval.float_val < 0.1)
							pval.float_val = 0.1;
						if (pval.float_val > 1000)
							pval.float_val = 1000;
						cfgConfig.pause_lift = pval.float_val;
						rdstate = CFGR_PRINTING;
						break;
					}
					if (strcmp(lexem, (char*)"LIFT_ON_COMPLETION") == 0)
					{
						if (pval.type != PARAMVAL_NUMERIC)
						{
							string = LANG_GetString(LSTR_MSG_INVALID_PARAMVAL_IN_CFG);
							sprintf(msg, string, numstr);
							break;
						}
						if (pval.float_val < 0.1)
							pval.float_val = 0.1;
						if (pval.float_val > 1000)
							pval.float_val = 1000;
						cfgConfig.end_lift = pval.float_val;
						rdstate = CFGR_PRINTING;
						break;
					}
				} else
				if (*lexem == 'M')
				{
					if (strcmp(lexem, (char*)"MB_FAN_MODE") == 0)
					{
						if (pval.type != PARAMVAL_NUMERIC)
						{
							string = LANG_GetString(LSTR_MSG_INVALID_PARAMVAL_IN_CFG);
							sprintf(msg, string, numstr);
							break;
						}
						if (pval.int_val < 0)
							pval.int_val = 0;
						if (pval.int_val > 2)
							pval.int_val = 3;
						cfgConfig.mb_fan_mode = pval.int_val;
						if (cfgConfig.mb_fan_mode == MBFAN_ALWAYS_ON)
							MBFAN_On();
						else
							MBFAN_Off();
						rdstate = CFGR_PRINTING;
						break;
					}
				}


		}
		
		if (rdstate == CFGR_ERROR)
			break;
		
		
	}
	f_close(&ufile);
	
	
	if (tguiActiveScreen == (TG_SCREEN*)&tguiMsgBox)
	{
		tguiActiveScreen = (TG_SCREEN*)((TG_MSGBOX*)tguiActiveScreen)->prevscreen;
	}

	if (rdstate == CFGR_ERROR)
	{
		TGUI_MessageBoxOk(LANG_GetString(LSTR_ERROR), msg);
		BUZZ_TimerOn(cfgConfig.buzzer_msg);
	}
	else
	{
		CFG_SaveMotor();
		CFG_SaveConfig();
		TGUI_MessageBoxOk(LANG_GetString(LSTR_COMPLETED), LANG_GetString(LSTR_MSG_CFGFILE_LOADED));
	}
}
//==============================================================================




