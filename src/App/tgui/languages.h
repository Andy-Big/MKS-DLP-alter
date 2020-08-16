#ifndef __languages_H
#define __languages_H

#include "includes.h"
#include "main.h"


#define	LNG_LANGS_COUNT		(uint8_t)2


typedef enum 
{
	LSTR____ = (uint32_t)0,
	LSTR_TESTSTRING,

	LSTR_JANUARY,
	LSTR_FEBRUARY,
	LSTR_MARCH,
	LSTR_APRIL,
	LSTR_MAY,
	LSTR_JUNE,
	LSTR_JULY,
	LSTR_AUGUST,
	LSTR_SEPTEMBER,
	LSTR_OCTOBER,
	LSTR_NOVEMBER,
	LSTR_DECEMBER,
	
	LSTR_SHORTHOUR,
	LSTR_SHORTMINUTE,
	LSTR_SHORTSECOND,
	
	LSTR_OK,
	LSTR_CANCEL,
	LSTR_YES,
	LSTR_NO,

	LSTR_10MM,
	LSTR_1MM,
	LSTR_02MM,
	LSTR_005MM,
	
	LSTR_BACK,
	LSTR_MAINMENU,
	LSTR_PRINT,
	LSTR_SERVICE,
	LSTR_INFO,
	LSTR_LANGUAGE,
	LSTR_MOVEZ,
	LSTR_LIGHTTEST,
	LSTR_VERSION,
	LSTR_LIGHTTIME,
	LSTR_FANSTIME,
	LSTR_URL,
	LSTR_UIAUTHOR,
	LSTR_MOVEHOME,
	LSTR_SETZ0,
	LSTR_STOPMOVE,
	
	LSTR_ERROR,
	LSTR_HOME_FIRST,
	
	_LSTR_LASTITEM,

	_LSTR_MAXITEM = 0xFFFFFFFF
} LNG_STRING_ID;


typedef struct
{
	char		*name;
	char		**strings;
} LNG_LANGUAGE;




char*		LANG_GetString(LNG_STRING_ID id);
char*		LANG_GetLanguageName(uint8_t lang);
void		LANG_SetLanguage(uint8_t lang);
uint8_t		LANG_GetLanduagesCount();





#endif /*__languages_H */

