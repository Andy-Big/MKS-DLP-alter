#include "prtfiles.h"

#include "files_pws.h"



FILES_TYPE		pfile_type = FTYPE_NONE;
FIL				*prt_file;
uint8_t			inited = 0;



uint8_t		PFILE_Init(FIL *file, FILES_TYPE ftype)
{
	uint8_t		res = 0;
	inited = 0;
	pfile_type = ftype;
	prt_file = file;
	
	switch (pfile_type)
	{
		case FTYPE_PWS:
			res = FPWS_ReadFileInfo(prt_file);
			if (res == 0)
			{
				pfile_type = FTYPE_NONE;
				prt_file = NULL;
			}
			else
			{
				inited = 1;
			}
			break;
		
	}

	return res;
}
//==============================================================================




uint8_t		PFILE_IsInited()
{
	return inited;
}
//==============================================================================




uint8_t		PFILE_SetPointerToPreview()
{
	switch (pfile_type)
	{
		case FTYPE_PWS:
			return FPWS_SetPointerToPreview(prt_file);
		
	}

	return 0;
}
//==============================================================================




uint16_t	PFILE_GetPreviewWidth()
{
	switch (pfile_type)
	{
		case FTYPE_PWS:
			return FPWS_GetPreviewWidth();
		
	}

	return 0;
}
//==============================================================================




uint16_t	PFILE_GetPreviewHeight()
{
	switch (pfile_type)
	{
		case FTYPE_PWS:
			return FPWS_GetPreviewHeight();
		
	}

	return 0;
}
//==============================================================================




uint8_t		PFILE_GetPreviewBpp()
{
	switch (pfile_type)
	{
		case FTYPE_PWS:
			return 16;
		
	}

	return 0;
}
//==============================================================================




uint8_t		PFILE_DrawPreview(FIL *file, TG_RECT *rect)
{
	switch (pfile_type)
	{
		case FTYPE_PWS:
			return FPWS_DrawPreview(file, rect);
		
	}

	return 0;
}
//==============================================================================




