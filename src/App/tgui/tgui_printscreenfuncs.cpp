/*******************************************************************************
*
*     Base user interface for LCD with touch
*
********************************************************************************/

#include "main.h"
#include "tgui_printscreenfuncs.h"
#include "tgui_defaultfuncs.h"
#include "printing.h"
#include "motor.h"
#include "z_endstops.h"
#include "sys_timer.h"


extern TG_SCREEN		*tguiActiveScreen;
extern char				msg[512];
extern __no_init FIL	ufile PLACE_TO_CCMRAM;

extern char				fv_filename[256];
extern char				fv_dirname[256];
extern FILES_TYPE		fv_filetype;
extern TCHAR			fv_tfilename[512];

extern PRINT_STATE		prtState;

uint32_t				old_time = 0;
uint32_t				old_layer = 0;
uint8_t					old_pause = 0;

uint8_t					disp_locked = 0;


void		TGUI_PrintScreenShow(void *tguiobj, void *param)
{
	if (PRINT_Init() == 0)
	{
		TGUI_MessageBoxOk(LANG_GetString(LSTR_ERROR), LANG_GetString(LSTR_MSG_FILE_OPEN_ERROR));
		BUZZ_TimerOn(cfgConfig.buzzer_msg);
		return;
	}

	tguiScreenPrint.prevscreen = tguiScreenFileview.prevscreen;
	tguiActiveScreen = &tguiScreenPrint;

	systemInfo.print_is_homing = 1;

	old_time = DTIME_GetCurrentUnixtime();
	old_layer = 0;

	for (uint8_t i = 0; i < tguiScreenPrint.btns_count; i++)
	{
		if (tguiScreenPrint.buttons[i].button_id == TG_SCR_PRINT_PAUSE_BTN_ID)
		{
			tguiScreenPrint.buttons[i].options.active = 0;
			break;
		}
	}

	ZMOTOR_StartHoming();

	TGUI_ForceRepaint();
}
//==============================================================================




void		TGUI_PrintScreenExit(void *tguiobj, void *param)
{
	disp_locked = 0;
	tguiActiveScreen = (TG_SCREEN*)tguiScreenFileview.prevscreen;
}
//==============================================================================




void		_tgui_PrintScreenProcess(void *tguiobj, void *param)
{
	if (old_time != DTIME_GetCurrentUnixtime() || old_layer != systemInfo.print_current_layer || old_pause != systemInfo.print_is_paused)
	{
		for (uint8_t i = 0; i < tguiScreenPrint.btns_count; i++)
		{
			if (tguiScreenPrint.buttons[i].button_id == TG_SCR_PRINT_INFO_ID)
			{
				_tgui_PrintScreenProgressUpdate((void*)&(tguiScreenPrint.buttons[i]), NULL);
				break;
			}
		}
		old_time = DTIME_GetCurrentUnixtime();
		old_layer = systemInfo.print_current_layer;
	}
	
	_tgui_DefaultScreenProcess(tguiobj, param);
}
//==============================================================================




void		_tgui_PrintScreenPausePress(void *tguiobj, void *param)
{
	if (systemInfo.print_is_paused)
		systemInfo.print_is_paused = 0;
	else
		systemInfo.print_is_paused = 1;
	for (uint8_t i = 0; i < tguiScreenPrint.btns_count; i++)
	{
		if (tguiScreenPrint.buttons[i].button_id == TG_SCR_PRINT_PAUSE_BTN_ID)
		{
			tguiScreenPrint.buttons[i].options.active = systemInfo.print_is_paused;
			tguiScreenPrint.buttons[i].options.needrepaint = 1;
			break;
		}
	}
	for (uint8_t i = 0; i < tguiScreenPrint.btns_count; i++)
	{
		if (tguiScreenPrint.buttons[i].button_id == TG_SCR_PRINT_INFO_ID)
		{
			_tgui_PrintScreenProgressUpdate((void*)&(tguiScreenPrint.buttons[i]), NULL);
			break;
		}
	}
	old_pause = systemInfo.print_is_paused;
}
//==============================================================================




void		_tgui_PrintScreenStopPress(void *tguiobj, void *param)
{
		TGUI_MessageBoxYesNo(LANG_GetString(LSTR_WARNING), LANG_GetString(LSTR_MSG_PRINT_CANCEL_QUEST), _tgui_PrintScreenStopping);
}
//==============================================================================




void		_tgui_PrintScreenStopping(void *tguiobj, void *param)
{
	TGUI_MessageBoxWait(LANG_GetString(LSTR_WAIT), LANG_GetString(LSTR_MSG_PRINT_CANCELING));
	
	ZMOTOR_Stop();
	while (ZMOTOR_IsMoving() != 0);
		
	if (systemInfo.print_is_paused)
		systemInfo.print_is_paused = 0;

	systemInfo.print_is_canceled = 1;
	
	PRINT_Complete();
}
//==============================================================================




void		_tgui_PrintScreenLockLPress(void *tguiobj, void *param)
{
	if (disp_locked == 0)
		disp_locked = 1;
	else
		disp_locked = 0;
	
	for (uint8_t i = 0; i < tguiScreenPrint.btns_count; i++)
	{
		if (tguiScreenPrint.buttons[i].button_id == TG_SCR_PRINT_LOCK_BTN_ID)
		{
			_tgui_PrintScreenLockPaint((void*)&(tguiScreenPrint.buttons[i]), NULL);
			break;
		}
		else
		{
			if (disp_locked == 0)
				tguiScreenPrint.buttons[i].options.disabled = 0;
			else
				tguiScreenPrint.buttons[i].options.disabled = 1;
		}
	}
}
//==============================================================================




void		_tgui_PrintScreenProgressPaint(void *tguiobj, void *param)
{
	TG_BUTTON		*thisbtn = (TG_BUTTON*)tguiobj;

	LCDUI_FONT_TYPE oldfont = LCDUI_SetFont(LCDUI_FONT_H18);
	uint16_t 		oldcolor = LCDUI_SetColor(LCDUI_RGB(0x00496C));
	uint16_t 		oldbackcolor = LCDUI_SetBackColor(LCDUI_RGB(0xDDDDDD));

	// file name
	LCDUI_SetColor(LCDUI_RGB(0x00496C));
	LCDUI_DrawText(fv_filename, LCDUI_TEXT_ALIGN_CENTER, thisbtn->position.left, thisbtn->position.top + 3, thisbtn->position.right, thisbtn->position.top + 22);


	// printing time
	LCDUI_SetFont(LCDUI_FONT_H18BOLD);
	LCDUI_SetColor(LCDUI_RGB(0x000000));
	LCDUI_DrawText(LANG_GetString(LSTR_TIME), 0, thisbtn->position.left + 5, thisbtn->position.top + 28, thisbtn->position.right - 366, -1);
	LCDUI_SetFont(LCDUI_FONT_H18);
	LCDUI_SetColor(LCDUI_RGB(0x00496C));
	uint32_t	est_time = PFILE_GetPrintTime();
	uint32_t	pass_time = DTIME_GetCurrentUnixtime() - systemInfo.print_time_begin;
	uint32_t	est_h = est_time / 3600;
	uint32_t	est_m = (est_time - (est_h * 3600)) / 60;
	uint32_t	pass_h = pass_time / 3600;
	uint32_t	pass_m = (pass_time - (pass_h * 3600)) / 60;
	sprintf(msg, (char*)"%02u:%02u:%02u/%02u:%02u:%02u", pass_h, pass_m, pass_time % 60, est_h, est_m, est_time % 60);
	LCDUI_DrawText(msg, LCDUI_TEXT_ALIGN_RIGHT, thisbtn->position.left + 70, thisbtn->position.top + 28, thisbtn->position.right - 176, -1);
	
	// layers + height
	LCDUI_SetFont(LCDUI_FONT_H18BOLD);
	LCDUI_SetColor(LCDUI_RGB(0x000000));
	LCDUI_DrawText(LANG_GetString(LSTR_LAYERS), 0, thisbtn->position.left + 5, thisbtn->position.top + 48, thisbtn->position.right - 366, -1);
	LCDUI_SetFont(LCDUI_FONT_H18);
	LCDUI_SetColor(LCDUI_RGB(0x00496C));
	sprintf(msg, (char*)"%u/%u (%0.1f %s)", systemInfo.print_current_layer + 1, PFILE_GetTotalLayers(), systemInfo.print_current_height, LANG_GetString(LSTR_SHORTMILLIMETERS));
	LCDUI_DrawText(msg, LCDUI_TEXT_ALIGN_RIGHT, thisbtn->position.left + 70, thisbtn->position.top + 48, thisbtn->position.right - 176, -1);

	// progress bar
	LCDUI_DrawRect(thisbtn->position.left + 5, thisbtn->position.top + 69, 380, 18);
	float		f_proc = ((float)(systemInfo.print_current_layer + 1) / (float)PFILE_GetTotalLayers()) * 100;
	uint32_t	proc = (uint32_t)f_proc;
	f_proc = (376.0 / 100.0) * f_proc;
	uint32_t		pb_width = (uint32_t)f_proc;
	LCDUI_FillRect(thisbtn->position.left + 7, thisbtn->position.top + 71, pb_width, 14);
	// percents
	LCDUI_SetFont(LCDUI_FONT_H18BOLD);
	sprintf(msg, (char*)"%u%%", proc);
	LCDUI_DrawText(msg, LCDUI_TEXT_ALIGN_RIGHT, thisbtn->position.left + 387, thisbtn->position.top + 70, thisbtn->position.right - 10, -1);

	// state (printing/pause)
	LCDUI_SetFont(LCDUI_FONT_H24BOLD);
	if (systemInfo.print_is_paused)
	{
		LCDUI_SetColor(LCDUI_RGB(0xA67500));
		LCDUI_DrawText(LANG_GetString(LSTR_PAUSE_CAPS), LCDUI_TEXT_ALIGN_CENTER, thisbtn->position.left + 285, thisbtn->position.top + 37, thisbtn->position.right - 5, -1);
	}
	else
	{
		LCDUI_SetColor(LCDUI_RGB(0x009700));
		LCDUI_DrawText(LANG_GetString(LSTR_PRINTING_CAPS), LCDUI_TEXT_ALIGN_CENTER, thisbtn->position.left + 285, thisbtn->position.top + 37, thisbtn->position.right - 5, -1);
	}

	LCDUI_SetColor(oldcolor);
	LCDUI_SetBackColor(oldbackcolor);
	LCDUI_SetFont(oldfont);
}
//==============================================================================




void		_tgui_PrintScreenPreviewPaint(void *tguiobj, void *param)
{
	if (UVLED_TimerState())
		PRINT_DrawLayerPreview();
	else
		PRINT_ClearLayerPreview();
}
//==============================================================================




void		_tgui_PrintScreenProgressUpdate(void *tguiobj, void *param)
{
	TG_BUTTON		*thisbtn = (TG_BUTTON*)tguiobj;

	LCDUI_FONT_TYPE oldfont = LCDUI_SetFont(LCDUI_FONT_H18);
	uint16_t 		oldcolor = LCDUI_SetColor(LCDUI_RGB(0x00496C));
	uint16_t 		oldbackcolor = LCDUI_SetBackColor(LCDUI_RGB(0xDDDDDD));

	// clear
	
	// printing time
	if (old_time != DTIME_GetCurrentUnixtime())
	{
		if (systemInfo.printer_state == PST_PRINT_PAUSELIFT || systemInfo.printer_state == PST_HOMING_PREUP
					|| systemInfo.printer_state == PST_HOMING_FAST || systemInfo.printer_state == PST_HOMING_UP
					|| systemInfo.printer_state == PST_HOMING_SLOW || systemInfo.printer_state == PST_PRINT_LASTLAYERLIFT)
		{
			systemInfo.print_pause_time++;
		}

		uint32_t	est_time = PFILE_GetPrintTime();
		uint32_t	pass_time = DTIME_GetCurrentUnixtime() - systemInfo.print_time_begin - systemInfo.print_pause_time;
		uint32_t	est_h = est_time / 3600;
		uint32_t	est_m = (est_time - (est_h * 3600)) / 60;
		uint32_t	pass_h = pass_time / 3600;
		uint32_t	pass_m = (pass_time - (pass_h * 3600)) / 60;
		sprintf(msg, (char*)"%02u:%02u:%02u/%02u:%02u:%02u", pass_h, pass_m, pass_time % 60, est_h, est_m, est_time % 60);
		LCDUI_SetColor(LCDUI_RGB(0xDDDDDD));
		LCDUI_FillRect(thisbtn->position.left + 86, thisbtn->position.top + 28, 190, 18);
		LCDUI_SetColor(LCDUI_RGB(0x00496C));
		LCDUI_DrawText(msg, LCDUI_TEXT_ALIGN_RIGHT, thisbtn->position.left + 70, thisbtn->position.top + 28, thisbtn->position.right - 176, -1);
		
	}
	
	if (old_layer != systemInfo.print_current_layer)
	{
		// layers
		LCDUI_SetColor(LCDUI_RGB(0x00496C));
		sprintf(msg, (char*)"%u/%u (%0.1f %s)", systemInfo.print_current_layer + 1, PFILE_GetTotalLayers(), systemInfo.print_current_height, LANG_GetString(LSTR_SHORTMILLIMETERS));
		LCDUI_SetColor(LCDUI_RGB(0xDDDDDD));
		LCDUI_FillRect(thisbtn->position.left + 80, thisbtn->position.top + 48, 210, 18);
		LCDUI_FillRect(thisbtn->position.left + 386, thisbtn->position.top + 70, 72, 18);

		LCDUI_SetColor(LCDUI_RGB(0x00496C));
		LCDUI_DrawText(msg, LCDUI_TEXT_ALIGN_RIGHT, thisbtn->position.left + 70, thisbtn->position.top + 48, thisbtn->position.right - 176, -1);

		// progress bar
		float		f_proc = ((float)(systemInfo.print_current_layer + 1) / (float)PFILE_GetTotalLayers()) * 100;
		uint32_t	proc = (uint32_t)f_proc;
		f_proc = (376.0 / 100.0) * f_proc;
		uint32_t		pb_width = (uint32_t)f_proc;
		LCDUI_FillRect(thisbtn->position.left + 7, thisbtn->position.top + 71, pb_width, 14);
		// percents
		LCDUI_SetFont(LCDUI_FONT_H18BOLD);
		sprintf(msg, (char*)"%u%%", proc);
		LCDUI_DrawText(msg, LCDUI_TEXT_ALIGN_RIGHT, thisbtn->position.left + 387, thisbtn->position.top + 70, thisbtn->position.right - 10, -1);
	
	}

	// state (printing/pause)
	if (old_pause != systemInfo.print_is_paused)
	{
		LCDUI_SetColor(LCDUI_RGB(0xDDDDDD));
		LCDUI_FillRect(thisbtn->position.left + 270, thisbtn->position.top + 35, 195, 25);
		LCDUI_SetFont(LCDUI_FONT_H24BOLD);
		if (systemInfo.print_is_paused)
		{
			LCDUI_SetColor(LCDUI_RGB(0xA67500));
			LCDUI_DrawText(LANG_GetString(LSTR_PAUSE_CAPS), LCDUI_TEXT_ALIGN_CENTER, thisbtn->position.left + 282, thisbtn->position.top + 37, thisbtn->position.right - 5, -1);
		}
		else
		{
			LCDUI_SetColor(LCDUI_RGB(0x009700));
			LCDUI_DrawText(LANG_GetString(LSTR_PRINTING_CAPS), LCDUI_TEXT_ALIGN_CENTER, thisbtn->position.left + 282, thisbtn->position.top + 37, thisbtn->position.right - 5, -1);
		}
	}

	LCDUI_SetColor(oldcolor);
	LCDUI_SetBackColor(oldbackcolor);
	LCDUI_SetFont(oldfont);
}
//==============================================================================




void		_tgui_PrintScreenLockPaint(void *tguiobj, void *param)
{
	TG_BUTTON		*thisbtn = (TG_BUTTON*)tguiobj;
	
	char *img = thisbtn->bgimagename_en;

	if (disp_locked == 1)
		img = thisbtn->bgimagename_act;

	_tgui_DrawFileCimg(img, thisbtn->textposition.left, thisbtn->textposition.top);

}
//==============================================================================




