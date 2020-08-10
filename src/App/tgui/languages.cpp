#include "languages.h"


constexpr char		*clngLangEng[_LSTR_LASTITEM] = {
		(char*)"---",
		(char*)"Jan",
		(char*)"Feb",
		(char*)"Mar",
		(char*)"Apr",
		(char*)"May",
		(char*)"Jun",
		(char*)"Jul",
		(char*)"Aug",
		(char*)"Sep",
		(char*)"Oct",
		(char*)"Nov",
		(char*)"Dec",

		(char*)"h",
		(char*)"min",
		(char*)"sec",

		(char*)"10",
		(char*)"1",
		(char*)"0.2",
		(char*)"0.05",

		(char*)"BACK",
		(char*)"MAIN MENU",
		(char*)"PRINT",
		(char*)"SERVICE",
		(char*)"INFO",
		(char*)"LANGUAGE",
		(char*)"MOVE Z",
		(char*)"TEST",

		(char*)"Version",
		(char*)"Light time",
		(char*)"Fans time",
		(char*)"URL",
		(char*)"UI author",

		(char*)"Home",
		(char*)"Set Z=0",
		(char*)"Stop!",
		
	};

constexpr char		*clngLangRus[_LSTR_LASTITEM] = {
		(char*)"---",
		(char*)"Янв",
		(char*)"Фев",
		(char*)"Мар",
		(char*)"Апр",
		(char*)"Май",
		(char*)"Июн",
		(char*)"Июл",
		(char*)"Авг",
		(char*)"Сен",
		(char*)"Окт",
		(char*)"Ноя",
		(char*)"Дек",

		(char*)"ч",
		(char*)"мин",
		(char*)"сек",

		(char*)"10",
		(char*)"1",
		(char*)"0.2",
		(char*)"0.05",

		(char*)"НАЗАД",
		(char*)"ГЛАВНОЕ МЕНЮ",
		(char*)"ПЕЧАТЬ",
		(char*)"СЕРВИС",
		(char*)"ИНФО",
		(char*)"ЯЗЫК",
		(char*)"ОСЬ Z",
		(char*)"ТЕСТ",


		(char*)"Версия",
		(char*)"Время засветки",
		(char*)"Время вентиляторов",
		(char*)"URL",
		(char*)"Автор UI",

		(char*)"Домой",
		(char*)"Уст. Z=0",
		(char*)"Стоп!",
		
	};






LNG_LANGUAGE	lngLanguages[LNG_LANGS_COUNT] = {
		{ (char*)"English", (char**)clngLangEng },
		{ (char*)"Русский", (char**)clngLangRus }
	};

char		**lngCurrent = (char**)clngLangEng;


/*
char*		LANG_GetString(LNG_STRING_ID id)
{
	for (uint16_t i = 0; i < _LSTR_LASTITEM; i++)
	{
		if (lngCurrLanguage[i].str_id == id)
			return lngCurrLanguage[i].str_text;
	}
		
	return (char*)lngEmptyString;
}
//==============================================================================
*/



char*		LANG_GetString(LNG_STRING_ID id)
{
	return lngCurrent[id];
}
//==============================================================================




char*		LANG_GetLanguageName(uint8_t lang)
{
	if (lang < LNG_LANGS_COUNT)
		return lngLanguages[lang].name;
		
	return (char*)"";
}
//==============================================================================




void		LANG_SetLanguage(uint8_t lang)
{
	lngCurrent = lngLanguages[lang].strings;
	
	return;
}
//==============================================================================




uint8_t		LANG_GetLanduadesCount()
{
	return LNG_LANGS_COUNT;
}
//==============================================================================




