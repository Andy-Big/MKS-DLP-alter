#include "at24cxx.h"
#include "mks_reprint.h"
#include "mks_cfg.h"
#include "fatfs.h"
#include "draw_printing.h"
#include "usb_host.h"
#include "ili9320.h"
#include "mks_dlp_main.h"

extern uint8_t pause_resum;
extern uint32_t logo_tick1,logo_tick2;
extern uint8_t from_flash_pic;

uint8_t continue_print_error_flg = 0;

char *mks_pft_name = "mks_pft.sys"; 

void mks_ReadFromEpr_pwroff();
void mks_WriteToEpr_pwroff();

void mks_G28(char *g_command);
void mkstft_ui_load()
{
	//volatile uint32_t  valid_flag;
	//epr_read_data(EPR_INF_VALID_FLAG,(uint8_t *)&valid_flag,sizeof(valid_flag)); 
	//if(valid_flag == BAK_INF_VALID_FLAG)
	{
		epr_read_data(EPR_SCREEN_BKCOLOR,(uint8_t *)&gCfgItems.background_color,sizeof(gCfgItems.background_color)); 
        epr_read_data(EPR_BTN_BKCOLOR,(uint8_t *)&gCfgItems.btn_color,sizeof(gCfgItems.btn_color)); 
		epr_read_data(EPR_BTN_TEXT_BKCOLOR,(uint8_t *)&gCfgItems.btn_text_bkcolor,sizeof(gCfgItems.btn_text_bkcolor)); 
		epr_read_data(EPR_BTN_TEXTCOLOR,(uint8_t *)&gCfgItems.btn_textcolor,sizeof(gCfgItems.btn_textcolor));
		epr_read_data(EPR_FILENAME_BKCOLOR,(uint8_t *)&gCfgItems.filename_background_color,sizeof(gCfgItems.filename_background_color)); 
		epr_read_data(EPR_FILENAME_TEXTCOLOR,(uint8_t *)&gCfgItems.filename_color,sizeof(gCfgItems.filename_color)); 
        epr_read_data(EPR_MSG_AREA_BKCOLOR,(uint8_t *)&gCfgItems.msg_area_bkcolor,sizeof(gCfgItems.msg_area_bkcolor)); 
		epr_read_data(EPR_STATE_MSG_BKCOLOR,(uint8_t *)&gCfgItems.state_msg_bkcolor,sizeof(gCfgItems.state_msg_bkcolor)); 
		epr_read_data(EPR_STATE_MSG_TEXTCOLOR,(uint8_t *)&gCfgItems.state_msg_textcolor,sizeof(gCfgItems.state_msg_textcolor)); 
		epr_read_data(EPR_SEL_BTN_BKCOLOR,(uint8_t *)&gCfgItems.btn_state_sel_color,sizeof(gCfgItems.btn_state_sel_color)); 
		epr_read_data(EPR_SEL_BTN_TEXT_BKCOLOR,(uint8_t *)&gCfgItems.btn_state_sel_text_bkcolor,sizeof(gCfgItems.btn_state_sel_text_bkcolor)); 
        epr_read_data(EPR_SEL_BTN_TEXTCOLOR,(uint8_t *)&gCfgItems.btn_state_sel_textcolor,sizeof(gCfgItems.btn_state_sel_textcolor)); 
        epr_read_data(EPR_DIALOG_BKCOLOR,(uint8_t *)&gCfgItems.dialog_color,sizeof(gCfgItems.dialog_color)); 
		epr_read_data(EPR_DIALOG_MSG_TEXT_COLOR,(uint8_t *)&gCfgItems.dialog_text_color,sizeof(gCfgItems.dialog_text_color)); 
        epr_read_data(EPR_DIALOG_BTN_BKCOLOR,(uint8_t *)&gCfgItems.dialog_btn_color,sizeof(gCfgItems.dialog_btn_color)); 
		epr_read_data(EPR_DIALOG_BTN_TEXTCOLOR,(uint8_t *)&gCfgItems.dialog_btn_textcolor,sizeof(gCfgItems.dialog_btn_textcolor)); 
        epr_write_data(EPR_LAYER_AREA_BKCOLOR,(uint8_t *)&gCfgItems.layer_area_bkcolor,sizeof(gCfgItems.layer_area_bkcolor)); 
		epr_write_data(EPR_PRINTING_STATE_BKCOLOR,(uint8_t *)&gCfgItems.printing_state_bkcolor,sizeof(gCfgItems.printing_state_bkcolor)); 
        epr_write_data(EPR_PRINTING_STATE_TEXTCOLOR,(uint8_t *)&gCfgItems.printing_state_text_color,sizeof(gCfgItems.printing_state_text_color)); 

//		epr_read_data(EPR_FILE_SYS,(uint8_t *)&gCfgItems.fileSysType,sizeof(gCfgItems.fileSysType)); 
		epr_read_data(EPR_MUTIL_LANGUAGE_FLG,(uint8_t *)&gCfgItems.multiple_language,sizeof(gCfgItems.multiple_language)); 
		epr_read_data(EPR_LANGUAGE,(uint8_t *)&gCfgItems.language,sizeof(gCfgItems.language)); 
        if((gCfgItems.language != LANG_SIMPLE_CHINESE)
            &&(gCfgItems.language != LANG_COMPLEX_CHINESE)
            &&(gCfgItems.language != LANG_ENGLISH)
            &&(gCfgItems.language != LANG_FRENCH)
            &&(gCfgItems.language != LANG_RUSSIAN)
            &&(gCfgItems.language != LANG_SPANISH)
            &&(gCfgItems.language != LANG_ITALY))
        {
            gCfgItems.language = LANG_ENGLISH;//默认为英语。
        }
		epr_read_data(EPR_VERSION_NUMBER_CUSTOM,(uint8_t *)&gCfgItems.version_number_custom,sizeof(gCfgItems.version_number_custom));		

		
//		epr_read_data(EPR_MORE_ITEM_CNT,(uint8_t *)&gCfgItems.MoreItem_pic_cnt,sizeof(gCfgItems.MoreItem_pic_cnt)); 
/*
		epr_read_data(EPR_SETMENU_FUNC1_DISPLAY_FLG,(uint8_t *)&gCfgItems.func_btn1_display_flag,sizeof(gCfgItems.func_btn1_display_flag));	

		epr_read_data(EPR_SCREEN_DISPLAY_STYLE,(uint8_t *)&gCfgItems.display_style,sizeof(gCfgItems.display_style));	
		epr_read_data(EPR_PWROFF_SAVE_MODE,(uint8_t *)&gCfgItems.pwroff_save_mode,sizeof(gCfgItems.pwroff_save_mode));	

		epr_read_data(EPR_INSERT_DET_MODULE_TYPE,(uint8_t *)&gCfgItems.insert_det_module,sizeof(gCfgItems.insert_det_module)); 
		epr_read_data(EPR_HAS_UPS,(uint8_t *)&gCfgItems.have_ups,sizeof(gCfgItems.have_ups));	
		epr_read_data(EPR_FILAMENT_DET0_LEVEL,(uint8_t *)&gCfgItems.filament_det0_level_flg,sizeof(gCfgItems.filament_det0_level_flg));	
		epr_read_data(EPR_FILAMENT_DET1_LEVEL,(uint8_t *)&gCfgItems.filament_det1_level_flg,sizeof(gCfgItems.filament_det1_level_flg));	
		epr_read_data(EPR_MASK_DET_FUNCTION,(uint8_t *)&gCfgItems.mask_det_Function,sizeof(gCfgItems.mask_det_Function));	


		epr_read_data(EPR_AUTO_CLOSE_MACHINE,(uint8_t *)&gCfgItems.print_finish_close_machine_flg,sizeof(gCfgItems.print_finish_close_machine_flg));
		epr_read_data(EPR_ENABLE_CLOUD,(uint8_t *)&gCfgItems.cloud_enable,sizeof(gCfgItems.cloud_enable));
    */

	}
	/*
	else
	
	*/

}

void mkstft_ui_set_epr()
{
	volatile uint32_t  valid_flag;
	epr_read_data(EPR_INF_VALID_FLAG,(uint8_t *)&valid_flag,sizeof(valid_flag)); 
	if(valid_flag != BAK_INF_VALID_FLAG)
	{
		epr_write_data(EPR_SCREEN_BKCOLOR,(uint8_t *)&gCfgItems.background_color,sizeof(gCfgItems.background_color)); 
		//epr_write_data(EPR_TITIL_COLOR,(uint8_t *)&gCfgItems.title_color,sizeof(gCfgItems.title_color)); 
		//epr_write_data(EPR_STATE_BKCOLOR,(uint8_t *)&gCfgItems.state_background_color,sizeof(gCfgItems.state_background_color)); 
		//epr_write_data(EPR_STATE_TEXTCOLOR,(uint8_t *)&gCfgItems.state_text_color,sizeof(gCfgItems.state_text_color)); 
		//epr_write_data(EPR_FILENAME_BKCOLOR,(uint8_t *)&gCfgItems.filename_background_color,sizeof(gCfgItems.filename_background_color)); 
		//epr_write_data(EPR_FILENAME_TEXTCOLOR,(uint8_t *)&gCfgItems.filename_color,sizeof(gCfgItems.filename_color)); 

        epr_write_data(EPR_BTN_BKCOLOR,(uint8_t *)&gCfgItems.btn_color,sizeof(gCfgItems.btn_color)); 
        epr_write_data(EPR_BTN_TEXT_BKCOLOR,(uint8_t *)&gCfgItems.btn_textcolor,sizeof(gCfgItems.btn_text_bkcolor)); 
		epr_write_data(EPR_BTN_TEXTCOLOR,(uint8_t *)&gCfgItems.btn_textcolor,sizeof(gCfgItems.btn_textcolor)); 

        
		epr_write_data(EPR_FILENAME_BKCOLOR,(uint8_t *)&gCfgItems.filename_background_color,sizeof(gCfgItems.filename_background_color)); 
		epr_write_data(EPR_FILENAME_TEXTCOLOR,(uint8_t *)&gCfgItems.filename_color,sizeof(gCfgItems.filename_color)); 

		epr_write_data(EPR_MSG_AREA_BKCOLOR,(uint8_t *)&gCfgItems.msg_area_bkcolor,sizeof(gCfgItems.msg_area_bkcolor)); 
		epr_write_data(EPR_STATE_MSG_BKCOLOR,(uint8_t *)&gCfgItems.state_msg_bkcolor,sizeof(gCfgItems.state_msg_bkcolor)); 
		epr_write_data(EPR_STATE_MSG_TEXTCOLOR,(uint8_t *)&gCfgItems.state_msg_textcolor,sizeof(gCfgItems.state_msg_textcolor)); 

		//epr_write_data(EPR_STATE_BTN_BKCOLOR,(uint8_t *)&gCfgItems.btn_state_color,sizeof(gCfgItems.btn_state_color)); 
		//epr_write_data(EPR_STATE_BTN_TEXTCOLOR,(uint8_t *)&gCfgItems.btn_state_textcolor,sizeof(gCfgItems.btn_state_textcolor)); 
		//epr_write_data(EPR_BACK_BTN_BKCOLOR,(uint8_t *)&gCfgItems.back_btn_color,sizeof(gCfgItems.back_btn_color)); 
		//epr_write_data(EPR_BACK_BTN_TEXTCOLOR,(uint8_t *)&gCfgItems.back_btn_textcolor,sizeof(gCfgItems.back_btn_textcolor)); 
		epr_write_data(EPR_SEL_BTN_BKCOLOR,(uint8_t *)&gCfgItems.btn_state_sel_color,sizeof(gCfgItems.btn_state_sel_color)); 
		epr_write_data(EPR_SEL_BTN_TEXT_BKCOLOR,(uint8_t *)&gCfgItems.btn_state_sel_text_bkcolor,sizeof(gCfgItems.btn_state_sel_text_bkcolor)); 
		epr_write_data(EPR_SEL_BTN_TEXTCOLOR,(uint8_t *)&gCfgItems.btn_state_sel_textcolor,sizeof(gCfgItems.btn_state_sel_textcolor)); 

        epr_write_data(EPR_DIALOG_BKCOLOR,(uint8_t *)&gCfgItems.dialog_color,sizeof(gCfgItems.dialog_color)); 
		epr_write_data(EPR_DIALOG_MSG_TEXT_COLOR,(uint8_t *)&gCfgItems.dialog_text_color,sizeof(gCfgItems.dialog_text_color)); 
        epr_write_data(EPR_DIALOG_BTN_BKCOLOR,(uint8_t *)&gCfgItems.dialog_btn_color,sizeof(gCfgItems.dialog_btn_color)); 
		epr_write_data(EPR_DIALOG_BTN_TEXTCOLOR,(uint8_t *)&gCfgItems.dialog_btn_textcolor,sizeof(gCfgItems.dialog_btn_textcolor)); 

        epr_write_data(EPR_LAYER_AREA_BKCOLOR,(uint8_t *)&gCfgItems.layer_area_bkcolor,sizeof(gCfgItems.layer_area_bkcolor)); 
		epr_write_data(EPR_PRINTING_STATE_BKCOLOR,(uint8_t *)&gCfgItems.printing_state_bkcolor,sizeof(gCfgItems.printing_state_bkcolor)); 
        epr_write_data(EPR_PRINTING_STATE_TEXTCOLOR,(uint8_t *)&gCfgItems.printing_state_text_color,sizeof(gCfgItems.printing_state_text_color)); 


//		epr_write_data(EPR_FILE_SYS,(uint8_t *)&gCfgItems.fileSysType,sizeof(gCfgItems.fileSysType)); 
		epr_write_data(EPR_MUTIL_LANGUAGE_FLG,(uint8_t *)&gCfgItems.multiple_language,sizeof(gCfgItems.multiple_language)); 
		epr_write_data(EPR_LANGUAGE,(uint8_t *)&gCfgItems.language,sizeof(gCfgItems.language)); 
		epr_write_data(EPR_VERSION_NUMBER_CUSTOM,(uint8_t *)&gCfgItems.version_number_custom,sizeof(gCfgItems.version_number_custom));		


//		epr_write_data(EPR_MORE_ITEM_CNT,(uint8_t *)&gCfgItems.MoreItem_pic_cnt,sizeof(gCfgItems.MoreItem_pic_cnt)); 

//		epr_write_data(EPR_SETMENU_FUNC1_DISPLAY_FLG,(uint8_t *)&gCfgItems.func_btn1_display_flag,sizeof(gCfgItems.func_btn1_display_flag));	
/*
		epr_write_data(EPR_SCREEN_DISPLAY_STYLE,(uint8_t *)&gCfgItems.display_style,sizeof(gCfgItems.display_style));	
		epr_write_data(EPR_PWROFF_SAVE_MODE,(uint8_t *)&gCfgItems.pwroff_save_mode,sizeof(gCfgItems.pwroff_save_mode));	

		epr_write_data(EPR_INSERT_DET_MODULE_TYPE,(uint8_t *)&gCfgItems.insert_det_module,sizeof(gCfgItems.insert_det_module));	
		epr_write_data(EPR_HAS_UPS,(uint8_t *)&gCfgItems.have_ups,sizeof(gCfgItems.have_ups));	
		epr_write_data(EPR_FILAMENT_DET0_LEVEL,(uint8_t *)&gCfgItems.filament_det0_level_flg,sizeof(gCfgItems.filament_det0_level_flg));	
		epr_write_data(EPR_FILAMENT_DET1_LEVEL,(uint8_t *)&gCfgItems.filament_det1_level_flg,sizeof(gCfgItems.filament_det1_level_flg));	
		epr_write_data(EPR_MASK_DET_FUNCTION,(uint8_t *)&gCfgItems.mask_det_Function,sizeof(gCfgItems.mask_det_Function));	


		epr_write_data(EPR_AUTO_CLOSE_MACHINE,(uint8_t *)&gCfgItems.print_finish_close_machine_flg,sizeof(gCfgItems.print_finish_close_machine_flg));
		epr_write_data(EPR_ENABLE_CLOUD,(uint8_t *)&gCfgItems.cloud_enable,sizeof(gCfgItems.cloud_enable));
*/
		valid_flag = BAK_INF_VALID_FLAG;
		epr_write_data(EPR_INF_VALID_FLAG,(uint8_t *)&valid_flag,sizeof(valid_flag)); 

	}		
		
}
void mkstft_ui_init()
{
#if defined(MKS_DLP_BOARD)
    gCfgItems.title_color = 0xFFFFFF;

    gCfgItems.background_color = 0xFFFFFF;//屏幕背景色
    
    gCfgItems.btn_color = 0x8DB937;//按钮背景色
    gCfgItems.btn_text_bkcolor = 0x000000;//按钮文字背景色
    gCfgItems.btn_textcolor = 0x8DB937;//按钮文字颜色
    
    //gCfgItems.printfile_btn_bkcolor = 0xFFFFFF;//文件目录按钮背景色
    gCfgItems.filename_background_color = 0x000000;//文件名称背景色
    gCfgItems.filename_color = 0xffffff;//文件名称字体颜色
    
    gCfgItems.msg_area_bkcolor = 0x8DB937;//状态提示信息区域背景色
    gCfgItems.state_msg_bkcolor = 0xffffff;//状态信息字体背景色
    gCfgItems.state_msg_textcolor = 0x8DB937;//状态信息字体颜色
    
    gCfgItems.btn_state_sel_color = 0xFFFFFF;//选定按钮背景色
    gCfgItems.btn_state_sel_text_bkcolor = 0xFFFFFF;//选定按钮字体背景色
    gCfgItems.btn_state_sel_textcolor = 0x43cdff;//选定按钮字体颜色
    
    gCfgItems.printing_bar_color_left= 0x00ff00;//进度条左边颜色
    gCfgItems.printing_bar_color_right= 0xAAAAAA ;//进度条右边颜色
    gCfgItems.printing_bar_text_color_left = 0x000000;//进度条左边字体颜色
    gCfgItems.printing_bar_text_color_right = 0x000000;//进度条右边字体颜色
    
    gCfgItems.dialog_color = 0x000000;//对话框背景色 
    gCfgItems.dialog_text_color = 0xffffff;//对话框提示音字体颜色
    gCfgItems.dialog_btn_color = 0xff0000;//对话框按钮颜色
    gCfgItems.dialog_btn_textcolor = 0xffffff;//对话框按钮字体颜色

    gCfgItems.layer_area_bkcolor = 0x8DB937;//图层显示区域背景色
    gCfgItems.printing_state_bkcolor = 0xffffff;//打印状态字体背景色
    gCfgItems.printing_state_text_color = 0x8DB937;//打印状态字体颜色
    

#else
		gCfgItems.background_color = 0x000000;//
		gCfgItems.title_color = 0xFFFFFF;//
		gCfgItems.state_background_color = 0x000000;//
		gCfgItems.state_text_color = 0xFFFFFF;//
		gCfgItems.btn_color = 0x000000;
		gCfgItems.btn_textcolor = 0xffffff;
		gCfgItems.btn_state_color = 000000;//0x505050;
		gCfgItems.btn_state_textcolor = 0xffffff;
		gCfgItems.btn_state_sel_color = 000000;//0x9dfcff;
		gCfgItems.btn_state_sel_textcolor = 0xFFFFFF;
		gCfgItems.back_btn_color = 0x000000;
		gCfgItems.back_btn_textcolor = 0xFFFFFF;
		gCfgItems.printing_bar_color_left= 0x00ff00;
		gCfgItems.printing_bar_color_right= 0xAAAAAA ;
		gCfgItems.printing_bar_text_color_left = 0x000000;
		gCfgItems.printing_bar_text_color_right = 0x000000;
		gCfgItems.dialog_btn_color = 0xff0000;
		gCfgItems.dialog_btn_textcolor = 0xffffff;
		gCfgItems.printfile_color = 0x000000;
		gCfgItems.printfile_textcolor = 0xffffff;

#endif
		gCfgItems.multiple_language = 1;
		gCfgItems.language = 3;
		
		//gCfgItems.touch_adj_flag = 0;
		gCfgItems.touch_adj_xMin = 3919;
		gCfgItems.touch_adj_xMax = 201;
		gCfgItems.touch_adj_yMin = 164;
		gCfgItems.touch_adj_yMax = 3776;

		gCfgItems.fileSysType = FILE_SYS_SD;
		gCfgItems.stepHeat = 5;
		gCfgItems.curSprayerChoose = 0;
		gCfgItems.move_dist = 1;
		gCfgItems.moveSpeed = 3000;
		gCfgItems.extruSpeed = 1;
		gCfgItems.extruStep = 5;
		gCfgItems.stepPrintSpeed = 5;
		gCfgItems.printSpeed = 100;
		gCfgItems.printExtSpeed0 = 100;

		
		gCfgItems.insert_det_module = 0;
		gCfgItems.have_ups = 0;
		gCfgItems.filament_det0_level_flg=0;
		gCfgItems.filament_det1_level_flg=0;
		gCfgItems.mask_det_Function = 0;

		gCfgItems.print_finish_close_machine_flg = 0;

		//gCfgItems.preheat_max_desireSprayerTemp = 275;
		//gCfgItems.preheat_max_desireBedTemp = 150;

		strcpy((char *)gCfgItems.z_display_pos,(char *)"0.000");
		gCfgItems.custom_bed_flag = 1;


		gCfgItems.MoreItem_pic_cnt = 0;
		gCfgItems.func_btn1_display_flag = 1;
		
		/*wifi_cfg*/
		gCfgItems.wifi_mode_sel = 1;//1为AP模式，其他为client模式，默认为AP模式。
		memset(&wifiPara, 0, sizeof(wifiPara));
		memset(&ipPara, 0, sizeof(ipPara)); 
		strcpy(wifiPara.ap_name,WIFI_AP_NAME);
		strcpy(wifiPara.keyCode,WIFI_KEY_CODE);
		//client模式初始值
		strcpy(ipPara.ip_addr,IP_ADDR);
		strcpy(ipPara.mask,IP_MASK);
		strcpy(ipPara.gate,IP_GATE);
		strcpy(ipPara.dns,IP_DNS);
		
		ipPara.dhcp_flag = IP_DHCP_FLAG;
		
		//AP模式初始值
		strcpy(ipPara.dhcpd_ip,AP_IP_ADDR);
		strcpy(ipPara.dhcpd_mask,AP_IP_MASK);
		strcpy(ipPara.dhcpd_gate,AP_IP_GATE);
		strcpy(ipPara.dhcpd_dns,AP_IP_DNS);
		strcpy(ipPara.start_ip_addr,IP_START_IP);
		strcpy(ipPara.end_ip_addr,IP_END_IP);
		
		ipPara.dhcpd_flag = AP_IP_DHCP_FLAG;
		
		gCfgItems.wifi_type = ESP_WIFI;
		
		strcpy((char*)gCfgItems.cloud_hostUrl, "baizhongyun.cn");
		gCfgItems.cloud_port = 10086;

		
		gCfgItems.cloud_enable = 1;
		
		gCfgItems.btn_text_offset = 23;
		gCfgItems.display_style = 1;

		gCfgItems.pwroff_save_mode = 0;

		gCfgItems.breakpoint_reprint_flg = 0;
        
		gCfgItems.clean_time = 100;
        gCfgItems.version_number_custom = 0;
		mkstft_ui_set_epr();
		
		//bmp_struct.XSize = BTN_PIC_X_PIXEL;
		//bmp_struct.YSize = BTN_PIC_Y_PIXEL;
	
		BMP_PIC_X = BTN_PIC_X_OFFSET;
		BMP_PIC_Y = BTN_PIC_Y_OFFSET;

        //gCfgItems.btn_color = 0xFFFFFF;
        //gCfgItems.btn_textcolor= 0x5B9212;

        //gCfgItems.background_color =  0xFFFFFF ;//
		//gCfgItems.title_color = 0x000000;//

        gCfgItems.btn_text_offset = 0;

}
DATA_REPRINT_ITMES mksReprint;
void mks_initPrint()
{
#if 0       //mks 2018-07_a  
	mksReprint.mks_pausePrint_x = mksCfg.filament_change_x_pos;//FILAMENT_CHANGE_X_POS;
	mksReprint.mks_pausePrint_y = mksCfg.filament_change_y_pos;//FILAMENT_CHANGE_Y_POS;
	mksReprint.mks_pausePrint_z = mksCfg.filament_change_z_add;//FILAMENT_CHANGE_Z_ADD;
#else
 //       mksReprint.mks_pausePrint_x = 5;
 //       mksReprint.mks_pausePrint_y = 5;
        mksReprint.mks_pausePrint_z = mksCfg.z_pause_pos;
        
#endif        
//	mksReprint.mks_pausePrint_e = 5.0;
//	mksReprint.waitEndMoves = 0;
	mksReprint.mks_printer_state = MKS_IDLE;

	mksReprint.sdpos = 0;

	memset(mksReprint.dirname,0,sizeof(mksReprint.dirname));
	memset(mksReprint.filename,0,sizeof(mksReprint.filename));

	mksReprint.sdprinting = 0;

//	mksReprint.target_temperature_0 = 0;
// 	mksReprint.target_temperature_bed = 0;

	mksReprint.resume = MKS_RESUME_IDLE;

//	mksReprint.refresh = false;
	
}

uint32_t t1,t2;
char t12[10];

void mks_WriteToEpr()        //暂停、断电时保存数据，以备断电后续打
{
    //t1 = getTick();    
	//保存文件位置
#if 0	
	mksReprint.sdpos = card.getsdpos();
	epr_write_data(EPR_SAV_SDPOS, (uint8_t *)&mksReprint.sdpos,sizeof(mksReprint.sdpos));	
	
	//保存打印头温度
	mksReprint.target_temperature_0 = thermalManager.target_temperature[0];
	epr_write_data(EPR_SAV_TARGET_TEMP_0, (uint8_t *)&mksReprint.target_temperature_0,sizeof(mksReprint.target_temperature_0));
#if 0       //mks 2018-07_a	
        if(mksCfg.extruders == 2)
	{
		mksReprint.target_temperature_1 = thermalManager.target_temperature[1];
		epr_write_data(EPR_SAV_TARGET_TEMP_1, (uint8_t *)&mksReprint.target_temperature_1,sizeof(mksReprint.target_temperature_1));
		
	}
#endif        
	//保存热床温度
#if HAS_TEMP_BED		//mks 20-8-07
	if(HAS_TEMP_BED)
	{
		mksReprint.target_temperature_bed = thermalManager.target_temperature_bed;
		epr_write_data(EPR_SAV_TARGET_TEMP_BED, (uint8_t *)&mksReprint.target_temperature_bed,sizeof(mksReprint.target_temperature_bed));
	}
#endif					//mks 20-8-07        
	
	//保存当前位置
	epr_write_data(EPR_SAV_CUR_X, (uint8_t *)&mksReprint.current_position[0],sizeof(mksReprint.current_position[0]));
	epr_write_data(EPR_SAV_CUR_Y, (uint8_t *)&mksReprint.current_position[1],sizeof(mksReprint.current_position[1]));
	epr_write_data(EPR_SAV_CUR_Z, (uint8_t *)&mksReprint.current_position[2],sizeof(mksReprint.current_position[2]));
	epr_write_data(EPR_SAV_CUR_E, (uint8_t *)&mksReprint.current_position[3],sizeof(mksReprint.current_position[3]));
	//保存目标位置
	epr_write_data(EPR_SAV_DST_X, (uint8_t *)&mksReprint.destination[0],sizeof(mksReprint.destination[0]));
	epr_write_data(EPR_SAV_DST_Y, (uint8_t *)&mksReprint.destination[1],sizeof(mksReprint.destination[1]));
	epr_write_data(EPR_SAV_DST_Z, (uint8_t *)&mksReprint.destination[2],sizeof(mksReprint.destination[2]));
	epr_write_data(EPR_SAV_DST_E, (uint8_t *)&mksReprint.destination[3],sizeof(mksReprint.destination[3]));

#if tan_mask        
	//保存进度和时间
	print_job_timer.getTime(&mksReprint.accumulator,&mksReprint.startTimestamp,&mksReprint.stopTimestamp);
	mksReprint.uwTick = millis();
	
	epr_write_data(EPR_SAV_ACCU, (uint8_t *)&mksReprint.accumulator,sizeof(mksReprint.accumulator));
	epr_write_data(EPR_SAV_STARTIME, (uint8_t *)&mksReprint.startTimestamp,sizeof(mksReprint.startTimestamp));
	epr_write_data(EPR_SAV_STOPTIME, (uint8_t *)&mksReprint.stopTimestamp,sizeof(mksReprint.stopTimestamp));
	epr_write_data(EPR_SAV_TICK, (uint8_t *)&mksReprint.uwTick,sizeof(mksReprint.uwTick));
#endif
	epr_write_data(EPR_SAV_HOUR, (uint8_t *)&print_time.hours,sizeof(print_time.hours));
	epr_write_data(EPR_SAV_MINUTE, (uint8_t *)&print_time.minutes,sizeof(print_time.minutes));
	epr_write_data(EPR_SAV_SECOND, (uint8_t *)&print_time.seconds,sizeof(print_time.seconds));


	//保存风扇速度
	epr_write_data(EPR_SAV_FAN, (uint8_t *)&mksReprint.fanSpeeds_0,sizeof(mksReprint.fanSpeeds_0));

	//保存feedrate
	epr_write_data(EPR_SAV_F, (uint8_t *)&mksReprint.feedrate_mm_s,sizeof(mksReprint.feedrate_mm_s));

	epr_write_data(EPR_SAV_CUR_EXTRUDE, (uint8_t *)&gCfgItems.curSprayerChoose,sizeof(gCfgItems.curSprayerChoose));

	//t2 = getTick();
	//sprintf(t12,"%d",t2-t1);
	//GUI_DispStringAt(t12,0,0);
#endif	
}


void mks_ReadFromEpr()        //暂停、断电时读取数据
{
	//读取文件名
	epr_read_data((int)EPR_SAV_FILENAME, (uint8_t*)&mksReprint.filename[0],sizeof(mksReprint.filename));  
	//读取文件偏移
	epr_read_data((int)EPR_SAV_SDPOS, (uint8_t*)&mksReprint.sdpos, sizeof(mksReprint.sdpos));

#if 0	
	if(gCfgItems.pwroff_save_mode != 1)
	{
		epr_read_data((int)EPR_SAV_SDPOS, (uint8_t*)&mksReprint.sdpos, sizeof(mksReprint.sdpos));	
	}
	else
	{
		epr_read_data((int)EPR_SAV_SDPOS, (uint8_t*)&mksReprint.sdpos_from_epr, sizeof(mksReprint.sdpos_from_epr));
		epr_read_data((int)EPR_SAV_SDPOS_BAK, (uint8_t*)&mksReprint.sdpos_bak, sizeof(mksReprint.sdpos_bak));//最后z位置
	}
	
	//读取打印头温度
	epr_read_data((int)EPR_SAV_TARGET_TEMP_0, (uint8_t*)&mksReprint.target_temperature_0, sizeof(mksReprint.target_temperature_0));
	//读取热床温度
#if HAS_TEMP_BED		//mks 20-8-07        
	if(HAS_TEMP_BED)
	{
		epr_read_data((int)EPR_SAV_TARGET_TEMP_BED, (uint8_t*)&mksReprint.target_temperature_bed, sizeof(mksReprint.target_temperature_bed));
	}
#endif        
	//读取当前位置
	epr_read_data((int)EPR_SAV_CUR_X, (uint8_t*)&mksReprint.current_position[0], sizeof(mksReprint.current_position[0]));
	epr_read_data((int)EPR_SAV_CUR_Y, (uint8_t*)&mksReprint.current_position[1], sizeof(mksReprint.current_position[1]));
	epr_read_data((int)EPR_SAV_CUR_Z, (uint8_t*)&mksReprint.current_position[2], sizeof(mksReprint.current_position[2]));
	epr_read_data((int)EPR_SAV_CUR_E, (uint8_t*)&mksReprint.current_position[3], sizeof(mksReprint.current_position[3]));
	//读取目标位置
	epr_read_data((int)EPR_SAV_DST_X, (uint8_t*)&mksReprint.destination[0], sizeof(mksReprint.destination[0]));
	epr_read_data((int)EPR_SAV_DST_Y, (uint8_t*)&mksReprint.destination[1], sizeof(mksReprint.destination[1]));
	epr_read_data((int)EPR_SAV_DST_Z, (uint8_t*)&mksReprint.destination[2], sizeof(mksReprint.destination[2]));
	epr_read_data((int)EPR_SAV_DST_E, (uint8_t*)&mksReprint.destination[3], sizeof(mksReprint.destination[3]));

#if tan_mask
	//读取进度和时间
	epr_read_data((int)EPR_SAV_ACCU, (uint8_t*)&mksReprint.accumulator, sizeof(mksReprint.accumulator));
	epr_read_data((int)EPR_SAV_STARTIME, (uint8_t*)&mksReprint.startTimestamp, sizeof(mksReprint.startTimestamp));
	epr_read_data((int)EPR_SAV_STOPTIME, (uint8_t*)&mksReprint.stopTimestamp, sizeof(mksReprint.stopTimestamp));
	epr_read_data((int)EPR_SAV_TICK, (uint8_t*)&mksReprint.uwTick, sizeof(mksReprint.uwTick));
#endif
	epr_read_data((int)EPR_SAV_HOUR, (uint8_t *)&print_time.hours,sizeof(print_time.hours));
	epr_read_data((int)EPR_SAV_MINUTE, (uint8_t *)&print_time.minutes,sizeof(print_time.minutes));
	epr_read_data((int)EPR_SAV_SECOND, (uint8_t *)&print_time.seconds,sizeof(print_time.seconds));

	//读取风扇速度
	epr_read_data((int)EPR_SAV_FAN, (uint8_t*)&mksReprint.fanSpeeds_0, sizeof(mksReprint.fanSpeeds_0));

	//读取feedrate
	epr_read_data((int)EPR_SAV_F, (uint8_t*)&mksReprint.feedrate_mm_s, sizeof(mksReprint.feedrate_mm_s));

	epr_read_data((int)EPR_SAV_CUR_EXTRUDE, (uint8_t *)&gCfgItems.curSprayerChoose,sizeof(gCfgItems.curSprayerChoose));
#endif	
}


void mks_resumePrint()
{
	if(mksReprint.mks_printer_state == MKS_PAUSED)
		{
		mksReprint.mks_printer_state = MKS_WORKING;
		
		//挤出补偿
		//mks_preExtrude(mksReprint.mks_pausePrint_e);

		//移动X,Y

		//mks_moveXY(mksReprint.destination[0],mksReprint.destination[1]);
		//Z轴下降
		//mks_moveZ(0);
		mksdlp.resumePrint();
		//lcd_setstatus("Resume print.");
		//lcd_setstatus(MSG_RESUME_PRINT);
		lcd_setstatus(mksReprint.filename);
		
		}
	
	if(mksReprint.mks_printer_state == MKS_REPRINTING)
		{
		mksReprint.mks_printer_state = MKS_REPRINTED;
		}
}



void mks_pausePrint()
{
	DLP_MOVE_STATUS cstatus;

	stepper.synchronize();
#if 0
	mksReprint.current_position[0] = current_position[X_AXIS];
	mksReprint.current_position[1] = current_position[Y_AXIS];
	mksReprint.current_position[2] = current_position[Z_AXIS];
	mksReprint.current_position[3] = current_position[E_AXIS];

	mksReprint.destination[0]=destination[X_AXIS];
	mksReprint.destination[1]=destination[Y_AXIS];
	mksReprint.destination[2]=destination[Z_AXIS];
	mksReprint.destination[3]=destination[E_AXIS];

        mksReprint.fanSpeeds_0 = fanSpeeds[0];
        mksReprint.feedrate_mm_s = feedrate_mm_s;

	//E 回抽
	mks_preExtrude(MKS_RETROVERSION);   
#endif	
	//Z轴上升
	//mks_moveZ(mksReprint.mks_pausePrint_z);
	cstatus = DLP_MOVING;
	epr_write_data(EPR_SAV_ISMOVING, (uint8_t *)&cstatus,1);

	mksdlp.moveRelative_Z(mksReprint.mks_pausePrint_z,PRINT_FEEDRATE);
	
	mksReprint.current_position[2] = current_position[Z_AXIS];

	cstatus = DLP_STOPED;
	epr_write_data(EPR_SAV_ISMOVING, (uint8_t *)&cstatus,1);

#if 0
    //移动X,Y
	mks_moveXY(mksReprint.mks_pausePrint_x,mksReprint.mks_pausePrint_y);
#endif
	//mks_WriteToEpr();
	//lcd_setstatus("Pause print.");
#if ENABLED(ULTRA_LCD)         
	lcd_setstatus(MSG_PAUSE_PRINT);
#endif
}

void mks_PrintStatePolling()
{

	if(mksReprint.mks_printer_state == MKS_PAUSING)
		{
		
		if(!mksdlp.pausePrint())	//等待灯灭
			{
#if defined(MKS_DLP_DEBUG)			
			SERIAL_PROTOCOLPGM("paused:");
			report_current_position();
#endif			
			mksReprint.mks_printer_state = MKS_PAUSED;
			//if(gCfgItems.pwroff_save_mode != 1)
			epr_write_data(EPR_SAV_FLAG, (uint8_t *)&mksReprint.mks_printer_state,sizeof(mksReprint.mks_printer_state));  //暂停标志置位
			mks_pausePrint();
			mksdlp.write_Information_paused();	
			mksdlp.buzzer_tone(1000,1000,2);
			}
		}
/*
	Cartesian机型 暂停后，LCD 移动X,Y,Z处理
	DELTA 机型 暂停后，X,Y,Z都归零，不需处理
*/
	//if(mksReprint.mks_printer_state == MKS_PAUSED && MACHINETPYE != DELTA)
#if 0	
	if(mksReprint.mks_printer_state == MKS_PAUSED)		
		{
		if( abs(mksReprint.current_position[0] - current_position[X_AXIS]) > 0.0001 )
			{
			mksReprint.current_position[0] = current_position[X_AXIS];
			epr_write_data(EPR_SAV_CUR_X, (uint8_t *)&mksReprint.current_position[0],sizeof(mksReprint.current_position[0]));
			}
		if( abs(mksReprint.current_position[1] - current_position[Y_AXIS]) > 0.0001 )
			{
			mksReprint.current_position[1] = current_position[Y_AXIS];
			epr_write_data(EPR_SAV_CUR_Y, (uint8_t *)&mksReprint.current_position[1],sizeof(mksReprint.current_position[1]));
			}
		if( abs(mksReprint.current_position[2] - current_position[Z_AXIS]) > 0.0001 )
			{
			mksReprint.current_position[2] = current_position[Z_AXIS];
			epr_write_data(EPR_SAV_CUR_Z, (uint8_t *)&mksReprint.current_position[2],sizeof(mksReprint.current_position[2]));
			}
		}
#endif
/*
	文件正常打印完后，标志位处理
*/
	if(mksReprint.mks_printer_state == MKS_WORKING)
	{
		if(pause_resum == 1)//暂停恢复
		{
			pause_resum = 0;
			//mks_resumePrint();
			//mks_preExtrude(mksReprint.mks_pausePrint_e);
			//移动X,Y
			//mks_moveXY(mksReprint.destination[0],mksReprint.destination[1]);
			//Z轴下降
			//mks_moveZ(0);
			mksdlp.resumePrint();
			card.startFileprint();
			print_job_timer.start();

		}
		
		if(card.sdprinting == 0)
			mksReprint.sdprinting++;
		else
			mksReprint.sdprinting = 0;

		if(mksReprint.sdprinting > 20)
		{
			mksReprint.sdprinting = 0;
			mksReprint.mks_printer_state = MKS_IDLE;
            //if(gCfgItems.pwroff_save_mode != 1)
            {
			    epr_write_data(EPR_SAV_FLAG, (uint8_t *)&mksReprint.mks_printer_state,sizeof(mksReprint.mks_printer_state));  
#if 0			
				mks_clearFile();
#endif
            }

		}
	}
	//停止打印
	if(mksReprint.mks_printer_state == MKS_STOP)
	{
		mksReprint.mks_printer_state = MKS_IDLE;
		//if(gCfgItems.pwroff_save_mode != 1)
			epr_write_data(EPR_SAV_FLAG, (uint8_t *)&mksReprint.mks_printer_state,sizeof(mksReprint.mks_printer_state));  
	
		clear_command_queue();
		//Z轴上升
		//mksReprint.destination[2]=destination[Z_AXIS];
		mksdlp.stopPrint();
		//mks_moveZ(mksReprint.mks_pausePrint_z);
		mksdlp.moveRelative_Z(mksReprint.mks_pausePrint_z,PRINT_FEEDRATE);
		disable_Z();
		mksdlp.buzzer_tone(1000,1000,2);
		//mksdlp.buzzer_tone(1000,1000,2);
		//mks_G28("G28 X0 Y0");
		//gcode_M998();
		//card.stopSDPrint();
		//clear_command_queue();
		//quickstop_stepper();
		
		print_job_timer.stop();
#if 0		
		thermalManager.disable_all_heaters();
	#if FAN_COUNT > 0
		  for (uint8_t i = 0; i < FAN_COUNT; i++) 
		  {
		  	fanSpeeds[i] = 0;
			MKS_FAN_TIM = 0 ;
		   }
	#endif
		wait_for_heatup = false;	
#endif
#if 0
		if(gCfgItems.pwroff_save_mode != 1)
			mks_clearFile();	
#endif		
	}
	//暂停,断电续打
	if(mksReprint.mks_printer_state == MKS_REPRINTED)
		{
#if 0       //mks 2018-07           
		switch(MACHINETPYE)
			{
			case Cartesian:
			case COREXY:
			case COREYX:	
#endif              
				if(mksReprint.resume == MKS_RESUME_PAUSE)
					mksdlp.contiuePrint_paused();
				else if(mksReprint.resume == MKS_RESUME_PWDWN) 
					mksdlp.contiuePrint_Pwdwn();
#if 0                                
			break;
			case DELTA:
				mks_contiuePrintDelta();
				break;
			default: 
				break;				
			}
#endif                
			
		}


#if 0
	//SD文件保存 "层" 信息
	
	if(mksReprint.mks_printer_state == MKS_WORKING && card.sdprinting == true)
		{
			if(mksReprint.refresh)
			{
				if(card.getsdpos()>MIN_FILE_PRINTED)
				{
					mks_WriteToFile();
				}
				
				mksReprint.refresh = false;
				
			}
		}
#endif	
}



void mks_setTemperature()
{
#if 0
	char string[20];
	
		//设置热床温度 M140
#if HAS_TEMP_BED		//mks 20-8-07        
		if(HAS_TEMP_BED && mksReprint.target_temperature_bed != 0)
			{
			memset(mksReprint.command_queue,0,MAX_CMD_SIZE);
			strcpy(mksReprint.command_queue,"M140 S");	
			//itoa(mksReprint.target_temperature_bed,string,10);
			sprintf(string,"%d",mksReprint.target_temperature_bed);
			strcat(mksReprint.command_queue,string);
			//current_command_args 
			//parser.string_arg = &mksReprint.command_queue[5];
			parser.parse(mksReprint.command_queue);
			gcode_M140();
			}
	
#endif	
			//设置打印头温度
		if(mksReprint.target_temperature_0 != 0)
		{
#if 0       //mks 2018-07_a                  
			if((mksCfg.extruders ==2))
			{
				memset(mksReprint.command_queue,0,MAX_CMD_SIZE);
				strcpy(mksReprint.command_queue,"M104 T1 S");	
			//	itoa(mksReprint.target_temperature_0,string,10);
				sprintf(string,"%d",mksReprint.target_temperature_1);
				strcat(mksReprint.command_queue,string);
				//current_command_args 
				//parser.string_arg = &mksReprint.command_queue[5];
				parser.parse(mksReprint.command_queue);
				gcode_M104();			
			}
#endif                        
			memset(mksReprint.command_queue,0,MAX_CMD_SIZE);
			strcpy(mksReprint.command_queue,"M109 T0 S");	
		//	itoa(mksReprint.target_temperature_0,string,10);
			sprintf(string,"%d",mksReprint.target_temperature_0);
			strcat(mksReprint.command_queue,string);
			//current_command_args = 
			//parser.string_arg = &mksReprint.command_queue[5];
			parser.parse(mksReprint.command_queue);
			gcode_M109();
		}
#if 0       //mks 2018-07_a                
		if((mksCfg.extruders ==2)&&(mksReprint.target_temperature_1 != 0))
		{
			memset(mksReprint.command_queue,0,MAX_CMD_SIZE);
			strcpy(mksReprint.command_queue,"M109 T1 S");	
		//	itoa(mksReprint.target_temperature_0,string,10);
			sprintf(string,"%d",mksReprint.target_temperature_1);
			strcat(mksReprint.command_queue,string);
			//current_command_args = 
			//parser.string_arg = &mksReprint.command_queue[5];
			parser.parse(mksReprint.command_queue);
			gcode_M109();
		}
#endif                
		//设置热床温度
#if HAS_TEMP_BED		//mks 20-8-07                
		if(HAS_TEMP_BED && mksReprint.target_temperature_bed != 0)
		{
			memset(mksReprint.command_queue,0,MAX_CMD_SIZE);
			strcpy(mksReprint.command_queue,"M190 S");	
			//itoa(mksReprint.target_temperature_bed,string,10);
			sprintf(string,"%d",mksReprint.target_temperature_bed);
			strcat(mksReprint.command_queue,string);
			//current_command_args 
			//parser.string_arg = &mksReprint.command_queue[5];
			parser.parse(mksReprint.command_queue);
			gcode_M190();
		}
#endif
#endif
}

void mks_setPositionZ()
{
 	char string[20];
   
	//设置Z当前位置
	memset(mksReprint.command_queue,0,MAX_CMD_SIZE);
	strcpy(mksReprint.command_queue,"G92 Z");
	memset(string,0,sizeof(string));
	sprintf(string,"%f",mksReprint.current_position[2]);
	strcat(mksReprint.command_queue,string);
	//current_command_args
	//parser.string_arg = &mksReprint.command_queue[4];
	parser.parse(mksReprint.command_queue);
	gcode_G92();
	soft_endstop_min[2] = 0;
}



void mks_setFeedrate( )
{
	//设置feedrate
	feedrate_mm_s = mksReprint.feedrate_mm_s;
}


void mks_preExtrude(float e)
{
	//挤出补偿
#if 0
	char string[20];
	relative_mode = true;

	memset(mksReprint.command_queue,0,MAX_CMD_SIZE);
	strcpy(mksReprint.command_queue,"G1 E");
	memset(string,0,sizeof(string));
	sprintf(string,"%f",e);
	strcat(mksReprint.command_queue,string);
	//current_command_args = 
	//parser.command_ptr = &mksReprint.command_queue[3];
    parser.parse(mksReprint.command_queue);
	gcode_G0_G1();
	stepper.synchronize();

	relative_mode = false;


//设置E当前位置
	memset(mksReprint.command_queue,0,MAX_CMD_SIZE);
	strcpy(mksReprint.command_queue,"G92 E");
	memset(string,0,sizeof(string));
	//sprintf(string,"%f",mksReprint.current_position[3]);
	sprintf(string,"%f",mksReprint.destination[3]);	
	strcat(mksReprint.command_queue,string);
	//current_command_args = 
	//parser.string_arg = &mksReprint.command_queue[4];
	parser.parse(mksReprint.command_queue);
	gcode_G92();
#endif
}

void mks_moveXY(float X,float Y)
{
      char string[20];
    //移动打印头X,Y位置
	memset(mksReprint.command_queue,0,MAX_CMD_SIZE);
	strcpy(mksReprint.command_queue,"G1 X");
	memset(string,0,sizeof(string));
	sprintf(string,"%f",X);
	strcat(mksReprint.command_queue,string);

	strcat(mksReprint.command_queue," Y");
	memset(string,0,sizeof(string));
	sprintf(string,"%f",Y);
	strcat(mksReprint.command_queue,string);
	
	//current_command_args = 
	//parser.string_arg = &mksReprint.command_queue[3];
	parser.parse(mksReprint.command_queue);
	gcode_G0_G1();
	stepper.synchronize();

}
void mks_moveZ(float Z)
{
      char string[20];
    //移动打印头Z位置
	memset(mksReprint.command_queue,0,MAX_CMD_SIZE);
	strcpy(mksReprint.command_queue,"G1 Z");
	memset(string,0,sizeof(string));
	sprintf(string,"%f",mksReprint.destination[2]+Z);
	strcat(mksReprint.command_queue,string);
	//current_command_args = 
	//parser.command_ptr = &mksReprint.command_queue[0];
	parser.parse(mksReprint.command_queue);
	gcode_G0_G1();
	stepper.synchronize();
}

void mks_G28(char *g_command)
{
	memset(mksReprint.command_queue,0,MAX_CMD_SIZE);
	strcpy(mksReprint.command_queue,g_command);
	//current_command_args
	//parser.string_arg = &mksReprint.command_queue[4];
	parser.parse(mksReprint.command_queue);
	gcode_G28(false);
#if 0       //mks 2018-07	
	if((BED_LEVELING_METHOD & HAS_LEVELING) && (BED_LEVELING_METHOD != AUTO_BED_LEVELING_UBL))
		{
		  set_bed_leveling_enabled(true);
	#if ENABLED(ENABLE_LEVELING_FADE_HEIGHT)
		if((MACHINETPYE != DELTA) && (BED_LEVELING_METHOD&HAS_FADE))	//mks_delta
		 { if (parser.seen('Z')) set_z_fade_height(parser.value_linear_units()/*code_value_linear_units()*/);}
	#endif

                }
#endif        
}

extern uint8_t leveling_first_time;

void mks_manual_leveling(int16_t x,int16_t y)
{
    char string[20];
    
    if(leveling_first_time == 1)
    {
        leveling_first_time = 0;
        mks_G28("G28");
    } 
    mksReprint.destination[2] = current_position[2];
    mks_moveZ(10);
    mks_moveXY((float)x,(float)y);
    enqueue_and_echo_commands_P(PSTR("G1 Z0")); 
}

void mks_contiuePrintPause()
{
	 
	char string[20];

	card.sdprinting = true;

//	mks_setTemperature();

	// X,Y归零
//	mks_G28("G28 X0 Y0");

	//挤出补偿
//	mks_preExtrude(mksReprint.mks_pausePrint_e);

	
	//移动打印头X,Y位置
//	mks_moveXY(mksReprint.destination[0],mksReprint.destination[1]);	
	//设置Z当前位置
	mks_setPositionZ();

	//移动打印头Z位置
	mks_moveZ(0);


	//设置feedrate
//	mks_setFeedrate();

	//设置风扇速度
//	fanSpeeds[0] = mksReprint.fanSpeeds_0;
//	MKS_FAN_TIM = fanSpeeds[0]*10000/255;

	//lcd_setstatus("Resume printing...");
	//lcd_setstatus(mksReprint.filename);
	mksReprint.mks_printer_state = MKS_WORKING;
	gcode_M24(); 


}

void mks_contiuePrintPwdwn()
{
#if 0
	char string[20];

	card.sdprinting = true;

//	mks_setTemperature();

	//设置Z当前位置
	mks_setPositionZ();
	//E 回抽
//	mks_preExtrude(MKS_RETROVERSION);
	//移动打印头Z位置
	mks_moveZ(mksReprint.mks_pausePrint_z);

	// X,Y归零
//	mks_G28("G28 X0 Y0");

	delay(3000);
	
	//挤出补偿
//	mks_preExtrude(mksReprint.mks_pausePrint_e);

	//移动打印头X,Y位置
//	mks_moveXY(mksReprint.destination[0],mksReprint.destination[1]);	
	//移动打印头Z位置
	mks_moveZ(0);
	
	//设置feedrate
//	mks_setFeedrate();

	//设置风扇速度
//	fanSpeeds[0] = mksReprint.fanSpeeds_0;
//	MKS_FAN_TIM = fanSpeeds[0]*10000/255;

	//lcd_setstatus("Resume printing...");
	//lcd_setstatus(mksReprint.filename);
	
	mksReprint.mks_printer_state = MKS_WORKING;
	gcode_M24(); 
	
//	mksReprint.refresh = false;
#endif
}


void mks_contiuePrintDelta()
{
	char string[20];

	card.sdprinting = true;

//	mks_setTemperature();

	
	// X,Y,Z归零
//	mks_G28("G28");
	
	//移动打印头Z位置
	mks_moveZ(mksReprint.mks_pausePrint_z);

	//挤出补偿
//	mks_preExtrude(mksReprint.mks_pausePrint_e);

	//移动打印头X,Y位置
//	mks_moveXY(mksReprint.destination[0],mksReprint.destination[1]);	

	//移动打印头Z位置
	mks_moveZ(0);

	//设置feedrate
//	mks_setFeedrate();

	//设置风扇速度
//	fanSpeeds[0] = mksReprint.fanSpeeds_0;
//	MKS_FAN_TIM = fanSpeeds[0]*10000/255;

	//lcd_setstatus("Resume printing...");
	//lcd_setstatus(mksReprint.filename);
	
	mksReprint.mks_printer_state = MKS_WORKING;
	gcode_M24(); 

//	mksReprint.refresh = false;

}

static uint8_t first_resu=1;

void mks_getPositionXYZE()
{
    volatile char *ZPOS_TEMP;
    volatile char z_dest[20];
    volatile char k=0;
#if 0    
	//设置文件偏移
	if(gCfgItems.pwroff_save_mode != 1)
	{
		card.setIndex(mksReprint.sdpos);

	
		card.sdprinting = true;
		get_sdcard_commands();
		card.sdprinting = false;
		mksReprint.refresh = false;

		LOOP_XYZE(i) 
		{
			destination[i] = current_position[i] = 0;
		}

		LOOP_XYZE(i) 
		{
			//current_command_args
			//parser.string_arg = &command_queue[i][0];
			//断点z轴第一条指令不完整，需要另外处理
          /*
			if(i!=0)
            {   
                parser.parse(&command_queue[i][0]);
                gcode_get_destination();
            }
            else
            {
               ZPOS_TEMP = strchr(&command_queue[i][0],'Z');
               while((*ZPOS_TEMP++ != 0))
               {
                    z_dest[k++]=*ZPOS_TEMP;
                    if(k>20)break;
               }
               destination[2]=atof((char*)z_dest);
            }
           */
                if((i==0)&&(first_resu==1))
                {
                   first_resu = 0;
                   ZPOS_TEMP = strchr(&command_queue[i][0],'Z');
                   if(ZPOS_TEMP)
                   {
                     while((*ZPOS_TEMP++ != 0))
                     {
                          z_dest[k++]=*ZPOS_TEMP;
                          if(k>20)break;
                     }
                     destination[2]=atof((char*)z_dest);                  
                   }
                }
                else
                {            
				    parser.parse(&command_queue[i][0]);
				    gcode_get_destination();                 
                }          
			
			

			LOOP_XYZE(j)
			{
				if(abs(mksReprint.destination[j])< 0.0001 && abs(destination[j]) > 0.0001)
					mksReprint.destination[j] = destination[j];
			}
		}

		mksReprint.current_position[2] = mksReprint.destination[2];
		mksReprint.current_position[3] = mksReprint.destination[3];
	}
	else
	{
		LOOP_XYZE(i) 
		{
			destination[i] = current_position[i] = 0;
		}
		card.setIndex(mksReprint.sdpos_bak);
		while(card.getsdpos() < mksReprint.sdpos_from_epr)
		{
			commands_in_queue = 0;	
			card.sdprinting = true;
			get_sdcard_commands();

			LOOP_XYZE(i) 
			{
				//current_command_args
				//parser.string_arg = &command_queue[i][0];
                /*
				if(i != 0)
                {            
				    parser.parse(&command_queue[i][0]);
				    gcode_get_destination();
                }
                else
                {
                    ZPOS_TEMP = strchr(&command_queue[i][0],'Z');
                    while((*ZPOS_TEMP++ != 0))
                    {
                        z_dest[k++]=*ZPOS_TEMP;
                        if(k>20)break;
                    }
                    destination[2]=atof((char*)z_dest);
                }
                */
                if((i==0)&&(first_resu==1))
                {
                   first_resu = 0;
                   ZPOS_TEMP = strchr(&command_queue[i][0],'Z');
                   if(ZPOS_TEMP)
                   {
                     while((*ZPOS_TEMP++ != 0))
                     {
                          z_dest[k++]=*ZPOS_TEMP;
                          if(k>20)break;
                     }
                     destination[2]=atof((char*)z_dest);  
                   }
                   
                }
                else
                {            
				    parser.parse(&command_queue[i][0]);
				    gcode_get_destination();                 
                }
                
				LOOP_XYZE(j)
				{
					if(abs(mksReprint.destination[j])< 0.0001 && abs(destination[j]) > 0.0001)
						mksReprint.destination[j] = destination[j];

						if(j!=2&&(abs(destination[j]> 0.0001)))
						{
							mksReprint.destination[j] = destination[j];
						}
					
				}
			}
			mksReprint.current_position[2] = mksReprint.destination[2];
			mksReprint.current_position[3] = mksReprint.destination[3];			
		}

		
		card.sdprinting = false;
		mksReprint.refresh = false;

	}





	LOOP_XYZE(i) 
	{
	 	destination[i] = current_position[i] = 0;
	}
	for(int i=0;i<BUFSIZE;i++)
		memset(&command_queue[i][0],0,MAX_CMD_SIZE);
	clear_command_queue();
	
#endif
}

void mks_clearDir()
{
	char *tmp_index = NULL;
	char string[30];

// 将mksReprint.filename字符串中的目录部分去掉，只保留文件名
	while(1)
		{
		tmp_index = (char *)strstr(mksReprint.filename, "/");
		if(tmp_index)
			{
			strcpy(string,tmp_index+1);
			strcpy(mksReprint.filename,string);
			}
		else
			break;
		}
	
}

void mks_contiuePrint_UI()
{

	
	lcd_setstatus("Resume print?");
	
	card.openFile(mksReprint.filename, true);
	strcpy(curFileName,mksReprint.filename);

	if((strstr((char *)curFileName,".cbddlp")!=NULL)
		||(strstr((char *)curFileName,".CBDDLP")!=NULL)
		||(strstr((char *)curFileName,".photon")!=NULL)
		||(strstr((char *)curFileName,".PHOTON")!=NULL))
	{
		mksdlp.print_file_type= 2;
	}	
	else 
	{
		mksdlp.print_file_type= 1;
	}
	
	if(!card.isFileOpen())	//打开续打文件失败
	{
		draw_dialog(DIALOG_TYPE_REPRINT_NO_FILE);
	}
	else
	{	
           
    		epr_write_data(EPR_SAV_FILENAME, (uint8_t *)&mksReprint.filename[0],sizeof(mksReprint.filename)); 
    		card.sdprinting = 0;

    			mks_clearDir();
    		while(1)
    		{
    			logo_tick2 = getTick();
    			if((getTickDiff(logo_tick2, logo_tick1)>=3000)
    				||(gCfgItems.fileSysType == FILE_SYS_USB))
    			{
    				AT24CXX_Read(EPR_PREVIEW_FROM_FLASH, &from_flash_pic,1);
    				
    				if(from_flash_pic != 0)
    				{
    					flash_preview_begin = 1;
    				}
    				else
    				{
    					default_preview_flg = 1;
    				}		
    				draw_printing();
					mksdlp.draw_printing();
    				break;
    			}
    		}
        
	}
	
	
}

/*
void mks_rePrintCheck() 实现
暂停 -> 断电 -> 断点续打
直接断电 -> 断层续打
暂停续打测试步骤:
1.M32 *.gcode --打开并打印文件
2.M25		  -- 暂停打印
3.断电复位
4.M24		  -- 续打
断电续打测试步骤:
1.M32 *.gcode --打开并打印文件
2.断电复位
3.M24		  -- 续打
*/


volatile MKS_PRINTER_STATE ctemp;
void mks_rePrintCheck()
{
	//bool pft_ok = false;
   /*
	if ((!card.cardOK)&&(!card.usbOK)) 
    {
    
	    epr_read_data((int)EPR_SAV_FLAG, (uint8_t*)&mksReprint.mks_printer_state, sizeof(mksReprint.mks_printer_state));
        if((mksReprint.mks_printer_state == MKS_PAUSED)
            ||(mksReprint.mks_printer_state == MKS_WORKING))
        {            
			epr_read_data((int)EPR_SAV_FILENAME, (uint8_t*)&mksReprint.filename[0],sizeof(mksReprint.filename)); 
            draw_dialog(DIALOG_TYPE_REPRINT_NO_FILE);
        }
        return; 
    }
	*/
	//需要确保USB已经准备好
#if 0	//skyblue 2018-08
	if(gCfgItems.fileSysType == FILE_SYS_USB)
	{
		while(1)
		{
			logo_tick2 = getTick();
			if(getTickDiff(logo_tick2, logo_tick1)>=3000)
			{
				break;
			}
			MX_USB_HOST_Process();
		}				
	}	
#endif

	epr_read_data((int)EPR_SAV_FLAG, (uint8_t*)&mksReprint.mks_printer_state, sizeof(mksReprint.mks_printer_state));
	ctemp = mksReprint.mks_printer_state;
        if(mksReprint.mks_printer_state == MKS_PAUSED && mksdlp.Is_Paused_Finished())		//续打
		{
			//if(mksdlp.Is_Paused_Finished())
			//	{
				mksReprint.mks_printer_state = MKS_REPRINTING;
				mksdlp.read_Information_paused();		//从EERPOM读取 续打参数
				mksReprint.resume = MKS_RESUME_PAUSE;
				mks_contiuePrint_UI();
			//	}
#if 0			
			else
				{
				mksReprint.mks_printer_state = MKS_IDLE;
				epr_write_data(EPR_SAV_FLAG, (uint8_t *)&mksReprint.mks_printer_state,sizeof(mksReprint.mks_printer_state));
				continue_print_error_flg = 1;
				}
#endif			
		
		}
	else if(mksReprint.mks_printer_state == MKS_WORKING && mksdlp.read_Information_layer())	//断电续打
		{
			//if(mksdlp.read_Information_layer())
			//	{
				mksReprint.mks_printer_state = MKS_REPRINTING;
				mksReprint.resume = MKS_RESUME_PWDWN;
				mks_contiuePrint_UI();
		//		}
#if 0			
			else
				{
				mksReprint.mks_printer_state = MKS_IDLE;
				epr_write_data(EPR_SAV_FLAG, (uint8_t *)&mksReprint.mks_printer_state,sizeof(mksReprint.mks_printer_state));
				continue_print_error_flg = 1;
				}
#endif			
		}
	else
		{
		mksReprint.mks_printer_state = MKS_IDLE;
	    epr_write_data(EPR_SAV_FLAG, (uint8_t *)&mksReprint.mks_printer_state,sizeof(mksReprint.mks_printer_state));
 		continue_print_error_flg = 1;
		}

	if(continue_print_error_flg == 1)
	{
		continue_print_error_flg = 0;
		while(1)
		{
			logo_tick2 = getTick();
			if(getTickDiff(logo_tick2, logo_tick1)>=3000)
			{
				draw_ready_print();
				break;
			}
		}		
	}
}


void mks_WriteToFile()
{
	char string[20];
	//mksReprint.card.openFile(MKS_PFT_NAME, false);
	//if (mksReprint.card.isFileOpen()) 
	char name[30]={0};
#if 0	
	if(card.cardOK)
		strcat(name,SD_Path);
	else
		strcat(name,USBH_Path);

	strcat(name,mks_pft_name);
        
	if (f_open(&mksReprint.pft, (const TCHAR *)name, FA_OPEN_EXISTING | FA_READ | FA_WRITE) == FR_OK)
		{
		memset(string,0,sizeof(string));
		strcpy(string,"start\n");
		//mksReprint.card.write_command(string);	//"start"
		f_printf(&mksReprint.pft,string);

		//保存文件位置
		memset(string,0,sizeof(string));
		sprintf(string,"%d",mksReprint.sdpos);
		//mksReprint.card.write_command(string);
		strcat(string,"\n");
		f_printf(&mksReprint.pft,string);
		//保存打印头温度
		memset(string,0,sizeof(string));
		sprintf(string,"%d",thermalManager.target_temperature[0]);
		strcat(string,"\n");
		f_printf(&mksReprint.pft,string);
		
		memset(string,0,sizeof(string));
		sprintf(string,"%d",thermalManager.target_temperature[1]);
		strcat(string,"\n");
		f_printf(&mksReprint.pft,string);		
		
		//保存热床温度
#if HAS_TEMP_BED		//mks 20-8-07                
		//if(HAS_TEMP_BED)
		{
			memset(string,0,sizeof(string));
			sprintf(string,"%d",thermalManager.target_temperature_bed);
			//mksReprint.card.write_command(string);
			strcat(string,"\n");
			f_printf(&mksReprint.pft,string);
			
		}
#endif
		//保存进度和时间
		print_job_timer.getTime(&mksReprint.accumulator,&mksReprint.startTimestamp,&mksReprint.stopTimestamp);
		mksReprint.uwTick = millis();
		if(mksReprint.startTimestamp > mksReprint.stopTimestamp  ) mksReprint.stopTimestamp = mksReprint.uwTick;

		memset(string,0,sizeof(string));					//accumulator
		sprintf(string,"%d",mksReprint.accumulator);
		//mksReprint.card.write_command(string);
		strcat(string,"\n");
		f_printf(&mksReprint.pft,string);
#if tan_mask
		memset(string,0,sizeof(string));					//startTimestamp
		sprintf(string,"%d",mksReprint.startTimestamp);
		//mksReprint.card.write_command(string);
		strcat(string,"\n");
		f_printf(&mksReprint.pft,string);

		memset(string,0,sizeof(string));					//stopTimestamp
		sprintf(string,"%d",mksReprint.stopTimestamp);
		//mksReprint.card.write_command(string);
		strcat(string,"\n");
		f_printf(&mksReprint.pft,string);

		memset(string,0,sizeof(string));					//uwTick
		sprintf(string,"%d",mksReprint.uwTick);
		//mksReprint.card.write_command(string);
		strcat(string,"\n");
		f_printf(&mksReprint.pft,string);
#endif
		//打印时间:hour
		memset(string,0,sizeof(string));					//uwTick
		sprintf(string,"%d",print_time.hours);
		//mksReprint.card.write_command(string);
		strcat(string,"\n");
		f_printf(&mksReprint.pft,string);
		
		//打印时间:min
		memset(string,0,sizeof(string));					//uwTick
		sprintf(string,"%d",print_time.minutes);
		//mksReprint.card.write_command(string);
		strcat(string,"\n");
		f_printf(&mksReprint.pft,string);
		
		//打印时间:sec
		memset(string,0,sizeof(string));					//uwTick
		sprintf(string,"%d",print_time.seconds);
		//mksReprint.card.write_command(string);
		strcat(string,"\n");
		f_printf(&mksReprint.pft,string);	
		
		//保存风扇
		memset(string,0,sizeof(string));
		sprintf(string,"%d",fanSpeeds[0]);
		//mksReprint.card.write_command(string);
		strcat(string,"\n");
		f_printf(&mksReprint.pft,string);
		
		//保存feedrate
		memset(string,0,sizeof(string));
		sprintf(string,"%f",feedrate_mm_s);
		//mksReprint.card.write_command(string);
		strcat(string,"\n");
		f_printf(&mksReprint.pft,string);
		
		//保存当前挤出头
		memset(string,0,sizeof(string));
		sprintf(string,"%d",active_extruder);
		//mksReprint.card.write_command(string);
		strcat(string,"\n");
		f_printf(&mksReprint.pft,string);


		memset(string,0,sizeof(string));
		strcpy(string,"end");						//"end"
		//mksReprint.card.write_command(string);	
		strcat(string,"\n");
		f_printf(&mksReprint.pft,string);
		
		//mksReprint.card.closefile(true);
		f_close(&mksReprint.pft);
		}
	
#endif
}

void mks_WriteToEpr_pwroff()        //断电时保存数据，以备断电后续打
{ 
#if 0

    //打印状态
    epr_write_data(EPR_SAV_FLAG, (uint8_t *)&mksReprint.mks_printer_state,sizeof(mksReprint.mks_printer_state));

	//保存文件位置
	epr_write_data(EPR_SAV_SDPOS_BAK, (uint8_t *)&mksReprint.sdpos_bak,sizeof(mksReprint.sdpos_bak));
	
	mksReprint.sdpos = card.getsdpos();
	epr_write_data(EPR_SAV_SDPOS, (uint8_t *)&mksReprint.sdpos,sizeof(mksReprint.sdpos));	
	
	//保存打印头温度
	mksReprint.target_temperature_0 = thermalManager.target_temperature[0];
	epr_write_data(EPR_SAV_TARGET_TEMP_0, (uint8_t *)&mksReprint.target_temperature_0,sizeof(mksReprint.target_temperature_0));
#if 0       //mks 2018-07_a	
        if(mksCfg.extruders == 2)
	{
		mksReprint.target_temperature_1 = thermalManager.target_temperature[1];
		epr_write_data(EPR_SAV_TARGET_TEMP_1, (uint8_t *)&mksReprint.target_temperature_1,sizeof(mksReprint.target_temperature_1));
		
	}
#endif        
	//保存热床温度
#if HAS_TEMP_BED		//mks 20-8-07        
	if(HAS_TEMP_BED)
	{
		mksReprint.target_temperature_bed = thermalManager.target_temperature_bed;
		epr_write_data(EPR_SAV_TARGET_TEMP_BED, (uint8_t *)&mksReprint.target_temperature_bed,sizeof(mksReprint.target_temperature_bed));
	}
#endif
	epr_write_data(EPR_SAV_HOUR, (uint8_t *)&print_time.hours,sizeof(print_time.hours));
	epr_write_data(EPR_SAV_MINUTE, (uint8_t *)&print_time.minutes,sizeof(print_time.minutes));
	epr_write_data(EPR_SAV_SECOND, (uint8_t *)&print_time.seconds,sizeof(print_time.seconds));

	//保存风扇速度
	mksReprint.fanSpeeds_0 = fanSpeeds[0];
	epr_write_data(EPR_SAV_FAN, (uint8_t *)&mksReprint.fanSpeeds_0,sizeof(mksReprint.fanSpeeds_0));

	//保存feedrate
	mksReprint.feedrate_mm_s = feedrate_mm_s;
	epr_write_data(EPR_SAV_F, (uint8_t *)&mksReprint.feedrate_mm_s,sizeof(mksReprint.feedrate_mm_s));

	epr_write_data(EPR_SAV_CUR_EXTRUDE, (uint8_t *)&active_extruder,sizeof(active_extruder));
#endif
}



void mks_ReadFromEpr_pwroff()        //断电时读取数据
{
#if 0

	//读取文件名
	epr_read_data((int)EPR_SAV_FILENAME, (uint8_t*)&mksReprint.filename[0],sizeof(mksReprint.filename));  
	//读取文件偏移

	epr_read_data((int)EPR_SAV_SDPOS, (uint8_t*)&mksReprint.sdpos_from_epr, sizeof(mksReprint.sdpos_from_epr));
	epr_read_data((int)EPR_SAV_SDPOS_BAK, (uint8_t*)&mksReprint.sdpos_bak, sizeof(mksReprint.sdpos_bak));//最后z位置
	
	//读取打印头温度
	epr_read_data((int)EPR_SAV_TARGET_TEMP_0, (uint8_t*)&mksReprint.target_temperature_0, sizeof(mksReprint.target_temperature_0));
#if 0       //mks 2018-07_a	
        if(mksCfg.extruders == 2)
		epr_read_data((int)EPR_SAV_TARGET_TEMP_1, (uint8_t*)&mksReprint.target_temperature_1, sizeof(mksReprint.target_temperature_1));
#endif
	//读取热床温度
#if HAS_TEMP_BED		//mks 20-8-07        
	if(HAS_TEMP_BED)
	{
		epr_read_data((int)EPR_SAV_TARGET_TEMP_BED, (uint8_t*)&mksReprint.target_temperature_bed, sizeof(mksReprint.target_temperature_bed));
	}
#endif        
#if tan_mask
	//读取进度和时间
	epr_read_data((int)EPR_SAV_ACCU, (uint8_t*)&mksReprint.accumulator, sizeof(mksReprint.accumulator));
	epr_read_data((int)EPR_SAV_STARTIME, (uint8_t*)&mksReprint.startTimestamp, sizeof(mksReprint.startTimestamp));
	epr_read_data((int)EPR_SAV_STOPTIME, (uint8_t*)&mksReprint.stopTimestamp, sizeof(mksReprint.stopTimestamp));
	epr_read_data((int)EPR_SAV_TICK, (uint8_t*)&mksReprint.uwTick, sizeof(mksReprint.uwTick));
#endif
	epr_read_data((int)EPR_SAV_HOUR, (uint8_t *)&print_time.hours,sizeof(print_time.hours));
	epr_read_data((int)EPR_SAV_MINUTE, (uint8_t *)&print_time.minutes,sizeof(print_time.minutes));
	epr_read_data((int)EPR_SAV_SECOND, (uint8_t *)&print_time.seconds,sizeof(print_time.seconds));

	//读取风扇速度
	epr_read_data((int)EPR_SAV_FAN, (uint8_t*)&mksReprint.fanSpeeds_0, sizeof(mksReprint.fanSpeeds_0));

	//读取feedrate
	epr_read_data((int)EPR_SAV_F, (uint8_t*)&mksReprint.feedrate_mm_s, sizeof(mksReprint.feedrate_mm_s));

	epr_read_data((int)EPR_SAV_CUR_EXTRUDE, (uint8_t *)&active_extruder,sizeof(active_extruder));
#endif	
}


int16_t pft_get() {
#if 0	
	BYTE readByte;
	UINT rc;
	if (f_read(&mksReprint.pft, &readByte, 1, &rc) != FR_OK)				   
	{
		readByte = -1;
	}
	else
	{
		mksReprint.pftpos += rc;
	}
	return (int16_t) readByte;
#endif	
}



bool mks_get_commands() {
	char string[20];
	char commands_count = 0;
  uint16_t sd_count = 0;
//  bool card_eof = false;
	bool end_ok = false;
#if 0  
  //while (!card_eof ) 
  while(mksReprint.pftpos < mksReprint.filesize)
  	{
	const int16_t n = pft_get();
	if(n == -1)	break;
	char sd_char = (char)n;
	
	
	if ( sd_char == '\n') 	//"start\r\n"
		{
		string[sd_count-1] = 0;		//去掉 "\r"
		switch(commands_count)
			{
			case 0:			//start
				break;
			case 1:			//读取文件位置
				mksReprint.sdpos = atoi(string);
				break;
			case 2:			//读取打印头温度
				mksReprint.target_temperature_0 = atoi(string);
				break;
			case 3:			//读取打印头温度
				mksReprint.target_temperature_1 = atoi(string);
				break;				
			case 4:			//读取热床温度
				mksReprint.target_temperature_bed = atoi(string);
				break;
			case 5:			//accumulator
				mksReprint.accumulator = atoi(string);
				break;
#if tan_mask
			case 5:			//startTimestamp
				mksReprint.startTimestamp = atoi(string);
				break;
			case 6:			//stopTimestamp
				mksReprint.stopTimestamp = atoi(string);
				break;
			case 7:			//uwTick
				mksReprint.uwTick = atoi(string);
				break;
#endif	
			case 6:			//hour
				print_time.hours= atoi(string);
				break;
			case 7:			//min
				print_time.minutes= atoi(string);
				break;
			case 8:			//second
				print_time.seconds = atoi(string);
				break;

			case 9:			//读取风扇
				mksReprint.fanSpeeds_0 = atoi(string);
				break;
			case 10:		//读取feedrate
				mksReprint.feedrate_mm_s = atof(string);
				break;
			case 11:		//读取当前挤出头
				gCfgItems.curSprayerChoose = atof(string);
				break;				
			case 12:		//end
				if(strcmp(string,"end") == 0)
					end_ok = true;
				break;
			default: break;				
				
			}

	  	sd_count = 0;
		memset(string,0,sizeof(string));
		commands_count++;
		}
	else 
		{
		  string[sd_count++] = sd_char;
		}
		
  }
#endif
  return(end_ok);
}



bool mks_ReadFromFile()
{
	bool get_ok = false;
	//mksReprint.card.openFile(MKS_PFT_NAME, true);
	//if (mksReprint.card.isFileOpen()) 
       	char name[30]={0};
#if 0
	if(card.cardOK)
		strcat(name,SD_Path);
	else
		strcat(name,USBH_Path);

	strcat(name,mks_pft_name);
        
	if (f_open(&mksReprint.pft, (const TCHAR *)name, FA_OPEN_EXISTING | FA_READ) == FR_OK)		
	{
		mksReprint.pftpos = 0;
		mksReprint.filesize = f_size(&mksReprint.pft);
		get_ok = mks_get_commands();

		//mksReprint.card.closefile(true);
		f_close(&mksReprint.pft);
	}
	
	return(get_ok);
#endif
}

void mks_clearFile()
{
	char string[20]="clear";
	char name[30]={0};
#if 0	
	if(card.cardOK)
		strcat(name,SD_Path);
	else
		strcat(name,USBH_Path);

	strcat(name,mks_pft_name);

	
	if (f_open(&mksReprint.pft, (const TCHAR *)name, FA_CREATE_ALWAYS |  FA_WRITE) == FR_OK)
        {
          f_printf(&mksReprint.pft,string);
          f_close(&mksReprint.pft);
        }
#endif	
}

void mks_saveFileName(char *name)
{
		memset(mksReprint.filename,0,sizeof(mksReprint.filename));
	  
	//保存文件名
		strcpy(mksReprint.filename,name);
		epr_write_data(EPR_SAV_FILENAME, (uint8_t *)&mksReprint.filename[0],sizeof(mksReprint.filename)); 
#if 0
	//清除 mks_pft.sys文件中的记录
		if((mksReprint.mks_printer_state == MKS_IDLE)
			&&(gCfgItems.pwroff_save_mode != 1))	/*打开新文件时，才清除记录。续打时不清除*/
                  mks_clearFile();
#endif
}

extern "C" void EXTI9_5_IRQHandler(void);

void EXTI9_5_IRQHandler(void)
{
  //cli(); // Stop interrupts
#if 0  
  Lcd_Light_OFF;
  if(__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_5) != RESET)
  {
    __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_5);
	
	if(mksReprint.mks_printer_state == MKS_IDLE)
	{
		FALA_CTRL = 0;
		return;
	}
	if((mksReprint.mks_printer_state == MKS_REPRINTING)
		||(mksReprint.mks_printer_state == MKS_REPRINTING))
	{
		FALA_CTRL = 0;
		return;	
	}
	if((mksReprint.mks_printer_state == MKS_PAUSING)
		||(mksReprint.mks_printer_state == MKS_PAUSED))
	{
		FALA_CTRL = 0;
		return;	
	}
	
	if(gCfgItems.pwroff_save_mode == 1)
	{
		EXT_CTRL = 0;
    	mks_WriteToEpr_pwroff();
		delay(1000);
		FALA_CTRL = 0;
		while(1);
	}
  }
  sei(); // 开启 interrupts
#endif  
}

