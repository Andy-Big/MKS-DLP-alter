
#include "config.h"
#include "eeprom.h"





MOTOR_CONFIG			cfgzMotor;
SYSTEM_INFO				systemInfo;
GLOBAL_CONFIG			cfgConfig;
WORK_TIME				cfgTimers;









void			CFG_Init()
{
	memset(&systemInfo, 0, sizeof(SYSTEM_INFO));
	memset(&cfgzMotor, 0, sizeof(MOTOR_CONFIG));
	memset(&cfgConfig, 0, sizeof(GLOBAL_CONFIG));
	memset(&cfgTimers, 0, sizeof(WORK_TIME));

	cfgzMotor.invert_z_dir = 1;
	cfgzMotor.z_home_dir = -1;
	cfgzMotor.z_home_pos = 0;
	cfgzMotor.z_zero_pos = cfgzMotor.z_home_pos;
	cfgzMotor.z_min_pos = -3;
	cfgzMotor.z_max_pos = 180;
	cfgzMotor.z_min_endstop_inverting = 1;
	cfgzMotor.z_max_endstop_inverting = 1;
	cfgzMotor.z_pause_pos = 5.0;
	
	cfgzMotor.axis_steps_per_mm = 1600;
	cfgzMotor.max_feedrate_mm_s = 100;
	cfgzMotor.max_acceleration_mm_per_s2 = 100;

	cfgzMotor.homing_feedrate_z = 1;
	cfgzMotor.acceleration = 0.7;
	cfgzMotor.feedrate = 5;
	cfgzMotor.travel_acceleration = 40;
	cfgzMotor.travel_feedrate = 30;
	cfgzMotor.min_feedrate_mm_s = 0.01;
	cfgzMotor.min_travel_feedrate_mm_s = 0.01;
	cfgzMotor.max_jerk = 0.05;
	cfgzMotor.current_vref = 800;
	cfgzMotor.current_hold_vref = 300;
	cfgzMotor.hold_time = 10;
}
//==============================================================================




