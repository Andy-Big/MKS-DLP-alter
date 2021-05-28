#ifndef __INCLUDES_H
#define __INCLUDES_H



#define	FW_VERSION			(unsigned short)0x0008
#define FW_DATE				(char*)__DATE__


#define	SDIR_UPD					(char*)"alterupd"
#define SDIR_UI						(char*)"ui"
#define	SDIR_UI_480					(char*)"480"		// UI images for 480x320 resolution
#define	SDIR_UI_320					(char*)"320"		// UI images for 320x240 resolution
#define	SDIR_FILES					(char*)"files"


#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <limits.h>
#if !defined(__GNUC__)
#include <intrinsics.h>
#endif
#include <assert.h>


#include "stm32f4xx_hal.h"

#include "sys_timer.h"


#endif  // __INCLUDES_H
