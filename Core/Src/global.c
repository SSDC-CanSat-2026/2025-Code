/*
 * global.c
 *
 *  Created on: Feb 25, 2025
 *      Author: bbevel0133
 */

#include "global.h"

// Flags
// TODO: make this false before any demonstrations
volatile uint8_t telemetry_enable	= 1;
volatile uint8_t gps_time_enable	= 0;
volatile uint8_t is_calibrated		= 0;
volatile uint8_t mec_wire_enable	= 0;
volatile uint8_t simulation_enable	= 0;
volatile uint8_t drop_egg_enable	= 0;
volatile uint8_t simulation_pre 	= 0;
volatile double simulated_pressure 	= 0.0;

// Flight State FSM variables
const	float	launch_altitude_threshold	= 50.0;
const	float	release_height_percentage	= 0.80;
const	float	release_height_tolerance	= 0.10;
const	float	egg_drop_height				= 5.0;
volatile float	max_altitude				= 0.0;
volatile float 	altitude_history[3]			= {0.0,0.0,0.0};

Mission_Data global_mission_data = {0};

void init_mission_data(void)
{
	memset(&global_mission_data, 0, sizeof(global_mission_data));

	global_mission_data.TEAM_ID = 3174;//1075 for 2026;
	global_mission_data.MISSION_TIME = 0; // Time since start in milliseconds
	global_mission_data.PACKET_COUNT = 0;
	global_mission_data.MODE = 'F';
	strcpy(global_mission_data.STATE, "LAUNCH_PAD");
	global_mission_data.ALTITUDE = 0.0;
	global_mission_data.MAG_R = 0.0;
	global_mission_data.MAG_P = 0.0;
	global_mission_data.MAG_Y = 0.0;
	global_mission_data.AUTO_GYRO_ROTATION_RATE = 0;
	strcpy(global_mission_data.GPS_TIME, "XX:XX:XX");
	global_mission_data.GPS_ALTITUDE = 0.0;
	global_mission_data.GPS_LATITUDE = 0.0;
	global_mission_data.GPS_LONGITUDE = 0.0;
	global_mission_data.GPS_SATS = 0;
	strcpy(global_mission_data.CMD_ECHO, "CMD");
	global_mission_data.ALTITUDE_OFFSET = 0.0;
}
