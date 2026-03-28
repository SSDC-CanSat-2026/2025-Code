#ifndef _ICM42688PSPI_H_
#define _ICM42688PSPI_H_

#include <stm32g491xx.h>

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define ACCEL_SENSITIVITY	2048.0 			// LSB/g
#define GYRO_SENSITIVITY	16.4			// LSB/(degree/sec)
#define ACCEL_CONVERT		9.80665			// From g's 		-> m/sec^2
#define GYRO_CONVERT		0.0174532925	// From degrees/sec -> rad/sec

typedef struct ICM42688P_AccelData
{
	float accel_x;
	float accel_y;
	float accel_z;

	// FIXME : Need to redefine what RPY is based on new mount position of sensor
    int16_t accel_pitch;
    int16_t accel_yaw;
    int16_t accel_roll;

    float gyro_x;
    float gyro_y;
    float gyro_z;
} ICM42688P_AccelData;

int16_t ICM42688P_read_reg(uint8_t reg);

ICM42688P_AccelData ICM42688P_read_data();

uint8_t ICM42688P_init(SPI_TypeDef *spi_handle, GPIO_TypeDef* chip_select_port, uint16_t chip_select_gpio_pin);

#endif
