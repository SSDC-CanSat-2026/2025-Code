#include "LC76G.h"
#include <string.h>
#include <math.h>

// Initialize global fields
LC76G_gps_data gps_data;

void LC76G_init(void)
{
    HAL_UART_Transmit(&huart5, "$PAIR062,0,0*3E\r\n", 18, HAL_MAX_DELAY); // Disable GGA
    HAL_Delay(100); // Use delays since we don't care about the ACK messages

    HAL_UART_Transmit(&huart5, "$PAIR062,1,0*3F\r\n", 18, HAL_MAX_DELAY); // Disable GLL
    HAL_Delay(100);

    HAL_UART_Transmit(&huart5, "$PAIR062,2,0*3C\r\n", 18, HAL_MAX_DELAY); // Disable GSA
    HAL_Delay(100);

    HAL_UART_Transmit(&huart5, "$PAIR062,3,0*3D\r\n", 18, HAL_MAX_DELAY); // Disable GSV
    HAL_Delay(100);

    HAL_UART_Transmit(&huart5, "$PAIR062,4,0*3A\r\n", 18, HAL_MAX_DELAY); // Disable RMC
    HAL_Delay(100);

    HAL_UART_Transmit(&huart5, "$PAIR062,5,0*3B\r\n", 18, HAL_MAX_DELAY); // Disable VTG
    HAL_Delay(200);

    HAL_UART_Transmit(&huart5, "$PAIR062,0,1*3F\r\n", 18, HAL_MAX_DELAY); // Enable GGA
}

void LC76G_test(UART_HandleTypeDef* huart) {
	char test_msg[] = "$PAIR865,0,0*31\r\n";
//	char buf[256];
//	uint8_t byte;

	HAL_UART_Transmit(huart, &test_msg, sizeof(test_msg)-1, HAL_MAX_DELAY);
//	HAL_UART_Receive_IT(huart, &byte, 1);
//	HAL_UART_Transmit(&huart5, LC76_ENABLE_GGA, strlen(LC76_ENABLE_GGA), TIMEOUT);
//	HAL_UART_Receive(huart, &buf, 256, 0x8FF);
//	HAL_UART_Transmit(&huart3, &buf, 256, HAL_MAX_DELAY);
}

void LC76G_read_data(char rx_buffer[], int n, LC76G_gps_data* data) {
	// The init() sets the GGA message set to be the only one used.
	// The protocol specification can be found at this link
	// https://quectel.com/content/uploads/2024/02/Quectel_LC26GABLC76GLC86G_Series_GNSS_Protocol_Specification_V1.1.pdf

	// $<TalkerID>GGA,<UTC>,<Lat>,<N/S>,<Lon>,<E/W>,<Quality>,<NumSatUsed>,<HDOP>,<Alt>,M,<Sep>,M,<DiffAge>,<DiffStation>*<Checksum><CR><LF>
	// Example: $GNGGA,040143.000,3149.334166,N,11706.941670,E,2,36,0.48,61.496,M,-0.335,M,,*58 (DiffAge and DiffStation not supported)

	// TalkerID 			- 2 Characters
	// GGA 					- 3 Characters
	// <UTC> (hhmmss.sss) 	- 10 Characters (start at 7 character offset)
	// <Lat> (ddmm.mmmmmm) 	- 11 Characters (start at 18 character offset)
	// <N/S> 				- 1 Character
	// <Long> (ddmm.mmmmmm) - 11 Characters (start at 32 character offset)
	// <E/W> 				- 1 Character
	// <Quality> 			- Numeric, 1 Digit  (start at 47 character offset)
	// <NumSatUsed> 		- Numeric, 2 Digits (start at 49 character offset)
	// <HDOP> 				- Numeric
	// <Alt> 				- Numeric
	// 'M' 					- <Alt> unit
	// <Sep> 				- Numeric
	// 'M' 					- <Sep> unit
	// <DiffAge> and <DiffStation> are not supported
	// <Checksum> 			- Hexadecimal, starts with '*'
	// <CR><LF>				- 2 Characters

	data->time_H[0] = rx_buffer[7];
	data->time_H[1] = rx_buffer[8];

	data->time_M[0] = rx_buffer[9];
	data->time_M[1] = rx_buffer[10];

	data->time_S[0] = rx_buffer[11];
	data->time_S[1] = rx_buffer[12];

	char latD[2];
	char latM[9];
	for (uint8_t i = 0; i < 2; i++) { // Latitude Degrees
		latD[i] = rx_buffer[i+18];
	}
	for (uint8_t i = 0; i < 9; i++) { // Latitude Minutes
		latM[i] = rx_buffer[i+20];
	}

	char lonD[2];
	char lonM[9];
	for (uint8_t i = 0; i < 2; i++) { // Longitude Degrees
		lonD[i] = rx_buffer[i+32];
	}
	for (uint8_t i = 0; i < 9; i++) { // Latitude Degrees
		lonM[i] = rx_buffer[i+34];
	}

	char num_sats[2];
	for (uint8_t i = 0; i < 2; i++) { // Number of Satellites
		num_sats[i] = rx_buffer[i+49];
	}

//		// Various while loops are the only way I can think of getting
//		//     to the necessary field since the field sizes aren't fixed.
//		while (rx_buffer[index] != ',') { // Quality
//			index++;
//		}
//		index++;
//		uint8_t curr_count = index;
//		while(rx_buffer[index] != ',') { // Altitude
//			alt[index-curr_count] = rx_buffer[index];
//			index++;
//		}
//
//		// Now to do the conversions to the final data types
//
//		// Altitude
//		double altitude = convert_to_double(alt);
//		if (altitude != -1) {
//			gps_data.altitude = altitude;
//		}
//
	// Latitude
//	double latitudeDegrees = convert_to_double(latD);
//	double latitudeMinutes = convert_to_double(latM);
//	if (latitudeDegrees != -1 && latitudeMinutes != -1) {
//		double LAT = latitudeDegrees + (latitudeMinutes * convert);
//		data->lat = LAT;
//	} else {
//	data->lat = -1;
//	}
//
//		// Longitude
//		double longitudeDegrees = convert_to_double(lonD);
//		double longitudeMinutes = convert_to_double(lonM);
//		if (longitudeDegrees != -1 && longitudeMinutes != -1) {
//			double LONG = longitudeDegrees + (longitudeMinutes * convert);
//			gps_data.lon = LONG;
//		}
//
//		// Num Sats
//		uint8_t sats_count = convert_to_integer(num_sats);
//		if (sats_count != -1) {
//			gps_data.num_sat_used = sats_count;
//		}
//
//		// Hours
//
//		gps_data.sats[0] = num_sats[0];
//		gps_data.sats[1] = num_sats[1];
//		gps_data.sats[2] = num_sats[2];
//
//		data.time_H = 1;
}

int parse_gga(char *sentence, GGA_Data_t *out)
{
    if (strncmp(sentence, "$GNGGA", 6) != 0)
        return 0;

    char *fields[MAX_GGA_FIELDS] = {0};
    int field_count = 0;

    char *p = sentence;
    fields[field_count++] = p;

    while (*p && field_count < MAX_GGA_FIELDS)
    {
        if (*p == ',' || *p == '*')
        {
            *p = '\0';
            fields[field_count++] = p + 1;
        }
        p++;
    }

    if (field_count < 10)
        return 0;

    out->fix_quality = (fields[6][0]) ? atoi(fields[6]) : 0;
    out->num_satellites  = (fields[7][0]) ? atoi(fields[7]) : 0;
    out->hdop        = (fields[8][0]) ? atof(fields[8]) : 0.0f;
    out->altitude    = (fields[9][0]) ? atof(fields[9]) : 0.0f;

    out->latitude = nmea_to_decimal(fields[2], fields[3][0]);
    out->longitude = nmea_to_decimal(fields[4], fields[5][0]);

    return 1;
}

//double convert_to_double(char string_double[]){
//    char *endptr;
//    double result = strtod(string_double, &endptr);
//
//    if(result == 0) {
//        return -1;
//    }
//
//    return result;
//}
//
//uint8_t convert_to_integer(char string_int[]) {
//	char *endptr;
//	double result = strtol(string_int, &endptr);
//
//	if(result == 0) {
//		return -1;
//	}
//
//	return result;
//}

static float nmea_to_decimal(char *coord, char dir)
{
    if (coord == NULL || coord[0] == '\0')
        return 0.0f;

    float raw = atof(coord);

    int degrees = (int)(raw / 100);
    float minutes = raw - (degrees * 100);

    float decimal = degrees + (minutes / 60.0f);

    if (dir == 'S' || dir == 'W')
        decimal *= -1.0f;

    return decimal;
}
