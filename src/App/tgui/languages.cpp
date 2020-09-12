#include "languages.h"


constexpr char		*clngLangEng[_LSTR_LASTITEM] = {
		(char*)"---",
		(char*)"String for test purpose",

// abbreviation
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
		(char*)"mm",
		(char*)"ml",
		(char*)"mm/min",
		(char*)"mm/sec",
		(char*)"mm/sec2",
		(char*)"KB",
		(char*)"MB",
		
		(char*)"BACK",
		(char*)"MAIN MENU",
		(char*)"FILES",
		(char*)"SERVICE",
		(char*)"INFO",
		(char*)"LANGUAGE",
		(char*)"MOVE Z",
		(char*)"UV TEST",
		(char*)"SETTINGS",
		(char*)"FILE VIEW",
		(char*)"PRINTING",

		(char*)"SETTINGS",

// Info screen
		(char*)"Version:",
		(char*)"Light time:",
		(char*)"Display time:",
		(char*)"Fan time:",
		(char*)"Total print time:",
		(char*)"URL:",
		(char*)"UI author:",

// Move Z screen
		(char*)"0.05",
		(char*)"0.2",
		(char*)"1",
		(char*)"10",
		(char*)"30",
		(char*)"Home",
		(char*)"Set Z=0",
		(char*)"Stop!",
		
// Message box captions
		(char*)"Warning!",
		(char*)"Error!",
		(char*)"Confirm the action",
		(char*)"Please wait",
		(char*)"Completed",
// Message box buttons
		(char*)"OK",
		(char*)"Cancel",
		(char*)"Yes",
		(char*)"No",
// Message box texts
		(char*)"You must first home the\nplatform. Do it now?",
		(char*)"Failed to reach the home position\nlimit switch!",
		(char*)"Check the limit switch!",
		(char*)"Do you want to load settings\nfrom this file?",
		(char*)"Loading settings from a file\n%s ...",
		(char*)"File opening error!",
		(char*)"Settings loaded success.",
		(char*)"Error in settings file\non line %u :\nUnknown section name",
		(char*)"Error in settings file\non line %u :\nUnknown parameter name",
		(char*)"Error in settings file\non line %u :\nParameter value is invalid",
		(char*)"Make sure there is no\nprinted model or debris left\non the platform or in the vat.",
		(char*)"Settings are being saved ...",
		(char*)"Set the original position of the\nplatform in this position?",
		(char*)"The original position of the\nplatform is set and saved.",
		(char*)"Printing is complete.\nPrinting time: %02u:%02u:%02u",
		
// Settings screen
		(char*)"Lift on pause",
		(char*)"Buzzer",
		(char*)"On",
		(char*)"Off",

// Numpad screen
		(char*)"AC",

// Fileview screen
		(char*)"SIZE:",
		(char*)"DATE:",
		(char*)"Total layers:",
		(char*)"Print time:",
		(char*)"Layer thickness:",
		(char*)"Antialiasing:",
		(char*)"Layers light:",
		(char*)"Bottom light:",
		(char*)"Light pause:",
		(char*)"Lift height:",
		(char*)"Lift speed:",
		(char*)"Drop speed:",
		(char*)"Resin volume:",
		(char*)"Ind. layers sett.:",

// Printing screen
		(char*)"Time:",
		(char*)"Layers:",
		(char*)"PRINTING",
		(char*)"PAUSE",

	};

constexpr char		*clngLangRus[_LSTR_LASTITEM] = {
		(char*)"---",
		(char*)"Строка для тестовых целей",
	
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
		(char*)"мм",
		(char*)"мл",
		(char*)"мм/мин",
		(char*)"мм/сек",
		(char*)"мм/сек2",
		(char*)"КБ",
		(char*)"МБ",

		(char*)"НАЗАД",
		(char*)"ГЛАВНОЕ МЕНЮ",
		(char*)"ФАЙЛЫ",
		(char*)"СЕРВИС",
		(char*)"ИНФО",
		(char*)"ЯЗЫК",
		(char*)"ОСЬ Z",
		(char*)"УФ ТЕСТ",
		(char*)"НАСТРОЙКИ",
		(char*)"ПРОСМОТР",
		(char*)"ПЕЧАТЬ",

		(char*)"НАСТР.",

// Info screen
		(char*)"Версия:",
		(char*)"Время засветки:",
		(char*)"Время дисплея:",
		(char*)"Время вентилятора:",
		(char*)"Общее время печати:",
		(char*)"URL:",
		(char*)"Автор UI:",

// Move Z screen
		(char*)"0.05",
		(char*)"0.2",
		(char*)"1",
		(char*)"10",
		(char*)"30",
		(char*)"Домой",
		(char*)"Уст. Z=0",
		(char*)"Стоп!",

// Message box captions
		(char*)"Внимание!",
		(char*)"Ошибка!",
		(char*)"Подтвердите действие",
		(char*)"Пожалуйста, подождите",
		(char*)"Завершено",
// Message box buttons
		(char*)"OK",
		(char*)"Отмена",
		(char*)"Да",
		(char*)"Нет",
// Message box texts
		(char*)"Домашняя позиция платформы\nне определена.\nСделать это сейчас?",
		(char*)"Не удалось достичь концевого выключателя\nдомашней позиции!",
		(char*)"Проверьте концевой выключатель!",
		(char*)"Вы хотите загрузить настройки\nиз этого файла?",
		(char*)"Загружаются настройки из файла\n%s ...",
		(char*)"Ошибка открытия файла!",
		(char*)"Настройки успешно загружены.",
		(char*)"Ошибка в файле настроек\nв строке %u :\nНеизвестное имя раздела",
		(char*)"Ошибка в файле настроек\nв строке %u :\nНеизвестное имя параметра",
		(char*)"Ошибка в файле настроек\nв строке %u :\nНеверное значения параметра",
		(char*)"Убедитесь, что на платформе\nи в ванне не осталась напечатанная\nмодель или ее остатки.",
		(char*)"Идет сохранение настроек...",
		(char*)"Установить исходное положение\nплатформы в этой позиции?",
		(char*)"Исходное положение платформы\nустановлено и сохранено.",
		(char*)"Печать завершена.\nВремя печати: %02u:%02u:%02u",

// Settings screen
		(char*)"Подъем на паузе",
		(char*)"Звук",
		(char*)"Вкл",
		(char*)"Выкл",

// Numpad screen
		(char*)"AC",

// Fileview screen
		(char*)"РАЗМЕР:",
		(char*)"ДАТА:",
		(char*)"Всего слоев:",
		(char*)"Время печати:",
		(char*)"Толщина слоя:",
		(char*)"Антиалиасинг:",
		(char*)"Засветка слоя:",
		(char*)"Засветка низа:",
		(char*)"Пауза засветки:",
		(char*)"Высота подъема:",
		(char*)"Скорость вверх:",
		(char*)"Скорость вниз:",
		(char*)"Объем смолы:",
		(char*)"Инд. настр. слоев:",

// Printing screen
		(char*)"Время:",
		(char*)"Слои:",
		(char*)"ПЕЧАТЬ",
		(char*)"ПАУЗА",

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




uint8_t		LANG_GetLanduagesCount()
{
	return LNG_LANGS_COUNT;
}
//==============================================================================




