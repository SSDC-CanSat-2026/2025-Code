#include "ICM42688PSPI.h"
#include "stm32g4xx_hal.h"
#include <stdint.h>
#include "AccelerationCalculations.c"

static SPI_HandleTypeDef *hspi;

/* Private GPIO CS Pin Variables */
static GPIO_TypeDef *ChipSelect_GPIO_Port;
static uint16_t ChipSelect_Pin;

volatile static float gyro_old_x = 0;
volatile static float gyro_old_y = 0;
volatile static float gyro_old_z = 0;
volatile static uint32_t old_time = 0;

static void ICM42688P_disable_chip_select()
{
    HAL_GPIO_WritePin(ChipSelect_GPIO_Port, ChipSelect_Pin, GPIO_PIN_RESET);
}

static void ICM42688P_enable_chip_select()
{
    HAL_GPIO_WritePin(ChipSelect_GPIO_Port, ChipSelect_Pin, GPIO_PIN_SET);
}

static HAL_StatusTypeDef ICM42688P_write_reg(uint8_t reg, uint8_t data)
{
    uint8_t tx[2] = {reg, data};
    ICM42688P_disable_chip_select();
    HAL_SPI_Transmit(hspi, tx, 2, HAL_MAX_DELAY);
    ICM42688P_enable_chip_select();
    return HAL_OK;
}


int16_t ICM42688P_read_reg(uint8_t reg)
{
    uint8_t tx[3] = { reg | 0x80, 0x00, 0x00 }; // 0x80 = read bit
    uint8_t rx[3] = {0};
    ICM42688P_disable_chip_select();
    HAL_SPI_TransmitReceive(hspi, &tx, &rx, 3, HAL_MAX_DELAY);
    ICM42688P_enable_chip_select();

    // Read from the IMU will read 2 registers at a time.
    // The data returned from the IMU is in 2's complement,
    //   this means we have to handle the indiviudal bytes
    //   as unsigned integers and only cast to a signed
    //   integer when returning
    return (int16_t)((rx[1] << 8) | rx[0]);
}

uint8_t ICM42688P_init(SPI_TypeDef *spi_handle, GPIO_TypeDef *chip_select_port, uint16_t chip_select_gpio_pin)
{
    hspi = spi_handle;
    ChipSelect_GPIO_Port = chip_select_port;
    ChipSelect_Pin = chip_select_gpio_pin;

    /* // ???? WHAT??????
    HAL_Delay(100);
    ICM42688P_write_reg(0x4F, 0x04);  // Reset device
    HAL_Delay(100);
    ICM42688P_write_reg(0x11, 0x00);  // Power management
    ICM42688P_write_reg(0x10, 0x0F);  // Gyro and accel config
    */

    ICM42688P_write_reg(0x11, 0x01); // Reset Device
    
    HAL_Delay(100);

    ICM42688P_write_reg(0x50, (0b00000110)); // Set FSR (Full Scale Range) to +/- 16g

    ICM42688P_write_reg(0x4E, (0b11 << 2) | (0b11 << 0)); // Enable gyro & accelerometer

    HAL_Delay(1); // PWR_MGMT0 register requires a small delay when changing the gyro or accelerometer from off to any mode.

    ICM42688P_write_reg(0x7B, (0b10 << 1));               // Enable CLKIN

    return 0;
}

//int16_t Get_Accel_P(int16_t gyro_p, uint32_t time)
//{
//    return (gyro_old_p - gyro_p); // add timer later / (old_time - time);
//}
//
//int16_t Get_Accel_Y(int16_t gyro_y, uint32_t time)
//{
//    return (gyro_old_y - gyro_y); // add timer later / (old_time - time);
//}
//
//int16_t Get_Accel_R(int16_t gyro_r, uint32_t time)
//{
//    return (gyro_old_r - gyro_r); // add timer later / (old_time - time);
//}

ICM42688P_AccelData ICM42688P_read_data()
{
	ICM42688P_AccelData data = {0};

    data.gyro_x = ((float)ICM42688P_read_reg(0x25) / GYRO_SENSITIVITY) * GYRO_CONVERT;
    data.gyro_y = ((float)ICM42688P_read_reg(0x27) / GYRO_SENSITIVITY) * GYRO_CONVERT;
    data.gyro_z = ((float)ICM42688P_read_reg(0x29) / GYRO_SENSITIVITY) * GYRO_CONVERT;

    uint32_t time = 0;

    data.accel_x = ((float)ICM42688P_read_reg(0x1F) / ACCEL_SENSITIVITY) * ACCEL_CONVERT;
    data.accel_y = ((float)ICM42688P_read_reg(0x21) / ACCEL_SENSITIVITY) * ACCEL_CONVERT;
    data.accel_z = ((float)ICM42688P_read_reg(0x23) / ACCEL_SENSITIVITY) * ACCEL_CONVERT;

    gyro_old_x = data.gyro_x;
    gyro_old_y = data.gyro_y;
    gyro_old_z = data.gyro_z;
    old_time = time;

    return data;
}
