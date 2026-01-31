#include "LC76G.h"
#include <string.h>
#include <math.h>

// Initialize global fields
uint8_t gps_dma_buffer[GPS_DMA_BUFFER_SIZE] = {0};
LC76G_gps_data gps_data;

void LC76G_init()
{
    // Disable all other types of NEMA sentences
    HAL_UART_Transmit(&huart5, LC76_DISABLE_GGL, strlen(LC76_DISABLE_GGL), TIMEOUT);
    HAL_UART_Receive(&huart5, NULL, 32, TIMEOUT);

    HAL_UART_Transmit(&huart5, LC76_DISABLE_GSA, strlen(LC76_DISABLE_GSA), TIMEOUT);
    HAL_UART_Receive(&huart5, NULL, 32, TIMEOUT);

    HAL_UART_Transmit(&huart5, LC76_DISABLE_GSV, strlen(LC76_DISABLE_GSV), TIMEOUT);
    HAL_UART_Receive(&huart5, NULL, 32, TIMEOUT);

    HAL_UART_Transmit(&huart5, LC76_DISABLE_RMC, strlen(LC76_DISABLE_RMC), TIMEOUT);
    HAL_UART_Receive(&huart5, NULL, 32, TIMEOUT);

    HAL_UART_Transmit(&huart5, LC76_DISABLE_VTG8, strlen(LC76_DISABLE_VTG8), TIMEOUT);
    HAL_UART_Receive(&huart5, NULL, 32, TIMEOUT);

    // Enable GGA sentences
    HAL_UART_Transmit(&huart5, LC76_ENABLE_GGA, strlen(LC76_ENABLE_GGA), TIMEOUT);
    HAL_UART_Receive(&huart5, NULL, 32, TIMEOUT);


    /* Old Tim DMA stuff, not sure if needed but will not use for now */
//    // Clear UART idle flag if needed
//    if (__HAL_UART_GET_FLAG(&huart5, UART_FLAG_IDLE)) {
//    __HAL_UART_CLEAR_IDLEFLAG(&huart5);
//    }
//
//    // Enable DMA reception
//    HAL_UARTEx_ReceiveToIdle_DMA(&huart5, gps_dma_buffer, GPS_DMA_BUFFER_SIZE);
}

LC76G_gps_data LC76G_read_data() {

	LC76G_gps_data data = {0,0,0,0,0,0,0};

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

	uint8_t rx_buffer[82] = { 0 };


	HAL_UART_Receive_IT(&huart5, rx_buffer, 82);

	char copy[7] = { 0 };
	strncpy(copy, rx_buffer+1, 5);

	if (rx_buffer[0] == '$' && !strcmp(copy, "GNGGA")) {
		char timeH[3] = {0};
		char timeM[3] = {0};
		char timeS[3] = {0};

		char latD[3] = {0};
		char latM[10] = {0};
		char lonD[3] = {0};
		char lonM[10] = {0};
		char num_sats[3] = {0};
		char alt[6] = {0};

		// Index *should* start at the <Quality> Section
		uint8_t index = 51;

		for (uint8_t i = 0; i < 2; i++) { // Hours
			timeH[i] = rx_buffer[i+7];
		}
		for (uint8_t i = 0; i < 2; i++) { // Minutes
			timeM[i] = rx_buffer[i+9];
		}
		for (uint8_t i = 0; i < 2; i++) { // Seconds
			timeS[i] = rx_buffer[i+11];
		}
		for (uint8_t i = 0; i < 2; i++) { // Latitude Degrees
			latD[i] = rx_buffer[i+18];
		}
		for (uint8_t i = 0; i < 9; i++) { // Latitude Minutes
			latM[i] = rx_buffer[i+20];
		}
		for (uint8_t i = 0; i < 2; i++) { // Longitude Degrees
			lonD[i] = rx_buffer[i+32];
		}
		for (uint8_t i = 0; i < 9; i++) { // Latitude Degrees
			lonM[i] = rx_buffer[i+34];
		}
		for (uint8_t i = 0; i < 2; i++) { // Number of Satellites
			num_sats[i] = rx_buffer[i+49];
		}

		// Various while loops are the only way I can think of getting
		//     to the necessary field since the field sizes aren't fixed.
		while (rx_buffer[index] != ',') { // Quality
			index++;
		}
		index++;
		uint8_t curr_count = index;
		while(rx_buffer[index] != ',') { // Altitude
			alt[index-curr_count] = rx_buffer[index];
			index++;
		}

		// Now to do the conversions to the final data types

		// Altitude
		double altitude = convert_to_double(alt);
		if (altitude != -1) {
			gps_data.altitude = altitude;
		}

		// Latitude
		double latitudeDegrees = convert_to_double(latD);
		double latitudeMinutes = convert_to_double(latM);
		if (latitudeDegrees != -1 && latitudeMinutes != -1) {
			double LAT = latitudeDegrees + (latitudeMinutes * convert);
			gps_data.lat = LAT;
		}

		// Longitude
		double longitudeDegrees = convert_to_double(lonD);
		double longitudeMinutes = convert_to_double(lonM);
		if (longitudeDegrees != -1 && longitudeMinutes != -1) {
			double LONG = longitudeDegrees + (longitudeMinutes * convert);
			gps_data.lon = LONG;
		}

		// Num Sats
		uint8_t sats_count = convert_to_integer(num_sats);
		if (sats_count != -1) {
			gps_data.num_sat_used = sats_count;
		}

		// Hours
		uint8_t hours = convert_to_integer(timeH);
		if (hours != -1) {
			gps_data.time_H = hours;
		}

		// Minutes
		uint8_t minutes = convert_to_integer(timeM);
		if (minutes != -1) {
			gps_data.time_H = minutes;
		}
		// Seconds
		uint8_t seconds = convert_to_integer(timeS);
		if (seconds != -1) {
			gps_data.time_H = seconds;
		}

		gps_data.sats[0] = num_sats[0];
		gps_data.sats[1] = num_sats[1];
		gps_data.sats[2] = num_sats[2];

	}

	return data;
}

void LC76G_get_array(UART_HandleTypeDef *huart,uint8_t *array[], uint16_t size) {
	HAL_UART_Receive(huart, array, size, 2000);
}

double convert_to_double(char string_double[]){
    char *endptr;
    double result = strtod(string_double, &endptr);

    if(result == 0) {
        return -1;
    }

    return result;
}

uint8_t convert_to_integer(char string_int[]) {
	char *endptr;
	double result = strtol(string_int, &endptr);

	if(result == 0) {
		return -1;
	}

	return result;
}

