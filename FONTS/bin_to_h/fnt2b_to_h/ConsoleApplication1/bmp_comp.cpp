// fnt2b_to_h.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include "windows.h"


#define	ITEMTYPE	WORD


#define RGB24toRGB16(color) ((((color) >> 19) & 0x1f) << 11) | ((((color) >> 10) & 0x3f) << 5) | (((color) >> 3) & 0x1f)

#define LIMIT     1
#define NON_MATCH 2
#define EOF_FOUND 3

HANDLE	rfile, wfile;


void	encode (void);
int		process_comp(void);
int		get_word(void);
void	process_uncomp(WORD);
void	flush_outbuf(void);

// table used to look for sequences of repeating bytes
WORD	tmpbuf[4];  // we use subscripts 1 - 3 
WORD		ch;
DWORD		tmpbuf_cnt;

// output buffer for non-compressed output data
WORD	outbuf[128];
DWORD		outbuf_cnt;

BYTE		*imgbase;
BYTE		*endimgbase;

DWORD		bpp;




int _tmain(int argc, _TCHAR* argv[])
{

	SetLocaleInfo(0x0419, LOCALE_IDEFAULTCODEPAGE, _T("1251"));
	if (argc < 2)
	{
		printf("\r\nUsage: bmp2cimg <.bmp file mask>\n");
		return 10;
	}


	char	rcfname[256];	// bmp filename
	char	wcfname[256];	// cimg filename

	DWORD	fsize = 0;
	char	cst[64];
	DWORD	readed = 0;
	BOOL	fliprows = 0;

	CharToOem(argv[1], rcfname);
	CharToOem(argv[2], wcfname);

	WIN32_FIND_DATA finddata;
	HANDLE result = FindFirstFile(argv[1], &finddata);

	// -------------------- open and read bmp file --------------------------

	while (result != INVALID_HANDLE_VALUE)
	{
		CharToOem(finddata.cFileName, rcfname);

		printf("File <%s> ... ", rcfname);
		rfile = CreateFile(finddata.cFileName, FILE_READ_DATA, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
		if (rfile == INVALID_HANDLE_VALUE)
		{
			printf("Error open file %s\r\n", rcfname);
			goto getnextfile;
		}
		
		fsize = GetFileSize(rfile, NULL);
		BYTE *bmpdata = new BYTE[fsize];

		ReadFile(rfile, bmpdata, fsize, &readed, NULL);
		CloseHandle(rfile);

		// check image size
		// Image width
		DWORD bwidth = *(DWORD*)(bmpdata+18);
		// Image height
		DWORD bheight = *(DWORD*)(bmpdata+22);
		// Image bits per pixel
		bpp = *(WORD*)(bmpdata+28);

		if (bheight & 0x80000000)
		{
			// If standart row order
			bheight = 0xFFFFFFFF - bheight + 1;
		}
		else
		{
			// If flip row order
			fliprows = 1;
		}

		if (bwidth > 2560 || bheight > 2560)
		{
			printf("Image size is too big: %ux%u pix\r\n", bwidth, bheight);
			goto getnextfile;
		}
		WORD	wwidth = bwidth;
		WORD	wheight = bheight;
		if (fliprows == 0)
			wheight |= 0x8000;
		// Line lenght in bytes (16 bpp aligned of DWORD)
		DWORD	lsize = bwidth * 2;
		while (lsize % 4)
		{
			lsize++;
		}

		TCHAR	*cd;
		TCHAR	wfname[256];

		_tcscpy(wfname, finddata.cFileName);
		if((cd = _tcschr(wfname, '.')) > 0)
		{
			cd[0] = 0;
		}
		_tcscat(wfname, _T(".cimg"));
		CharToOem(wfname, wcfname);

		wfile = CreateFile(wfname, FILE_WRITE_DATA, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, 0, NULL);
		if (wfile == INVALID_HANDLE_VALUE)
		{
			printf("Error create file %s\r\n", wcfname);
			goto getnextfile;
		}

		WriteFile(wfile, &wwidth, 2, &readed, NULL);
		WriteFile(wfile, &wheight, 2, &readed, NULL);
		DWORD	inpos = *(DWORD *)(bmpdata + 10);
		imgbase = (BYTE*)(bmpdata + inpos);
		endimgbase = (BYTE*)(bmpdata + fsize);

		encode();

		CloseHandle(wfile);

		printf("Success!\r\n");

getnextfile:
		if(FindNextFile(result, &finddata) == FALSE)
			break;

	}
	return 0;
}






void encode ()
{
    int ret_code;

    tmpbuf_cnt = 0;  /* no. of char's in tmpbuf */
    outbuf_cnt = 0;  /* no. of char's in outbuf */
    while (1)
        {
        if (get_word() == EOF)  /* read next byte into ch */
           break;
        tmpbuf[++tmpbuf_cnt] = (ITEMTYPE) ch;
        if (tmpbuf_cnt == 3)
            {
            /* see if all 3 match each other */
            if ((tmpbuf[1] == tmpbuf[2])
             && (tmpbuf[2] == tmpbuf[3]))
               {
               /* they do - add compression */
               /* this will process all bytes in input file until
                  a non-match occurs, or 128 bytes are processed,
                  or we find eof */
               ret_code=process_comp();
               if (ret_code == EOF_FOUND)
                  break;        /* stop compressing */
               if (ret_code == NON_MATCH)
                  tmpbuf_cnt=1; /* save the char that didn't match */
               else
                  /* we just compressed the max. of 128 bytes */
                  tmpbuf_cnt=0;    /* start over for next chunk */
               }
            else
               {
               /* we know the first byte doesn't match 2 or more
                  others, so just send it out as uncompressed. */
               process_uncomp(tmpbuf[1]);

               /* see if the last 2 bytes in the buffer match */
               if (tmpbuf[2] == tmpbuf[3])
                  {
                  /* move byte 3 to position 1 and pretend we just
                     have 2 bytes -- note that the first byte was
                     already sent to output */
                  tmpbuf[1]=tmpbuf[3];
                  tmpbuf_cnt=2;
                  }
               else
                  {
                  /* send byte 2 and keep byte 3 - it may match the
                     next byte.  Move byte 3 to position 1 and set
                     count to 1.  Note that the first byte was
                     already sent to output */
                  process_uncomp(tmpbuf[2]);
                  tmpbuf[1]=tmpbuf[3];
                  tmpbuf_cnt=1;
                  }
               }
            }
        }  /* end while */
    flush_outbuf();
}

/*------------------------------------------------------------------*
 This flushes any non-compressed data not yet sent, then it processes
 repeating bytes until > 128, or EOF, or non-match.
      return values: LIMIT, EOF_FOUND, NON_MATCH
 Prior to ANY return, it writes out the 2 byte compressed code.
 If a NON_MATCH was found, this returns with the non-matching char
 residing in tmpbuf[0].
      Inputs: tmpbuf[0], input file
      Outputs: tmpbuf[0] (sometimes), output file, and return code
 *------------------------------------------------------------------*/
int process_comp()
{
     /* we start out with 3 repeating bytes */
     register int len = 3;
	 DWORD		wr;

     /* we're starting a repeating chunk - end the non-repeaters */
     flush_outbuf();

     while (get_word() != EOF)
        {
        if (ch != tmpbuf[1])
            {
            /* send no. of repeated bytes to be encoded */
			len--;
			len |= 0x80;
			WriteFile(wfile, &(len), 1, &wr, NULL);
            /* send the byte's value being repeated */
            WriteFile(wfile, &(tmpbuf[1]), sizeof(ITEMTYPE), &wr, NULL);
            /* save the non-matching character just read */
            tmpbuf[1]=(ITEMTYPE) ch;
            return NON_MATCH;
            }
        /* we know the new byte is part of the repeating seq */
        len++;
        if (len == 128)
            {
            /* send no. of repeated bytes to be encoded */
			len--;
			len |= 0x80;
			WriteFile(wfile, &(len), 1, &wr, NULL);
            /* send the byte's value being repeated */
            WriteFile(wfile, &(tmpbuf[1]), sizeof(ITEMTYPE), &wr, NULL);
            return LIMIT;
            }
        } /* end while */

        /* if flow comes here, we just read an EOF */
        /* send no. of repeated bytes to be encoded */
		len--;
		len |= 0x80;
		WriteFile(wfile, &(len), 1, &wr, NULL);
        /* send the byte's value being repeated */
        WriteFile(wfile, &(tmpbuf[1]), sizeof(ITEMTYPE), &wr, NULL);
        return EOF_FOUND;
}

/*----------------------------------------------------------------*
 This adds 1 non-repeating byte to outbuf.  If outbuf becomes full
 with 128 bytes, it flushes outbuf.
 There are no return codes and no bytes are read from the input.
 *----------------------------------------------------------------*/
void process_uncomp(ITEMTYPE char1)
{
    outbuf[outbuf_cnt++] = char1;
    if (outbuf_cnt == 128)
       flush_outbuf();
}

/*---------------------------------------------------*/
/* This flushes any non-compressed data not yet sent.*/
/* On exit, outbuf_cnt will equal zero.              */
/*---------------------------------------------------*/
void flush_outbuf()
{
    register int pos=0;
	 DWORD		wr;

    if(!outbuf_cnt)
       return;        /* nothing to do */

    /* send no. of unencoded bytes to be sent */
	BYTE t = outbuf_cnt - 1;
    WriteFile(wfile, &(t), 1, &wr, NULL);

    for ( ; outbuf_cnt; outbuf_cnt--)
		WriteFile(wfile, &(outbuf[pos++]), sizeof(ITEMTYPE), &wr, NULL);
}

/*---------------------------------------------------*/
/* This reads the next byte into ch.  It returns EOF */
/* at end-of-file, or on error.                      */
/*---------------------------------------------------*/
int get_word()
{
	DWORD		rd;
	if (bpp == 24)
	{
		DWORD dch = ((DWORD)*imgbase) + ((DWORD)*(imgbase + 1) << 8) + ((DWORD)*(imgbase + 2) << 16);
		ch = RGB24toRGB16(dch);
		imgbase += 3;
	}
	else
	{
		ch = *(WORD*)imgbase;
		imgbase += 2;
	}
	if (imgbase >= endimgbase)
	{
		/* there are either 0, 1, or 2 char's to write
		before we quit */
		if (tmpbuf_cnt == 1)
			process_uncomp(tmpbuf[1]);
		else
		{
			if (tmpbuf_cnt == 2)
			{
				process_uncomp(tmpbuf[1]);
				process_uncomp(tmpbuf[2]);
			}
		}
		return EOF;
	}
	return 0;
}

