#ifndef __MKS_REPRINT_H_
#define __MKS_REPRINT_H_

#include "cardreader.h"


#ifdef __cplusplus
 extern "C" {
#endif



#define MIN_FILE_PRINTED	3000	// �ļ����ٶ�ȡ MIN_FILE_PRINTED bytes �󣬲ű�����������


#define MKS_RETROVERSION  -2.0		// ��ͣʱ ����ͷ �س� MKS_RETROVERSION mm

typedef enum
{
	MKS_RESUME_IDLE	= 0,
	MKS_RESUME_PAUSE,		// ��ͣ����(�ϵ�)
	MKS_RESUME_PWDWN,		// �ϵ�����(�ϲ�)
	
} MKS_RESUME_STATE;

typedef enum
{
	MKS_IDLE=0xa6,	// ����
	MKS_WORKING,	// ��ӡ
	MKS_PAUSING,	// ��ͣ��	--���յ�����ָͣ����Ǵ�ӡͷ��δֹͣ�ƶ�
	MKS_PAUSED,		// ��ͣ	 	--��ӡͷ�Ѿ�ֹͣ�ƶ�
	MKS_REPRINTING,	// ������	--��⵽����Ҫ���򣬵����û���û�а�ȷ��
	MKS_REPRINTED,	// ����	 	--�û� ȷ�� ������
	MKS_STOP,//ֹͣ��ӡ
} MKS_PRINTER_STATE;



typedef struct
{
	char dirname[20];
	char filename[30];

	float mks_pausePrint_z;

	float current_position[4];
	float destination[4];
	MKS_PRINTER_STATE mks_printer_state;	

	uint32_t sdpos;		//�����ļ�sdpos
	uint32_t sdpos_bak;
	uint32_t sdpos_from_epr;
	
	uint16_t target_temperature_bed;


	float feedrate_mm_s;		//feedrate_mm_s
	uint8_t fanSpeeds_0;		//fanSpeeds[0]

	uint16_t sdprinting;

	char command_queue[MAX_CMD_SIZE];
	
	MKS_RESUME_STATE resume;

}DATA_REPRINT_ITMES;


extern DATA_REPRINT_ITMES mksReprint;



void mks_pausePrint();
void mks_initPrint();
void mks_PrintStatePolling();
void mks_resumePrint();

void mks_ReadFromEpr();
void mks_contiuePrintPause();
void mks_contiuePrintPwdwn();
void mks_contiuePrintDelta();

void mks_rePrintCheck();
void mks_contiuePrint_UI();

void mks_WriteToFile();
bool mks_ReadFromFile();
void mks_clearFile();
void Get_SerialNum(); 
void mks_preExtrude(float e);
void mks_clearDir();


void mks_setPositionZ();

void mks_moveXY(float X,float Y);
void mks_moveZ(float Z);
void mks_saveFileName(char *name);
void mks_getPositionXYZE();

extern void mkstft_ui_load();
extern void mkstft_ui_init();
extern void mks_manual_leveling(int16_t x,int16_t y);

#ifdef __cplusplus
}
#endif

#endif   
