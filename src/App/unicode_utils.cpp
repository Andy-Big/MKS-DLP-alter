#include "unicode_utils.h"



// Convert UTF8 char to ANSI char
// Return ANSI char
char	UTF8toANSI(char *utfchar)
{
	// Latin
	if (*utfchar < 128)
		return utfchar[0];
	
	uint32_t	t1 = 0, t2 = 32;

	t1 = ((uint16_t)utfchar[0] << 8) | utfchar[1];
	
	// UTF-8 to ANSI for cyrillic
	if (t1 > 0xD08F && t1 < 0xD0C0)
		t2 = t1 - 0xCFD0;
	if (t1 > 0xD17F && t1 < 0xD190)
		t2 = t1 - 0xD090;
	
	
	return t2;
/*
	t1 = (uint8_t)(utfchar[0] & 0x1F);
	t1 <<= 6;
	t2 = (utfchar[1] & 0x3F);
	t1 += t2;
	if (t1 < 849 || t1 > 1103)
		return 0;
	return (char)(t1 - 848);
*/
}
//==============================================================================




// Convert UTF8 char to UNICODE char
// Return UNICODE char
TCHAR		UTF8toUnicode(char *utfchar)
{
	// Latin
	if (*utfchar < 128)
		return utfchar[0];
	
	uint16_t	t1 = 0, t2 = 0;

	t1 = ((uint16_t)utfchar[0] << 8) | utfchar[1];
	
	// UTF-8 to UNICODE for cyrillic
	if (t1 > 0xD08F && t1 < 0xD0C0)
		t2 = t1 - 0xCC80;
	if (t1 > 0xD17F && t1 < 0xD190)
		t2 = t1 - 0xCD40;
	
	
	return t2;
}
//==============================================================================




// Convert UNICODE char to UTF8 char
// Return UTF8 char
uint16_t	UnicodeToUTF8(uint16_t unichar)
{
	if (unichar < 128)
		return unichar;
	
	uint16_t	t1, t2;
	
	switch ((unichar & 0xFF00))
	{
		// Cyrillic UNICODE
		case 0x0400:
			t1 = ((unichar & 0x3F) | 0x80);
			t2 = (((unichar << 2) & 0x1F00) | 0xC000);
			t1 = t1 | t2;
			return t1;
			
	}

	return 0;
}
//==============================================================================





// Convert UNICODE char to ANSI char
// Return ANSI char
char		UnicodeToANSI(uint16_t unichar)
{
	if (unichar < 127)
		return (char)unichar;
	
	char res = 0;
	uint8_t t = 0;
	switch ((unichar & 0xFF00))
	{
		// UNICODE Cyrillic
		case 0x0400:
			t = unichar & 0x00FF;
			if (t > 0x0F && t < 0x50)
				res = t + 176;
			break;
	}
	
	return res;
}
//==============================================================================




// Convert UTF8 string to ANSI string
// Return count of converted chars
// maxlen - max lenght of cstr buffer
uint16_t	UTF8ToANSI_Str(char *cstr, char *utfstr, uint16_t maxlen)
{
	if (cstr == NULL || utfstr == NULL || maxlen < 1)
		return 0;
	
	uint16_t clen = 0;
	while (utfstr && clen < maxlen-1)
	{
		cstr[clen] = UTF8toANSI(utfstr);
		if (*utfstr < 0x80)
		{
			utfstr++;
		}
		else
		{
			utfstr += 2;
		}
		clen++;
	}
	cstr[clen] = 0;
	
	
	
	return clen;
}
//==============================================================================




// Convert UTF8 string to Unicode string
// Return count of converted chars
// maxlen - max lenght of cstr buffer
uint16_t	UTF8ToUnicode_Str(uint16_t *unistr, char *utfstr, uint16_t maxlen)
{
	if (unistr == NULL || utfstr == NULL || maxlen < 1)
		return 0;
	
	uint16_t unilen = 0;
	while (utfstr && unilen < maxlen-1)
	{
		unistr[unilen] = UTF8toUnicode(utfstr);
		if (*utfstr < 0x80)
		{
			utfstr++;
		}
		else
		{
			utfstr += 2;
		}
		unilen++;
	}
	unistr[unilen] = 0;
	
	
	
	return unilen;
}
//==============================================================================




// Convert UNICODE string to UTF8 string
// Return count of converted chars
// maxlen - max lenght of utfstr buffer
uint16_t	UnicodeToUTF8_Str(char *utfstr, uint16_t *unistr, uint16_t maxlen)
{
	if (unistr == NULL || utfstr == NULL || maxlen < 1)
		return 0;
	
	uint16_t uinilen = 0, utflen = 0, utfchar = 0;
	while (unistr[uinilen] && utflen < maxlen-1)
	{
		utfchar = UnicodeToUTF8(unistr[uinilen]);
		if (utfchar < 0x80)
		{
			*utfstr = (char)utfchar;
			utfstr++;
			utflen++;
		}
		else
		{
			*(uint16_t*)utfstr = utfchar;
			utfstr += 2;
			utflen += 2;
		}
		uinilen++;
	}
	*utfstr = 0;
	
	return utflen;
}
//==============================================================================




// Convert UNICODE string to ANSI string
// Return count of converted chars
// maxlen - max lenght of cstr buffer
uint16_t	UnicodeToANSI_Str(char *cstr, uint16_t *unistr, uint16_t maxlen)
{
	if (unistr == NULL || cstr == NULL || maxlen < 1)
		return 0;
	
	uint16_t clen = 0;
	while (unistr[clen] && clen < maxlen-1)
	{
		cstr[clen] = UnicodeToANSI(unistr[clen]);
		clen++;
	}
	cstr[clen] = 0;
	
	return clen;
}
//==============================================================================




uint32_t	strlen_utf(char *text)
{
	uint32_t	len = 0;
	uint8_t	curr_char = (uint8_t)(*text);
	
	while (curr_char != 0)
	{
		if (curr_char < 0x80)
		{
			text++;
		}
		else
		{
			text += 2;
		}
		len++;
		curr_char = (uint8_t)(*text);
	}
	
	return len;
}
//==============================================================================




uint32_t	tstrlen(uint16_t *text)
{
	uint32_t	len = 0;
	uint16_t	curr_char = (uint16_t)(*text);
	
	while (curr_char != 0)
	{
		len++;
		text++;
		curr_char = (uint16_t)(*text);
	}
	
	return len;
}
//==============================================================================




char*	strcpy_utf(char *dst, char* src)
{
	char *cdst = dst;
	
	while (*src != 0)
	{
		if (*src < 0x80)
		{
			*cdst = *src;
			cdst++;
			src++;
		}
		else
		{
			*(uint16_t*)(cdst) = *(uint16_t*)(src);
			cdst += 2;
			src += 2;
		}
	}
	*cdst = 0;
	return dst;
}
//==============================================================================




TCHAR*	tstrcpy(TCHAR *dst, TCHAR *src)
{
	if (dst == NULL || src == NULL)
		return dst;
	
	TCHAR *cdst = dst, *csrc = src;
	while (*csrc)
	{
		*cdst = *csrc;
		cdst++;
		csrc++;
	}
	*cdst = 0;
	
	return dst;
}
//==============================================================================




TCHAR*	tstrcpy_utf(TCHAR *dst, char *src)
{
	if (dst == NULL || src == NULL)
		return dst;
	
	TCHAR *tdst = dst;
	char *csrc = src;
	while (*csrc)
	{
		if (*csrc < 0x80)
		{
			*tdst = *csrc;
			tdst++;
			csrc++;
		}
		else
		{
			*tdst = UTF8toUnicode((char*)csrc);
			tdst++;
			csrc += 2;
		}
	}
	*tdst = 0;
	
	return dst;
}
//==============================================================================




TCHAR*	tstrcat(TCHAR *dst, TCHAR *src)
{
	if (dst == NULL || src == NULL)
		return dst;
	
	TCHAR *cdst = dst, *csrc = src;
	while (*cdst)
		cdst++;
	while (*csrc)
	{
		*cdst = *csrc;
		cdst++;
		csrc++;
	}
	*cdst = 0;
	
	return dst;
}
//==============================================================================




TCHAR*	tstrcat_utf(TCHAR *dst, char *src)
{
	if (dst == NULL || src == NULL)
		return dst;
	
	TCHAR *cdst = dst;
	char *csrc = src;
	while (*cdst)
		cdst++;
	while (*csrc)
	{
		*cdst = UTF8toUnicode(csrc);
		cdst++;
		if (*csrc < 0x80)
			csrc++;
		else
			csrc += 2;
	}
	*cdst = 0;
	
	return dst;
}
//==============================================================================




