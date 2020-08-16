// fnt2b_to_h.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include "windows.h"


typedef struct
{
	TCHAR symb[32];
	DWORD  endpos;
} RAWINFO_STRUCT;

int _tmain(int argc, _TCHAR* argv[])
{

	SetLocaleInfo(0x0419, LOCALE_IDEFAULTCODEPAGE, _T("1251"));
	if (argc < 3)
	{
		printf("\r\nUsage: fntraw2h <raw_file> <info_file>\n");
		return 10;
	}


	HANDLE rfile = NULL, ifile = NULL, wfile = NULL;
	char rcfname[256];	// raw filename
	char icfname[256];	// info filename
	char wcfname[256];	// info filename

	RAWINFO_STRUCT rawinfo[1024];
	DWORD fsize = 0;
	char  cst[64];
	DWORD readed = 0;

	CharToOem(argv[1], rcfname);
	CharToOem(argv[2], icfname);

	// ---------------------- read info file ------------------------------
	ifile = CreateFile(argv[2], FILE_READ_DATA, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (ifile == INVALID_HANDLE_VALUE)
	{
		printf("Error open file %s", icfname);
		return 1;
	}
	printf("Scanning %s file... ", icfname);
	fsize = GetFileSize(ifile, NULL);
	DWORD	ipos = 0;
	DWORD	icount = 0;
	DWORD	cpos = 0;
	DWORD	fpos = 0;
	while (1)
	{
		// read one line from file
		ReadFile(ifile, cst+cpos, 1, &readed, NULL);
		fpos++;
		// full line readed or end of file reached
		if (cst[cpos] == '\n' || fpos >= fsize)
		{
			if (fpos == fsize)
				cpos++;
			cst[cpos] = 0;
			cpos = 0;
			if (sscanf_s(cst, "%s\t%u", rawinfo[icount].symb, 31, &(rawinfo[icount].endpos)));
				icount++;
			if (fpos >= fsize)
				break;
			continue;

		}
		if (cpos < 63)
			cpos++;

	}
	CloseHandle(ifile);
	printf("Found %u items\r\n", icount);


	// -------------------- open and read raw file --------------------------

	rfile = CreateFile(argv[1], FILE_READ_DATA, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (rfile == INVALID_HANDLE_VALUE)
	{
		printf("Error open file %s", rcfname);
		return 1;
	}
		
	fsize = GetFileSize(rfile, NULL);

	// check file size
	float fsymheight = (float)fsize / (float)rawinfo[icount-1].endpos;
	DWORD symheight = fsize / rawinfo[icount-1].endpos;

	if (fsymheight != (float)symheight)
	{
		printf("The %s file has the wrong size\r\n", rcfname);
		CloseHandle(rfile);
		return 4;
	}

	unsigned char out[4096], *data, tout[65536], htout[65536];

	data = new unsigned char[fsize];
	ReadFile(rfile, data, fsize, &readed, NULL);
	if (readed != fsize)
	{
		printf("Can't read file %s", rcfname);
		CloseHandle(rfile);
		return 3;
	}
	CloseHandle(rfile);

	TCHAR rfname[64];
	char rdataname[64];

	_tcscpy(rfname, argv[1]);

	// remove extension from filename
	TCHAR *cd;
	if((cd = _tcschr(rfname, '.')) > 0)
	{
		cd[0] = 0;
	}
	// save for char table name, widthname ant structname
	CharToOem(rfname, rdataname);

	// create result .c file name
	DWORD i = _tcslen(rfname);
	_tcscpy(rfname+i, _T(".c"));
	CharToOem(rfname, wcfname);

	printf("Writing images table in %s...\r\n", wcfname);

	wfile = CreateFile(rfname, FILE_WRITE_DATA, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, 0, NULL);
	if (wfile == INVALID_HANDLE_VALUE)
	{
		printf("Error create file %s", wcfname);
		return 5;
	}

	unsigned char c = 0;
	DWORD symbeginpos = 0;
	DWORD symendpos = rawinfo[0].endpos+1;
	DWORD rawwidth = rawinfo[icount-1].endpos;
	DWORD fptr = 0, optr = 0, sh = 0, symline = 0;
	DWORD j = 0;


	// write header in result file
	j = sprintf((char*)tout, "#include \"includes.h\"\r\n", rdataname);
	WriteFile(wfile, tout, j, &readed, NULL);
	j = sprintf((char*)tout, "#include \"lcd_ui_fonts.h\"\r\n\r\n\r\n", rdataname);
	WriteFile(wfile, tout, j, &readed, NULL);
	j = sprintf((char*)tout, "const uint8_t %s_data[] = {\r\n", rdataname);
	WriteFile(wfile, tout, j, &readed, NULL);

	try
	{
		while (ipos < icount)
		{
			c = 0;
			j = 0;
			sh = 0;
			j += sprintf((char*)tout+j, "\t\t\t// \"%s\" width %u\r\n\t\t", rawinfo[ipos].symb, symendpos-symbeginpos);

			// symbol bits coding
			for (symline = 0; symline < symheight; symline++)
			{
			
				j += sprintf((char*)tout+j, "\t\// ");
			
				for(i = symbeginpos; i < symendpos; i++)
				{
					if (data[i] != 0)
					{
						c |= 1 << sh;
						j += sprintf((char*)tout+j, "0");
					}
					else
					{
						j += sprintf((char*)tout+j, ".");
					}
					sh ++;
					if (sh == 8)
					{
						sh = 0;
						out[optr++] = c;
						c = 0;
					}
				}
				j += sprintf((char*)tout+j, "\r\n\t\t");

				symbeginpos += rawwidth;
				symendpos += rawwidth;
			}
			if (sh > 0)
				out[optr++] = c;

			// ------------ write hex result to file ---------------------
			for (i = 0; i < optr; i++)
			{
				j += sprintf((char*)tout+j, "0x%02X,", out[i]);
				if (((i+1)%12) == 0)
					j += sprintf((char*)tout+j, "\r\n\t\t");
			}
			j += sprintf((char*)tout+j, "\r\n\r\n");

			WriteFile(wfile, tout, j, &readed, NULL);
			if (readed < j)
			{
				printf("Error write file %s", wcfname);
				CloseHandle(wfile);
				return 5;
			}

			j = 0;
			optr = 0;
			ipos++;
			symbeginpos = rawinfo[ipos-1].endpos+1;
			symendpos = rawinfo[ipos].endpos+1;
		}
	}
	catch(...)
	{
		printf("Unknown error!");
		CloseHandle(wfile);
		return 10;
	}


	j = 0;
	j += sprintf((char*)tout+j, "};\r\n", rawinfo[ipos].symb, symendpos-symbeginpos);
	WriteFile(wfile, tout, j, &readed, NULL);
	if (readed < j)
	{
		printf("Error write file %s", wcfname);
		CloseHandle(wfile);
		return 5;
	}

	printf("Writing size table in %s...\r\n", wcfname);

	// write width table in result file
	j = sprintf((char*)tout, "const uint16_t %s_width[] = {\r\n", rdataname);
	WriteFile(wfile, tout, j, &readed, NULL);
	ipos = 0;
	symbeginpos = 0;
	while (ipos < icount)
	{
		j = 0;
		j = sprintf((char*)tout, "\t\t%u, // \"%s\" \r\n", rawinfo[ipos].endpos+1-symbeginpos, rawinfo[ipos].symb);
		WriteFile(wfile, tout, j, &readed, NULL);
		symbeginpos = rawinfo[ipos].endpos+1;
		ipos++;
	}
	j = 0;
	j += sprintf((char*)tout+j, "};\r\n", rawinfo[ipos].symb, symendpos-symbeginpos);
	WriteFile(wfile, tout, j, &readed, NULL);

	// write font info struct
	j = sprintf((char*)tout, "\r\n\r\n\r\nLCDUI_FONT font_%s = {\r\n", rdataname);
	WriteFile(wfile, tout, j, &readed, NULL);
	j = sprintf((char*)tout, "\t\t(uint16_t*)%s_width,\r\n", rdataname);
	WriteFile(wfile, tout, j, &readed, NULL);
	j = sprintf((char*)tout, "\t\t(uint8_t*)%s_data,\r\n", rdataname);
	WriteFile(wfile, tout, j, &readed, NULL);
	j = sprintf((char*)tout, "\t\t%u,\r\n", symheight);
	WriteFile(wfile, tout, j, &readed, NULL);
	j = sprintf((char*)tout, "\t\tsizeof(%s_width)/sizeof(uint16_t)\r\n", rdataname);
	WriteFile(wfile, tout, j, &readed, NULL);
	j = sprintf((char*)tout, "\t\t};\r\n");
	WriteFile(wfile, tout, j, &readed, NULL);

	printf("Success!\r\n");



	CloseHandle(wfile);



/*
	while (1)
	{

// 1b
		if (opt_1b)
		{
			for(i = 0; i < fseekdata.nFileSizeLow; i++)
			{
				c |= data[fptr++] << sh;
				sh ++;
				if (sh == 8)
				{
					sh = 0;
					out[optr++] = c;
					c = 0;
				}
			}
			if (sh > 0)
				out[optr++] = c;
		}
		else
		{
			for(i = 0; i < fseekdata.nFileSizeLow; i++)
			{
				c |= data[fptr++] << sh;
				sh += 2;
				if (sh == 8)
				{
					sh = 0;
					out[optr++] = c;
					c = 0;
				}
			}
			if (sh > 0)
				out[optr++] = c;
		}
		if (wfile == NULL)
		{
			if (opt_merge)
			{
				_tcscpy(st, _T("fnt2b_result.h"));
			}
			else
			{
				_tcscpy(st, fseekdata.cFileName);
				i = _tcslen(st);
				_tcscpy(st+i, _T(".h"));
			}
			wfile = CreateFile(st, FILE_WRITE_DATA, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, 0, NULL);
			if (wfile == INVALID_HANDLE_VALUE)
			{
				printf("Error create file %s", cfname);
				CloseHandle(fseek);
				CloseHandle(file);
				return 4;
			}
		}

		j = 0;
		sh = 0;
		for (i = 0; i < optr; i++)
		{
			j += sprintf((char*)tout+j, "0x%02X,", out[i]);
			if (((i+1)%12) == 0)
				j += sprintf((char*)tout+j, "\r\n");
		}
		j += sprintf((char*)tout+j, "\r\n");
		sh += sprintf((char*)(htout+sh), "// %s\r\n", cfname);

		WriteFile(wfile, htout, sh, (LPDWORD)&readed, NULL);
		if (readed < sh)
		{
			printf("Error write file %s", cfname);
			CloseHandle(fseek);
			CloseHandle(file);
			CloseHandle(wfile);
			return 5;
		}
		WriteFile(wfile, tout, j, (LPDWORD)&readed, NULL);
		if (readed < j)
		{
			printf("Error write file %s", cfname);
			CloseHandle(fseek);
			CloseHandle(file);
			CloseHandle(wfile);
			return 6;
		}
		
		printf("File completed - %s\r\n", cfname);
		
		delete[] data;
		CloseHandle(file);
		if (!opt_merge)
		{
			CloseHandle(wfile);
			wfile = NULL;
		}

		if (FindNextFile(fseek, &fseekdata) == FALSE)
			break;
	}
	CloseHandle(fseek);

*/
	return 0;
}

