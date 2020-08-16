/*******************************************************************************
*
*     Base user interface for LCD with touch
*
********************************************************************************/

#include "tgui_messagebox.h"
#include "tgui_defaultfuncs.h"
#include "unicode_utils.h"


extern TG_SCREEN				*tguiActiveScreen;


void		_tgui_ShowMessageBox(MSGBOXTYPE type, char *caption, char *text, pressfunc func_ok, pressfunc func_cancel)
{

	LCDUI_FONT_TYPE	oldfont = LCDUI_SetFont(tguiMsgeBox.font_caption);
	uint16_t		oldbcolor = LCDUI_GetCurrentBackColor();
	uint16_t		oldfcolor = LCDUI_GetCurrentColor();
	
	uint16_t		width_button = tguiMsgeBox.buttons[0].position.right - tguiMsgeBox.buttons[0].position.left + 1;
	uint16_t		height_button = tguiMsgeBox.buttons[0].position.bottom - tguiMsgeBox.buttons[0].position.top + 1;

	uint16_t		width_capt = LCDUI_GetTextWidth(caption);
	if (width_capt > 420)
		width_capt = 420;
	width_capt += 20;		// 10 pix for left and right sides each
	if (width_capt < 250)
		width_capt = 250;
	
	TSIZE		box_size;
	LCDUI_DrawText(text, LCDUI_TEXT_GETSIZE, 0, 0, -1, -1, &box_size);
	if (box_size.x_size > 420)
		box_size.x_size = 420;
	box_size.x_size += 20;		// 10 pix for left and right sides each
	if (box_size.x_size < width_capt)
		box_size.x_size = width_capt;

	box_size.y_size += tguiMsgeBox.caption_height + 10 + height_button + 20;			// caption, top side space and bottom side, include buttons
	if (box_size.y_size > 300)
		box_size.y_size = 300;
	if (box_size.y_size < 130)
		box_size.y_size = 130;
	
}
//==============================================================================




void		_tgui_MessageBoxButtonPress(void *tguiobj, void *param)
{
}
//==============================================================================




