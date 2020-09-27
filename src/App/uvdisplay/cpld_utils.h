#ifndef _cpld_utils_H_
#define _cpld_utils_H_

#include "stm32f4xx_hal.h"
#include "main.h"
	 
#define CPLD_RST_Off()				CPLD_RST_GPIO_Port->BSRR = CPLD_RST_Pin
#define CPLD_RST_On()				CPLD_RST_GPIO_Port->BSRR = (uint32_t)CPLD_RST_Pin << 16U

#define CPLD_Get_CRC()				(CPLD_CRC_GPIO_Port->IDR & CPLD_CRC_Pin)



#define  CPLD_X_RATIO		2560
#define  CPLD_Y_RATIO		1440
#define  CPLD_FILLCODE		48
#define  CPLD_DATA_LEN		192 	// (Y_RATIO+2*FILLCODE)/8				//tan ---Одна строка данных длиной (720 + 48 + 720 + 48) бит
#define  CPLD_DATA_CRC_LEN	194 	// mark16 + DATA_LEN
#define  CPLD_TXDATA_LEN	196		//mark16 + DATA_LEN + CRC16

#define BANK_USED_ID0		0	// CPLD_SDRAM BANK серийный номер 0 ~ 3
#define BANK_USED_ID1		1	// CPLD_SDRAM BANK серийный номер 0 ~ 3
#define BANK_USED_ID2		2	// CPLD_SDRAM BANK серийный номер 0 ~ 3
#define BANK_USED_ID3		3	// CPLD_SDRAM BANK серийный номер 0 ~ 3

#define WORK_USED_BANK		BANK_USED_ID0
#define TEST_USED_BANK		BANK_USED_ID1
#define CLEAN_USED_BANK		BANK_USED_ID2

#define MAX_RETRAN_TIMES	5			//Максимальное количество повторных передач



struct CPLD_DATA_FRAME
{
#pragma pack(1)  
	//SPI Высокий первым вышел
	//bit7: 	0-W 1-R	;
	//bit6: 	1-SDRAM 0-REG; 
	//bit5~4: 	4 номера банка
	//bit3~0:	Строка номер 4 старшие цифры
	uint8_t mark1;		

	//bit7~0:	Номер строки 8 младших бит
	uint8_t mark2;

	uint8_t data[CPLD_DATA_LEN];	//Пиксельные данные

	uint16_t CRC16;	// X^16 + X^15 + X^2 + 1
#pragma pack()        
};


typedef struct
{
#pragma pack(1)
	//bit7: 	0-W 1-R ;
	//bit6~0:	addr
	uint8_t mark;	
	//bit7~3:	0
	//bit2~0:	nouse
	uint8_t data1;
	//bit7:		scan_en		1:enable	0:disable
	//bit6:		rd_sdram_en	1:enable	0:disable
	//bit5~4:	номер банка
	//bit3~0:	VERSION
	uint8_t data2;		
	//uint16_t CRC16;	// X^16 + X^15 + X^2 + 1
#pragma pack()  
} CPLD_CMD_FRAME;

struct DLP_BMP_LINE
{
	//----------------Данные ряда----------------
		uint16_t start_py;		//Отправная точка
		uint16_t end_py;		//Конечная точка

#pragma pack(1)                  
		CPLD_DATA_FRAME d_frame;
		uint8_t d_frame_bakup[CPLD_TXDATA_LEN];
		volatile uint8_t crc_status;
#pragma pack()                  
		
};

struct DLP_BMP
{
	//----------------Данные кадра изображения----------------

		uint32_t totalPoint;	//Общее количество точек слоя (1 сплошная яркая линия считается точкой), используемое для определения конца слоя (вместе 0x0a, 0x0d)
		uint32_t currentPoint;	//Когда текущая точка currentPoint == totalPoint, слой заканчивается

		uint16_t coordinate_X;	//Строка
		uint16_t current_line;	//Текущая строка находится в диапазоне от 0 до 2560, что является полным набором координаты_X, используемой для заполнения 0

		
		DLP_BMP_LINE line;
};







inline uint16_t		_cpld_CRC16_XMODEM(unsigned char *puchMsg, unsigned int usDataLen);

void		_cpld_fill_lines_zero(char flag, uint16_t line, uint8_t bank_used_id);
uint8_t		_cpld_reTransmission();
void		_cpld_line_gen_data(uint16_t line, uint8_t bank_used_id);

uint8_t		_cpld_bank2disp_enable(uint8_t bank_used_id, uint8_t scan_en, uint8_t rd_sdram_en);
uint8_t		_cpld_bank2disp_read();
uint8_t		_cpld_get_version();



void		CPLD_Init();




#endif // _cpld_utils_H