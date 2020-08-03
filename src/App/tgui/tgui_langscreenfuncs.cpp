/*******************************************************************************
*
*     Base user interface for LCD with touch
*
********************************************************************************/

#include "tgui_langscreenfuncs.h"




void		_tgui_LanguageScreenProcess(void *tguiobj, void *param)
{
	TG_SCREEN		*thisscr = (TG_SCREEN*)tguiobj;
	TOUCH_STATES	ts;
	TOUCH_POINT		tp;
	
	Touch_GetCoords(&tp);
	ts = Touch_GetState();
	switch (ts)
	{
		case TS_SPRESSED:
			for (uint8_t i = 0; i < thisscr->btns_count; i++)
			{
				if (thisscr->buttons[i].options.disabled == 0 && thisscr->buttons[i].options.pressed == 0)
				{
					if (TGUI_PointInRect(&tp, &thisscr->buttons[i].position) == 1)
					{
						thisscr->buttons[i].options.pressed = 1;
						if (thisscr->buttons[i].options.repaintonpress == 1 && thisscr->buttons[i].funcs._call_paint != NULL)
						{
							thisscr->buttons[i].funcs._call_paint(&(thisscr->buttons[i]), NULL);
						}
						break;
					}
				}
			}
			break;

		case TS_LPRESSED:
			break;

		case TS_SRELEASED:
			for (uint8_t i = 0; i < thisscr->btns_count; i++)
			{
				if (thisscr->buttons[i].options.disabled == 0 && thisscr->buttons[i].options.pressed == 1)
				{
					thisscr->buttons[i].options.pressed = 0;
					if (thisscr->buttons[i].options.repaintonpress == 1 && thisscr->buttons[i].funcs._call_paint != NULL)
					{
						thisscr->buttons[i].funcs._call_paint(&(thisscr->buttons[i]), NULL);
					}
					if (thisscr->buttons[i].funcs._call_press != NULL)
					{
						if (thisscr->buttons[i].funcs._call_press == (pressfunc)BTNA_GOCHILDSCR && thisscr->buttons[i].childscreen != NULL)
						{
							TG_SCREEN		*curscreen = tguiActiveScreen;
							tguiActiveScreen = (TG_SCREEN*)thisscr->buttons[i].childscreen;
							tguiActiveScreen->prevscreen = curscreen;
							TGUI_ForceRepaint();
						}
						else
						{
							if (thisscr->buttons[i].funcs._call_press == (pressfunc)BTNA_GOPREVSCR && tguiActiveScreen->prevscreen != NULL)
							{
								tguiActiveScreen = (TG_SCREEN*)tguiActiveScreen->prevscreen;
								TGUI_ForceRepaint();
							}
							else
							{
								// Language select
								TGUI_ChangeLanguage(i);
//								tguiActiveScreen = (TG_SCREEN*)tguiActiveScreen->prevscreen;
								TGUI_ForceRepaint();
							}
						}
					}
				}

			}
			Touch_SetWorked();
			break;

		case TS_LRELEASED:
			for (uint8_t i = 0; i < thisscr->btns_count; i++)
			{
				if (thisscr->buttons[i].options.disabled == 0 && thisscr->buttons[i].options.pressed == 1)
				{
					thisscr->buttons[i].options.pressed = 0;
					if (thisscr->buttons[i].options.repaintonpress == 1 && thisscr->buttons[i].funcs._call_paint != NULL)
					{
						thisscr->buttons[i].funcs._call_paint(&(thisscr->buttons[i]), NULL);
					}
				}

			}
			Touch_SetWorked();
			break;
	}

	Touch_GetCoords(&tp);
	
}
//==============================================================================




void		_tgui_LanguageButtonPress(void *tguiobj, void *param)
{
	TG_BUTTON		*thisbtn = (TG_BUTTON*)tguiobj;
	// Language select
	TGUI_ChangeLanguage(thisbtn->button_id);
//								tguiActiveScreen = (TG_SCREEN*)tguiActiveScreen->prevscreen;
	TGUI_ForceRepaint();
}
//==============================================================================




