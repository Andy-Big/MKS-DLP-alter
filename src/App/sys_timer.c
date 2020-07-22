#include "sys_timer.h"

#include "touch_spi.h"



// Redefinition HAL 1ms timer
void		HAL_IncTick(void)
{
	uwTick += uwTickFreq;
	
	// Read Touch each 16 ms
	if ((uwTick & 0x0000000F) == 0)
		Touch_ReadCoords();
}

//==============================================================================




