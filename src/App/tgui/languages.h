#ifndef __languages_H
#define __languages_H

#include "includes.h"
#include "main.h"


#define	LNG_LANGS_COUNT		(uint8_t)2


typedef enum
{
	LNGS_JANUARY = 0,
	LNGS_FEBRUARY,
	LNGS_MARCH,
	LNGS_APRIL,
	LNGS_MAY,
	LNGS_JUNE,
	LNGS_JULY,
	LNGS_AUGUST,
	LNGS_SEPTEMBER,
	LNGS_OCTOBER,
	LNGS_NOVEMBER,
	LNGS_DECEMBER,
	
	LNGS_BACK,
	LNGS_MAINMENU,
	LNGS_PRINT,
	LNGS_SERVICE,
	LNGS_INFO,
	LNGS_LANGUAGE,
	LNGS_MOVEZ,
	LNGS_LIGHTTEST,
	
	_LNGS_LASTITEM
} LNG_STRING_ID;


typedef struct
{
	uint16_t		str_id;
	char			*str_text;
} LNG_STRINGS;


typedef struct
{
	char			*name;
	LNG_STRINGS		*strings;
} LNG_LANGUAGE;




char*		LANG_GetString(LNG_STRING_ID id);
char*		LANG_GetLanguageName(uint8_t lang);
void		LANG_SetLanguage(uint8_t lang);





#endif /*__languages_H */

