#include "languages.h"


LNG_STRINGS		lngLangEng[_LNGS_LASTITEM] = {
		{ LNGS_JANUARY, (char*)"Jan" },
		{ LNGS_FEBRUARY, (char*)"Feb" },
		{ LNGS_MARCH, (char*)"Mar" },
		{ LNGS_APRIL, (char*)"Apr" },
		{ LNGS_MAY, (char*)"May" },
		{ LNGS_JUNE, (char*)"Jun" },
		{ LNGS_JULY, (char*)"Jul" },
		{ LNGS_AUGUST, (char*)"Aug" },
		{ LNGS_SEPTEMBER, (char*)"Sep" },
		{ LNGS_OCTOBER, (char*)"Oct" },
		{ LNGS_NOVEMBER, (char*)"Nov" },
		{ LNGS_DECEMBER, (char*)"Dec" },
	
		{ LNGS_BACK, (char*)"Back" },
		{ LNGS_MAINMENU, (char*)"Main menu" },
		{ LNGS_PRINT, (char*)"Print" },
		{ LNGS_SERVICE, (char*)"Service" },
		{ LNGS_INFO, (char*)"Info" },
		{ LNGS_LANGUAGE, (char*)"Language" },
		{ LNGS_MOVEZ, (char*)"Move Z" },
		{ LNGS_LIGHTTEST, (char*)"Test" },
		
	};

LNG_STRINGS		lngLangRus[_LNGS_LASTITEM] = {
		{ LNGS_JANUARY, (char*)"Янв" },
		{ LNGS_FEBRUARY, (char*)"Фев" },
		{ LNGS_MARCH, (char*)"Мар" },
		{ LNGS_APRIL, (char*)"Апр" },
		{ LNGS_MAY, (char*)"Май" },
		{ LNGS_JUNE, (char*)"Июн" },
		{ LNGS_JULY, (char*)"Июл" },
		{ LNGS_AUGUST, (char*)"Авг" },
		{ LNGS_SEPTEMBER, (char*)"Сен" },
		{ LNGS_OCTOBER, (char*)"Окт" },
		{ LNGS_NOVEMBER, (char*)"Ноя" },
		{ LNGS_DECEMBER, (char*)"Дек" },
	
		{ LNGS_BACK, (char*)"Назад" },
		{ LNGS_MAINMENU, (char*)"Главное меню" },
		{ LNGS_PRINT, (char*)"Печать" },
		{ LNGS_SERVICE, (char*)"Сервис" },
		{ LNGS_INFO, (char*)"Инфо" },
		{ LNGS_LANGUAGE, (char*)"Язык" },
		{ LNGS_MOVEZ, (char*)"Двиг. Z" },
		{ LNGS_LIGHTTEST, (char*)"Тест" },
		
	};

LNG_LANGUAGE	lngLanguages[LNG_LANGS_COUNT] = {
		{ (char*)"English", lngLangEng },
		{ (char*)"Русский", lngLangRus }
	};

const char		*lngEmptyString = {0};
LNG_STRINGS		*lngCurrLanguage = lngLangEng;




char*		LANG_GetString(LNG_STRING_ID id)
{
	for (uint16_t i = 0; i < _LNGS_LASTITEM; i++)
	{
		if (lngCurrLanguage[i].str_id == id)
			return lngCurrLanguage[i].str_text;
	}
		
	return (char*)lngEmptyString;
}
//==============================================================================




char*		LANG_GetLanguageName(uint8_t lang)
{
	if (lang < LNG_LANGS_COUNT)
		return lngLanguages[lang].name;
		
	return (char*)lngEmptyString;
}
//==============================================================================




void		LANG_SetLanguage(uint8_t lang)
{
	if (lang < LNG_LANGS_COUNT)
		lngCurrLanguage = (lngLanguages[lang].strings);
	
	return;
}
//==============================================================================




