#include "unicode_utils.h"



uint16_t	UTF8toANSI(uint8_t *utfchar)
{
	if (*utfchar < 128)
		return utfchar[0];
	
	uint32_t	t1, t2;
	
	// UTF-8 to ANSI
	t1 = (uint8_t)(utfchar[1] & 0x1F);
	t1 <<= 6;
	t2 = (utfchar[0] & 0x3F);
	t1 += t2;
	return (t1 - 848);
			
}




uint16_t	UnicodeToUTF8(uint16_t unicode_char)
{
	if (unicode_char < 128)
		return unicode_char;
	
	uint16_t	t1, t2;
	
	switch ((unicode_char & 0xFF00))
	{
		// UNICODE to ANSI
		case 0x0400:
			t1 = ((unicode_char & 0x3F) | 0x80);
			t2 = (((unicode_char << 2) & 0x1F00) | 0xC000);
			t1 = t1 | t2;
			return t1;
			
	}

	return unicode_char;
}





