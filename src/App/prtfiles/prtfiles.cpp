#include "prtfiles.h"
#include "math.h"

#include "files_pws.h"
#include "files_photon.h"
#include "config.h"



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
		
		case FTYPE_PHOTON:
		case FTYPE_CBDDLP:
			res = FPHOTON_ReadFileInfo(prt_file);
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

	if (pfile_type != FTYPE_NONE)
	{
		if (PFILE_GetAntialiasing() > 1)
			systemInfo.print_use_aa_grade = 1;
		else
			systemInfo.print_use_aa_grade = 0;

		if (PFILE_GetIndLayerSettings() > 0)
			systemInfo.print_use_ind_params = 1;
		else
			systemInfo.print_use_ind_params = 0;
	}
	
	return res;
}
//==============================================================================




uint8_t		PFILE_IsInited()
{
	return inited;
}
//==============================================================================




uint8_t		PFILE_SetPointerToPreview(uint8_t small)
{
	switch (pfile_type)
	{
		case FTYPE_PWS:
			return FPWS_SetPointerToPreview(prt_file);

		case FTYPE_PHOTON:
		case FTYPE_CBDDLP:
			return FPHOTON_SetPointerToPreview(prt_file, small);

	}

	return 0;
}
//==============================================================================




uint16_t	PFILE_GetPreviewWidth(uint8_t small)
{
	switch (pfile_type)
	{
		case FTYPE_PWS:
			return FPWS_GetPreviewWidth();

		case FTYPE_PHOTON:
		case FTYPE_CBDDLP:
			return FPHOTON_GetPreviewWidth(small);

	}

	return 0;
}
//==============================================================================




uint16_t	PFILE_GetPreviewHeight(uint8_t small)
{
	switch (pfile_type)
	{
		case FTYPE_PWS:
			return FPWS_GetPreviewHeight();

		case FTYPE_PHOTON:
		case FTYPE_CBDDLP:
			return FPHOTON_GetPreviewHeight(small);

	}

	return 0;
}
//==============================================================================




uint8_t		PFILE_GetPreviewBpp()
{
	switch (pfile_type)
	{
		case FTYPE_PWS:
		case FTYPE_PHOTON:
		case FTYPE_CBDDLP:
			return 16;
		
	}

	return 0;
}
//==============================================================================




uint8_t		PFILE_DrawPreview(FIL *file, TG_RECT *rect, uint8_t small)
{
	switch (pfile_type)
	{
		case FTYPE_PWS:
			return FPWS_DrawPreview(file, rect);

		case FTYPE_PHOTON:
		case FTYPE_CBDDLP:
			return FPHOTON_DrawPreview(file, rect, small);

	}

	return 0;
}
//==============================================================================




uint32_t	PFILE_GetPrintTime()
{
	if (pfile_type == FTYPE_NONE)
		return 0;
	
	uint32_t	total_layers = PFILE_GetTotalLayers();
	float		set_liftspeed = PFILE_GetLiftSpeed();
	float		set_dropspeed = PFILE_GetDropSpeed();
	float		set_accel = cfgzMotor.acceleration;
	float		set_height = PFILE_GetLiftHeight();
	float		time1 = 0, up_time = 0, down_time = 0, total_time = 0;
	
	// TODO - change calculations for difference accelerations/decelerations in bottom and top points
	
	// distance of up accelerate/decelerate to set speed
	float	dist_acc = (set_liftspeed * set_liftspeed) / (set_accel * 2);
	// if accelerate distance is more then half of lift height
	if (dist_acc > (set_height / 2))
	{
		// up move time
		up_time = ((1.414 * sqrt(set_height)) / sqrt(set_accel)) * 2;
	}
	else
	{
		// up accelerate and decelerate time
		time1 = (set_liftspeed / set_accel) * 2;
		// up move time
		up_time = (set_height - (dist_acc * 2)) / set_liftspeed;
		// up total time
		up_time += time1;
	}
	// distance of down accelerate/decelerate to set speed
	dist_acc = (set_dropspeed * set_dropspeed) / (set_accel * 2);
	// if accelerate distance is more then half of lift height
	if (dist_acc > (set_height / 2))
	{
		// down move time
		down_time = ((1.414 * sqrt(set_height / 2)) / sqrt(set_accel)) * 2;
	}
	else
	{
		// down accelerate and decelerate time
		time1 = (set_dropspeed / set_accel) * 2;
		// down move time
		down_time = (set_height - (dist_acc * 2)) / set_dropspeed;
		// down total time
		down_time += time1;
	}
	// total move time for one layer
	total_time = up_time + down_time;
	// total move time for all layers
	total_time *= total_layers;
	
	// if antialiasing - add time for reading sublayers, about 0.8 sec per sublayer in debug mode and about 0.53 sec in release with max optimization
	if (PFILE_GetAntialiasing() > 1)
#ifdef __DEBUG__
		total_time += total_layers * (PFILE_GetAntialiasing() - 1) * 0.666;
#else
		total_time += total_layers * (PFILE_GetAntialiasing() - 1) * 0.362;
#endif	
	// correction
//	total_time *= 0.937;
	
	// add bottom light time
	total_time += PFILE_GetBottomLayers() * PFILE_GetLightBottom();
	// add common light time
	total_time += (total_layers - PFILE_GetBottomLayers()) * PFILE_GetLightLayer();
	// add pause light time
	total_time += total_layers * PFILE_GetLightPause();

	return (uint32_t)total_time;
}
//==============================================================================




uint32_t	PFILE_GetTotalLayers()
{
	switch (pfile_type)
	{
		case FTYPE_PWS:
			return FPWS_GetTotalLayers();

		case FTYPE_PHOTON:
		case FTYPE_CBDDLP:
			return FPHOTON_GetTotalLayers();

	}

	return 0;
}
//==============================================================================




uint32_t	PFILE_GetBottomLayers()
{
	switch (pfile_type)
	{
		case FTYPE_PWS:
			return FPWS_GetBottomLayers();

		case FTYPE_PHOTON:
		case FTYPE_CBDDLP:
			return FPHOTON_GetBottomLayers();

	}

	return 0;
}
//==============================================================================




float		PFILE_GetLayerThickness()
{
	switch (pfile_type)
	{
		case FTYPE_PWS:
			return FPWS_GetLayerThickness();

		case FTYPE_PHOTON:
		case FTYPE_CBDDLP:
			return FPHOTON_GetLayerThickness();

	}

	return 0;
}
//==============================================================================




uint32_t	PFILE_GetAntialiasing()
{
	switch (pfile_type)
	{
		case FTYPE_PWS:
			return FPWS_GetAntialiasing();

		case FTYPE_PHOTON:
		case FTYPE_CBDDLP:
			return FPHOTON_GetAntialiasing();

	}

	return 0;
}
//==============================================================================




float		PFILE_GetLightLayer()
{
	switch (pfile_type)
	{
		case FTYPE_PWS:
			return FPWS_GetLightLayer();

		case FTYPE_PHOTON:
		case FTYPE_CBDDLP:
			return FPHOTON_GetLightLayer();

	}

	return 0;
}
//==============================================================================




void		PFILE_SetLightLayer(float val)
{
	switch (pfile_type)
	{
		case FTYPE_PWS:
			return FPWS_SetLightLayer(val);

		case FTYPE_PHOTON:
		case FTYPE_CBDDLP:
			return FPHOTON_SetLightLayer(val);

	}

}
//==============================================================================




float		PFILE_GetLightBottom()
{
	switch (pfile_type)
	{
		case FTYPE_PWS:
			return FPWS_GetLightBottom();

		case FTYPE_PHOTON:
		case FTYPE_CBDDLP:
			return FPHOTON_GetLightBottom();

	}

	return 0;
}
//==============================================================================




void		PFILE_SetLightBottom(float val)
{
	switch (pfile_type)
	{
		case FTYPE_PWS:
			return FPWS_SetLightBottom(val);

		case FTYPE_PHOTON:
		case FTYPE_CBDDLP:
			return FPHOTON_SetLightBottom(val);

	}

}
//==============================================================================




float		PFILE_GetLightPause()
{
	switch (pfile_type)
	{
		case FTYPE_PWS:
			return FPWS_GetLightPause();

		case FTYPE_PHOTON:
		case FTYPE_CBDDLP:
			return FPHOTON_GetLightPause();

	}

	return 0;
}
//==============================================================================




void		PFILE_SetLightPause(float val)
{
	switch (pfile_type)
	{
		case FTYPE_PWS:
			return FPWS_SetLightPause(val);

		case FTYPE_PHOTON:
		case FTYPE_CBDDLP:
			return FPHOTON_SetLightPause(val);

	}

}
//==============================================================================




float		PFILE_GetLiftHeight()
{
	switch (pfile_type)
	{
		case FTYPE_PWS:
			return FPWS_GetLiftHeight();

		case FTYPE_PHOTON:
		case FTYPE_CBDDLP:
			return FPHOTON_GetLiftHeight();

	}

	return 0;
}
//==============================================================================




void		PFILE_SetLiftHeight(float val)
{
	switch (pfile_type)
	{
		case FTYPE_PWS:
			return FPWS_SetLiftHeight(val);

		case FTYPE_PHOTON:
		case FTYPE_CBDDLP:
			return FPHOTON_SetLiftHeight(val);

	}

}
//==============================================================================




float		PFILE_GetLiftBottom()
{
	switch (pfile_type)
	{
		case FTYPE_PWS:
			return FPWS_GetLiftHeight();

		case FTYPE_PHOTON:
		case FTYPE_CBDDLP:
			return FPHOTON_GetLiftHeight();

	}

	return 0;
}
//==============================================================================




float		PFILE_GetLiftSpeed()
{
	switch (pfile_type)
	{
		case FTYPE_PWS:
			return FPWS_GetLiftSpeed();

		case FTYPE_PHOTON:
		case FTYPE_CBDDLP:
			return FPHOTON_GetLiftSpeed();

	}

	return 0;
}
//==============================================================================




void		PFILE_SetLiftSpeed(float val)
{
	switch (pfile_type)
	{
		case FTYPE_PWS:
			return FPWS_SetLiftSpeed(val);

		case FTYPE_PHOTON:
		case FTYPE_CBDDLP:
			return FPHOTON_SetLiftSpeed(val);

	}

}
//==============================================================================




float		PFILE_GetLiftSpeedBottom()
{
	switch (pfile_type)
	{
		case FTYPE_PWS:
			return FPWS_GetLiftSpeed();

		case FTYPE_PHOTON:
		case FTYPE_CBDDLP:
			return FPHOTON_GetLiftSpeed();

	}

	return 0;
}
//==============================================================================




float		PFILE_GetDropSpeed()
{
	switch (pfile_type)
	{
		case FTYPE_PWS:
			return FPWS_GetDropSpeed();

		case FTYPE_PHOTON:
		case FTYPE_CBDDLP:
			return FPHOTON_GetDropSpeed();

	}

	return 0;
}
//==============================================================================




void		PFILE_SetDropSpeed(float val)
{
	switch (pfile_type)
	{
		case FTYPE_PWS:
			return FPWS_SetDropSpeed(val);

		case FTYPE_PHOTON:
		case FTYPE_CBDDLP:
			return FPHOTON_SetDropSpeed(val);

	}

}
//==============================================================================




float		PFILE_GetResinVolume()
{
	switch (pfile_type)
	{
		case FTYPE_PWS:
			return FPWS_GetResinVolume();

		case FTYPE_PHOTON:
		case FTYPE_CBDDLP:
			return FPHOTON_GetResinVolume();

	}

	return 0;
}
//==============================================================================




uint32_t	PFILE_GetIndLayerSettings()
{
	switch (pfile_type)
	{
		case FTYPE_PWS:
			return FPWS_GetIndLayerSettings();

		case FTYPE_PHOTON:
		case FTYPE_CBDDLP:
			return FPHOTON_GetIndLayerSettings();

	}

	return 0;
}
//==============================================================================




uint32_t	PFILE_GetResolutionX()
{
	switch (pfile_type)
	{
		case FTYPE_PWS:
			return FPWS_GetResolutionX();

		case FTYPE_PHOTON:
		case FTYPE_CBDDLP:
			return FPHOTON_GetResolutionX();

	}

	return 0;
}
//==============================================================================




uint32_t	PFILE_GetResolutionY()
{
	switch (pfile_type)
	{
		case FTYPE_PWS:
			return FPWS_GetResolutionY();

		case FTYPE_PHOTON:
		case FTYPE_CBDDLP:
			return FPHOTON_GetResolutionY();

	}

	return 0;
}
//==============================================================================




