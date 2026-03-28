#ifndef _LC76G_H_
#define _LC76G_H_

#include <stm32g491xx.h>
#include "stm32g4xx_hal.h"
#include "uart_interrupt.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAX_GGA_FIELDS 20
#define TIMEOUT 5

// DMA buffer aligned to 4-bytes

// Constant for converting Minutes to Degrees
static const double convert = 0.0166666667;

/* Define GPS commands */
// Checksums calculated using: https://nmeachecksum.eqth.net/

// PAIR messages
static const char LC76_ENABLE_GGA[] = "$PAIR062,0,1*3F\r\n";
static const char LC76_DISABLE_GGL[] = "$PAIR062,1,0*3F\r\n";
static const char LC76_DISABLE_GSA[] = "$PAIR062,2,0*3C\r\n";
static const char LC76_DISABLE_GSV[] = "$PAIR062,3,0*3D\r\n";
static const char LC76_DISABLE_RMC[] = "$PAIR062,4,0*3A\r\n";
static const char LC76_DISABLE_VTG8[] = "$PAIR062,5,0*3B\r\n";

// We need: time, lat, lon, alt, numberOfSats
// Time format: HH:MM:SS
// Degrees in decimal degrees
// Altitude in meters above sea level
typedef struct {
    char time_H[3];         // UTC Time
    char time_M[3];
    char time_S[3];

    double lat;
    double lon;

    double altitude;

    uint8_t num_sat_used;
    char sats[3];
}LC76G_gps_data;
extern LC76G_gps_data gps_data;

typedef struct
{
	uint32_t time_ms;   // milliseconds since midnight
	char gps_time[8];
    uint8_t fix_quality;
    uint8_t num_satellites;
    float hdop;
    float latitude;     // decimal degrees
    float longitude;    // decimal degrees
    float altitude;     // meters
}GGA_Data_t;
extern GGA_Data_t data;

/* Define functions */
// Set the constellation we will use (GNGGA)
void LC76G_init();
// Pass in the received message and parse fields
void LC76G_read_data(char rx_buffer[], int n, LC76G_gps_data* data);
int parse_gga(char *sentence, GGA_Data_t *out);

/* Helper functions */
//double convert_to_double(char string_double[]);
//uint8_t convert_to_integer(char string_int[]);
void time_to_string(uint32_t time_ms, char *out); // This is also used for MISSION_TIME

/* Private helpers */
static uint32_t parse_gps_str_time_ms(const char *s);
static float nmea_to_decimal(char *coord, char dir);
static float fast_atof(const char *s);

#endif /* _LC76G_H_ */
