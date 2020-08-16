/*******************************************************************************
*
*     Base user interface for LCD with touch
*
********************************************************************************/

#include "tgui_movezscreenfuncs.h"
#include "tgui_defaultfuncs.h"
#include "motor.h"
#include "z_endstops.h"
#include "config.h"


extern uint8_t			zHoldTimer;

float					fMoveStep = 0.2;




void		_tgui_MovezBackButtonPress(void *tguiobj, void *param)
{
	TG_BUTTON		*thisbtn = (TG_BUTTON*)tguiobj;

	if (ZMOTOR_IsMoving() == 1)
		return;

	thisbtn->options.pressed = 0;
	tguiActiveScreen = (TG_SCREEN*)tguiActiveScreen->prevscreen;
	TGUI_ForceRepaint();
}
//==============================================================================




void		_tgui_MovezStopButtonPress(void *tguiobj, void *param)
{
	if (ZMOTOR_IsMoving() == 0)
		return;

	ZMOTOR_Stop();
	while (ZMOTOR_IsMoving() != 0);

	systemInfo.target_position = ZMOTOR_GetCurrentPosition();
	ZMOTOR_SetPosition(systemInfo.target_position);
	
	systemInfo.printer_state = PST_IDLE;
	SYSTIMER_SetCountDown(zHoldTimer, cfgzMotor.hold_time * 1000);

}
//==============================================================================




void		_tgui_MovezHomeButtonPress(void *tguiobj, void *param)
{
	if (ZMOTOR_IsMoving() == 1)
		return;

	systemInfo.position_known = 0;

	if (systemInfo.zmotor_enabled == 0)
		ZMOTOR_MotorEnable();
	
	ZMOTOR_SetFullCurrent();
	SYSTIMER_SetCountDown(zHoldTimer, 0);
	
	ZMOTOR_EnableEndstops();

	systemInfo.target_position = ZMOTOR_GetCurrentPosition();
	ZMOTOR_SetPosition(systemInfo.target_position);

	if (cfgzMotor.z_home_dir == -1)
	{
		if (ZMOTOR_GetEndstopState() & (1 << Z_MIN))
		{
			systemInfo.target_position += 20;
			ZMOTOR_MoveAbsolute(systemInfo.target_position, 10);
		}
	}
	else
	{
		if (ZMOTOR_GetEndstopState() & (1 << Z_MAX))
		{
			systemInfo.target_position -= 20;
			ZMOTOR_MoveAbsolute(systemInfo.target_position, 10);
		}
	}
	systemInfo.printer_state = PST_HOMING_PREUP;
	_tgui_MovezUpdateHomed();
}
//==============================================================================




void		_tgui_MovezStepSelectButtonPress(void *tguiobj, void *param)
{
	TG_BUTTON		*thisbtn = (TG_BUTTON*)tguiobj;

	uint8_t			currid = thisbtn->button_id;
	switch (currid)
	{
		case TG_SCR_MOVEZ_10_ID:
			fMoveStep = 10.0;
			break;
		case TG_SCR_MOVEZ_1_ID:
			fMoveStep = 1.0;
			break;
		case TG_SCR_MOVEZ_02_ID:
			fMoveStep = 0.2;
			break;
		case TG_SCR_MOVEZ_005_ID:
			fMoveStep = 0.05;
			break;
	}
}
//==============================================================================




void		_tgui_MovezUpButtonPress(void *tguiobj, void *param)
{
	if (systemInfo.position_known == 1 && systemInfo.target_position >= cfgzMotor.z_max_pos)
		return;

	if (systemInfo.zmotor_enabled == 0)
		ZMOTOR_MotorEnable();
	
	ZMOTOR_SetFullCurrent();
	SYSTIMER_SetCountDown(zHoldTimer, 0);

	float feedrate = cfgzMotor.travel_feedrate;
	// if not homed then decrease speed | decrease speed below 30 mm
	if (systemInfo.position_known == 0 || systemInfo.target_position < 30)
		feedrate /= 3;

	systemInfo.target_position += fMoveStep;
	if (systemInfo.position_known == 1 && systemInfo.target_position > cfgzMotor.z_max_pos)
		systemInfo.target_position = cfgzMotor.z_max_pos;

	ZMOTOR_MoveAbsolute(systemInfo.target_position, feedrate);
	systemInfo.printer_state = PST_FREEMOVING_UP;
}
//==============================================================================




void		_tgui_MovezDownButtonPress(void *tguiobj, void *param)
{
	if (systemInfo.position_known == 1 && systemInfo.target_position <= cfgzMotor.z_min_pos)
		return;

	if (systemInfo.zmotor_enabled == 0)
		ZMOTOR_MotorEnable();
	
	ZMOTOR_SetFullCurrent();
	SYSTIMER_SetCountDown(zHoldTimer, 0);

	systemInfo.target_position -= fMoveStep;
	if (systemInfo.position_known == 1 && systemInfo.target_position < cfgzMotor.z_min_pos)
		systemInfo.target_position = cfgzMotor.z_min_pos;

	float feedrate = cfgzMotor.travel_feedrate;
	// if not homed then decrease speed | decrease speed below 30 mm
	if (systemInfo.position_known == 0 || systemInfo.target_position < 30)
		feedrate /= 3;
	ZMOTOR_MoveAbsolute(systemInfo.target_position, feedrate);
	systemInfo.printer_state = PST_FREEMOVING_DOWN;
}
//==============================================================================




void		_tgui_MovezSetZ0ButtonPress(void *tguiobj, void *param)
{
	if (systemInfo.position_known == 0)
	{
		TGUI_MessageBoxOk(LANG_GetString(LSTR_ERROR), LANG_GetString(LSTR_HOME_FIRST));
	}
}
//==============================================================================




void		_tgui_MovezUpdateHomed()
{
	for (uint8_t i = 0; i < tguiScreenMovez.btns_count; i++)
	{
		if (tguiScreenMovez.buttons[i].button_id == TG_SCR_MOVEZ_HOME_ID)
		{
			if (systemInfo.position_known == 0)
				tguiScreenMovez.buttons[i].options.active = 0;
			else
				tguiScreenMovez.buttons[i].options.active = 1;
			tguiScreenMovez.buttons[i].options.needrepaint = 1;
			break;
		}
	}
}
//==============================================================================




void		_tgui_MovezScreenProcess(void *tguiobj, void *param)
{
	
	_tgui_DefaultScreenProcess(tguiobj, param);
}
//==============================================================================




