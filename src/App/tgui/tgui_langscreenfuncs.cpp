/*******************************************************************************
*
*     Base user interface for LCD with touch
*
********************************************************************************/

#include "tgui_langscreenfuncs.h"
#include "config.h"




void		_tgui_LanguageButtonPress(void *tguiobj, void *param)
{
	TG_BUTTON		*thisbtn = (TG_BUTTON*)tguiobj;
	// Language select
	cfgConfig.language = thisbtn->button_id;
	LANG_SetLanguage(cfgConfig.language);
	CFG_SaveConfig();
//								tguiActiveScreen = (TG_SCREEN*)tguiActiveScreen->prevscreen;
	TGUI_ForceRepaint();
}
//==============================================================================




