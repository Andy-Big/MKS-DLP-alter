#include "unicode_utils.h"



// Convert UTF8 char to ANSI char
// Return ANSI char
char	UTF8toANSI(uint8_t *utfchar)
{
	if (*utfchar < 128)
		return utfchar[0];
	
	uint32_t	t1, t2;
	
	// UTF-8 to ANSI
	t1 = (uint8_t)(utfchar[1] & 0x1F);
	t1 <<= 6;
	t2 = (utfchar[0] & 0x3F);
	t1 += t2;
	if (t1 < 849 || t1 > 1103)
		return 0;
	return (char)(t1 - 848);
			
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
uint16_t	UTF8ToANSI_Str(char *cstr, uint8_t *utfstr, uint16_t maxlen)
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




char*	strcpy_utf(char *dst, char* src)
{
	uint32_t len = 0;
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
		len++;
	}
	*cdst = 0;
	return dst;
}
//==============================================================================




uint16_t*	strcpy_uni(uint16_t *dst, uint16_t *src)
{
	if (dst == NULL || src == NULL)
		return dst;
	
	uint16_t *cdst = dst, *csrc = src;
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




uint16_t*	strcat_uni(uint16_t *dst, uint16_t *src)
{
	if (dst == NULL || src == NULL)
		return dst;
	
	uint16_t *cdst = dst, *csrc = src;
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




