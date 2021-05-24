// cbd_util.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include "windows.h"


#include "main.h"

int			_tmain(int argc, _TCHAR* argv[])
{
	printf("\r\nctb_util v1.0\r\n");

	SetLocaleInfo(0x0419, LOCALE_IDEFAULTCODEPAGE, _T("1251"));
	if (argc < 3)
	{
		print_help();
		return 1;
	}


	char				ctbfname[1024];	// ctb filename

	DWORD				toread = 0, readed = 0;
	BYTE				buffer[4096];
	uint32_t			bpos = 0;
	HANDLE				rfile = NULL;

	FCTB_HEADER			ctb_header;
	FCTB_INFO			ctb_info;
	FCTB_INFOEXT		ctb_infoext;
	FCTB_LAYERSINFO		ctb_layerinfo;


	CharToOem(argv[1], ctbfname);
	rfile = CreateFile(argv[1], FILE_READ_DATA, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (rfile == INVALID_HANDLE_VALUE)
	{
		printf("\r\nCan't open file <%s>\r\n\r\n", ctbfname);
		return 2;
	}

	toread = sizeof(FCTB_HEADER);
	ReadFile(rfile, &ctb_header, toread, &readed, NULL);
	if (readed != toread)
	{
		CloseHandle(rfile);
		printf("\r\nFile <%s> head read error\r\n\r\n", ctbfname);
		return 3;
	}

	
	if (SetFilePointer(rfile, ctb_header.print_parameters_offset, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
	{
		CloseHandle(rfile);
		printf("\r\nFile <%s> info read error\r\n\r\n", ctbfname);
		return 4;
	}
	toread = sizeof(FCTB_INFO);
	ReadFile(rfile, &ctb_info, toread, &readed, NULL);
	if (readed != toread)
	{
		CloseHandle(rfile);
		printf("\r\nFile <%s> info read error\r\n\r\n", ctbfname);
		return 5;
	}

	if (SetFilePointer(rfile, ctb_header.slicer_offset, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
	{
		CloseHandle(rfile);
		printf("\r\nFile <%s> infoext read error\r\n\r\n", ctbfname);
		return 6;
	}
	toread = sizeof(FCTB_INFOEXT);
	ReadFile(rfile, &ctb_infoext, toread, &readed, NULL);
	if (readed != toread)
	{
		CloseHandle(rfile);
		printf("\r\nFile <%s> infoext read error\r\n\r\n", ctbfname);
		return 7;
	}


	if (_tcscmp(argv[2], _T("info")) == 0)
	{
		char msg[4096];
		uint32_t	msglen = 0;
		sprintf_s(msg, 4096-msglen, "\r\n\tFILE HEADER:");
		msglen = strlen(msg);
		sprintf_s(msg+msglen, 4096-msglen, "\r\nheader  ----------------------  0x%08x", ctb_header.header);
		msglen = strlen(msg);
		sprintf_s(msg+msglen, 4096-msglen, "\r\nversion  ---------------------  0x%08x", ctb_header.version);
		msglen = strlen(msg);
		sprintf_s(msg+msglen, 4096-msglen, "\r\nbed_x_mm  --------------------  %0.2f", ctb_header.bed_x_mm);
		msglen = strlen(msg);
		sprintf_s(msg+msglen, 4096-msglen, "\r\nbed_y_mm  --------------------  %0.2f", ctb_header.bed_y_mm);
		msglen = strlen(msg);
		sprintf_s(msg+msglen, 4096-msglen, "\r\nbed_z_mm  --------------------  %0.2f", ctb_header.bed_z_mm);
		msglen = strlen(msg);
		sprintf_s(msg+msglen, 4096-msglen, "\r\nmodel_height_mm  -------------  %0.3f", ctb_header.model_height_mm);
		msglen = strlen(msg);
		sprintf_s(msg+msglen, 4096-msglen, "\r\nslicer_layers_thickness  -----  %0.3f", ctb_header.slicer_layers_thickness);
		msglen = strlen(msg);
		sprintf_s(msg+msglen, 4096-msglen, "\r\nslicer_exp_time  -------------  %02.2f", ctb_header.slicer_exp_time);
		msglen = strlen(msg);
		sprintf_s(msg+msglen, 4096-msglen, "\r\nslicer_expbottom_time  -------  %0.2f", ctb_header.slicer_expbottom_time);
		msglen = strlen(msg);
		sprintf_s(msg+msglen, 4096-msglen, "\r\nslicer_lightoff_time  --------  %0.2f", ctb_header.slicer_lightoff_time);
		msglen = strlen(msg);
		sprintf_s(msg+msglen, 4096-msglen, "\r\nbottom_layers  ---------------  %d", ctb_header.bottom_layers);
		msglen = strlen(msg);
		sprintf_s(msg+msglen, 4096-msglen, "\r\nres_x  -----------------------  %d", ctb_header.res_x);
		msglen = strlen(msg);
		sprintf_s(msg+msglen, 4096-msglen, "\r\nres_y  -----------------------  %d", ctb_header.res_y);
		msglen = strlen(msg);
		sprintf_s(msg+msglen, 4096-msglen, "\r\npreview_large_offset  --------  0x%08x (%d)", ctb_header.preview_large_offset, ctb_header.preview_large_offset);
		msglen = strlen(msg);
		sprintf_s(msg+msglen, 4096-msglen, "\r\nlayersdef_offset  ------------  0x%08x (%d)", ctb_header.layersdef_offset, ctb_header.layersdef_offset);
		msglen = strlen(msg);
		sprintf_s(msg+msglen, 4096-msglen, "\r\ntotal_layers  ----------------  %d", ctb_header.total_layers);
		msglen = strlen(msg);
		sprintf_s(msg+msglen, 4096-msglen, "\r\npreview_small_offset  --------  0x%08x (%d)", ctb_header.preview_small_offset, ctb_header.preview_small_offset);
		msglen = strlen(msg);
		sprintf_s(msg+msglen, 4096-msglen, "\r\nprint_time_seconds  ----------  %d (%02d:%02d:%02d)", ctb_header.print_time_seconds, ctb_header.print_time_seconds / 3600, (ctb_header.print_time_seconds / 60) % 60, ctb_header.print_time_seconds % 60);
		msglen = strlen(msg);
		sprintf_s(msg+msglen, 4096-msglen, "\r\nmirror_show  -----------------  0x%08x", ctb_header.mirror_show);
		msglen = strlen(msg);
		sprintf_s(msg+msglen, 4096-msglen, "\r\nprint_parameters_offset  -----  0x%08x (%d)", ctb_header.print_parameters_offset, ctb_header.print_parameters_offset);
		msglen = strlen(msg);
		sprintf_s(msg+msglen, 4096-msglen, "\r\nprint_parameters_size  -------  0x%08x (%d)", ctb_header.print_parameters_size, ctb_header.print_parameters_size);
		msglen = strlen(msg);
		sprintf_s(msg+msglen, 4096-msglen, "\r\naa_grade  --------------------  %d", ctb_header.aa_grade);
		msglen = strlen(msg);
		sprintf_s(msg+msglen, 4096-msglen, "\r\nlight_pwm  -------------------  %d", ctb_header.light_pwm);
		msglen = strlen(msg);
		sprintf_s(msg+msglen, 4096-msglen, "\r\nlight_bottom_pwm  ------------  %d", ctb_header.light_bottom_pwm);
		msglen = strlen(msg);
		sprintf_s(msg+msglen, 4096-msglen, "\r\nencryption_key  --------------  0x%08x", ctb_header.encryption_key);
		msglen = strlen(msg);
		sprintf_s(msg+msglen, 4096-msglen, "\r\nslicer_offset  ---------------  0x%08x (%d)", ctb_header.slicer_offset, ctb_header.slicer_offset);
		msglen = strlen(msg);
		sprintf_s(msg+msglen, 4096-msglen, "\r\nslicer_size  -----------------  0x%08x (%d)", ctb_header.slicer_size, ctb_header.slicer_size);
		msglen = strlen(msg);
		sprintf_s(msg+msglen, 4096-msglen, "\r\n");
		msglen = strlen(msg);

		sprintf_s(msg+msglen, 4096-msglen, "\r\n\tFILE INFO:");
		msglen = strlen(msg);
		sprintf_s(msg+msglen, 4096-msglen, "\r\nlift_height_bottom  ----------  %0.2f", ctb_info.lift_height_bottom);
		msglen = strlen(msg);
		sprintf_s(msg+msglen, 4096-msglen, "\r\nlift_speed_bottom  -----------  %0.2f", ctb_info.lift_speed_bottom);
		msglen = strlen(msg);
		sprintf_s(msg+msglen, 4096-msglen, "\r\nlift_height  -----------------  %0.2f", ctb_info.lift_height);
		msglen = strlen(msg);
		sprintf_s(msg+msglen, 4096-msglen, "\r\nlift_speed  ------------------  %0.2f", ctb_info.lift_speed);
		msglen = strlen(msg);
		sprintf_s(msg+msglen, 4096-msglen, "\r\ndown_speed  ------------------  %0.2f", ctb_info.down_speed);
		msglen = strlen(msg);
		sprintf_s(msg+msglen, 4096-msglen, "\r\nresin_volume  ----------------  %0.2f", ctb_info.resin_volume);
		msglen = strlen(msg);
		sprintf_s(msg+msglen, 4096-msglen, "\r\nresin_weight  ----------------  %0.2f", ctb_info.resin_weight);
		msglen = strlen(msg);
		sprintf_s(msg+msglen, 4096-msglen, "\r\nresin_price  -----------------  %0.2f", ctb_info.resin_price);
		msglen = strlen(msg);
		sprintf_s(msg+msglen, 4096-msglen, "\r\nslicer_lightoff_time_bottom  -  %0.2f", ctb_info.slicer_lightoff_time_bottom);
		msglen = strlen(msg);
		sprintf_s(msg+msglen, 4096-msglen, "\r\nslicer_lightoff_time  --------  %d", ctb_info.slicer_lightoff_time);
		msglen = strlen(msg);
		sprintf_s(msg+msglen, 4096-msglen, "\r\nbottom_layers  ---------------  %d", ctb_info.bottom_layers);
		msglen = strlen(msg);

		sprintf_s(msg+msglen, 4096-msglen, "\r\n\tFILE INFOEXT:");
		msglen = strlen(msg);
		sprintf_s(msg+msglen, 4096-msglen, "\r\nmachine_type_offset  ---------  0x%08x (%d)", ctb_infoext.machine_type_offset, ctb_infoext.machine_type_offset);
		msglen = strlen(msg);
		sprintf_s(msg+msglen, 4096-msglen, "\r\nmachine_type_len  ------------  0x%08x (%d)", ctb_infoext.machine_type_len, ctb_infoext.machine_type_len);
		msglen = strlen(msg);
		sprintf_s(msg+msglen, 4096-msglen, "\r\nencryption_mode  -------------  0x%08x", ctb_infoext.encryption_mode);
		msglen = strlen(msg);
		sprintf_s(msg+msglen, 4096-msglen, "\r\ncount_unknown  ---------------  0x%08x (%d)", ctb_infoext.count_unknown, ctb_infoext.count_unknown);
		msglen = strlen(msg);
		sprintf_s(msg+msglen, 4096-msglen, "\r\naa_grade  --------------------  %d", ctb_infoext.aa_grade);
		msglen = strlen(msg);
		sprintf_s(msg+msglen, 4096-msglen, "\r\nsoftware_version  ------------  0x%08x (%d.%d.%d.%d)", ctb_infoext.software_version, (ctb_infoext.software_version >> 24) & 0xFF, (ctb_infoext.software_version >> 16) & 0xFF, (ctb_infoext.software_version >> 8) & 0xFF, ctb_infoext.software_version & 0xFF);
		msglen = strlen(msg);


		printf(msg);
		if (argc > 3)
		{
			HANDLE				wfile = NULL;
			char				wfname[1024];

			CharToOem(argv[3], wfname);
			wfile = CreateFile(argv[3], FILE_WRITE_DATA, FILE_SHARE_READ, NULL, CREATE_ALWAYS, 0, NULL);
			if (wfile == INVALID_HANDLE_VALUE)
			{
				CloseHandle(rfile);
				printf("\r\nCan't create file <%s>\r\n\r\n", wfname);
				return 8;
			}

			readed = 0;
			WriteFile(wfile, msg, msglen, &readed, NULL);
			if (readed != msglen)
			{
				CloseHandle(rfile);
				CloseHandle(wfile);
				printf("\r\nFile <%s> write error\r\n\r\n", wfname);
				return 9;
			}
			CloseHandle(wfile);
		}

		return 0;
	}


	if (_tcscmp(argv[2], _T("extract")) == 0)
	{
		if (argc < 5)
		{
			printf("\r\nNot enough arguments in command <extract>\r\n");
			return 10;
		}

		HANDLE				wfile;
		char				wfname[1024];
		FCTB_LAYERSINFO		l_info;
		uint32_t			l_readed = 0;
		DWORD				towrite = 0, writed = 0;

		uint32_t			layer_num = _tstoi(argv[3]);

		uint32_t			ckey = ctb_header.encryption_key * 0x2D83CDAC + 0xD8A83423;
		uint32_t			xkey = (layer_num * 0x1E1530CD + 0xEC3D47CD) * ckey;

		uint32_t			fpos = ctb_header.layersdef_offset + layer_num * sizeof(FCTB_LAYERSINFO);
		if (SetFilePointer(rfile, fpos, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
		{
			CloseHandle(rfile);
			printf("\r\nFile <%s> layer definition read error\r\n\r\n", ctbfname);
			return 11;
		}
		toread = sizeof(FCTB_LAYERSINFO);
		readed = 0;
		ReadFile(rfile, &l_info, toread, &readed, NULL);
		if (readed != toread)
		{
			CloseHandle(rfile);
			printf("\r\nFile <%s> layer definition read error\r\n\r\n", ctbfname);
			return 12;
		}

		fpos = l_info.data_offset;
		if (SetFilePointer(rfile, fpos, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
		{
			CloseHandle(rfile);
			printf("\r\nFile <%s> layer content read error\r\n\r\n", ctbfname);
			return 13;
		}


		CharToOem(argv[4], wfname);
		wfile = CreateFile(argv[4], FILE_WRITE_DATA, FILE_SHARE_READ, NULL, CREATE_ALWAYS, 0, NULL);
		if (wfile == INVALID_HANDLE_VALUE)
		{
			printf("\r\nCan't create file <%s>\r\n\r\n", wfname);
			return 14;
		}

		if (ctb_header.encryption_key != 0)
		{
			printf("\r\nLayers data is encrypted.");
		}
		printf("\r\nExtracting ");
		while (l_readed < l_info.data_length)
		{
			toread = sizeof(buffer);
			if (toread > (l_info.data_length - l_readed))
			{
				toread = l_info.data_length - l_readed;
			}
			towrite = toread;

			// read portion data of layer
			readed = 0;
			ReadFile(rfile, buffer, toread, &readed, NULL);
			if (readed != toread)
			{
				CloseHandle(rfile);
				CloseHandle(wfile);
				printf("\r\nFile <%s> layer data read error\r\n\r\n", ctbfname);
				return 15;
			}
			l_readed += toread;

			// decoding readed data
			if (ctb_header.encryption_key != 0)
			{
				bpos = 0;
				while (bpos < readed)
				{
					*(uint32_t*)(&buffer[bpos]) ^= xkey;
					xkey = xkey + ckey;
					bpos += 4;
				}
			}

			// writing decided data
			writed = 0;
			WriteFile(wfile, buffer, towrite, &writed, NULL);
			if (writed != towrite)
			{
				CloseHandle(rfile);
				CloseHandle(wfile);
				printf("\r\nFile <%s> write error\r\n\r\n", wfname);
				return 16;
			}
			printf(".");

		}
		printf("\r\nDone.\r\n\r\n");
		CloseHandle(wfile);
	}


	CloseHandle(rfile);
	return 0;
}
//==============================================================================






void		print_help()
{
	printf("\r\nUsage: ctb_util <.ctb filename> <command>");
	printf("\r\nCommands list:");
	printf("\r\n\textract <n> <filename>\t- extract to file <filename> layer with number <n>");
	printf("\r\n\tinfo [filename]\t\t- show file info, if [filename] specified, write info to file too");
	printf("\r\n\t");
	printf("\r\n");
}
//==============================================================================







