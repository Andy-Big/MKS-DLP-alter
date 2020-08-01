
#include "config.h"





CFGZ_STRUCT			cfgzMotor;









void			CFG_Init()
{
	cfgzMotor.invert_z_dir = 1;
	cfgzMotor.z_home_dir = -1;
	cfgzMotor.z_min_pos = 0;
	cfgzMotor.z_max_pos = 180;
	cfgzMotor.min_software_endstops = 0;
	cfgzMotor.max_software_endstops = 1;
	cfgzMotor.homing_feedrate_z = 600;
	cfgzMotor.z_min_endstop_inverting = 1;
	cfgzMotor.z_max_endstop_inverting = 1;
	cfgzMotor.z_pause_pos = 5.0;
	
	cfgzMotor.axis_steps_per_mm = 1600;
	cfgzMotor.max_feedrate_mm_s = 5;
	cfgzMotor.max_acceleration_mm_per_s2 = 100;
	cfgzMotor.acceleration = 100;
	cfgzMotor.retract_acceleration = 100;
	cfgzMotor.travel_acceleration = 100;
	cfgzMotor.min_feedrate_mm_s = 0.01;
	cfgzMotor.min_travel_feedrate_mm_s = 0.05;
	cfgzMotor.max_jerk = 0.4;
	cfgzMotor.current_vref = 600;
}
//==============================================================================




