/**
 * @file board_I2C.c
 * @brief Board I2C implementation
 */

#include "board.h"
#include "stm32f4xx_hal.h"

extern I2C_HandleTypeDef hi2c1;

/**
 * @brief Initialize I2C for SE050 communication
 */
void board_i2c_init(void)
{
    /* I2C1 is already initialized in main.c */
    /* This function can be used for any additional I2C configuration */
}

/**
 * @brief Write data to SE050 over I2C
 * @param addr I2C address
 * @param data Data to write
 * @param len Length of data
 * @retval 0 if successful, non-zero otherwise
 */
int board_i2c_write(uint8_t addr, uint8_t *data, size_t len)
{
    HAL_StatusTypeDef ret;
    
    ret = HAL_I2C_Master_Transmit(&hi2c1, addr << 1, data, len, 1000);
    if (ret != HAL_OK) {
        return -1;
    }
    
    return 0;
}

/**
 * @brief Read data from SE050 over I2C
 * @param addr I2C address
 * @param data Buffer to store read data
 * @param len Length of data to read
 * @retval 0 if successful, non-zero otherwise
 */
int board_i2c_read(uint8_t addr, uint8_t *data, size_t len)
{
    HAL_StatusTypeDef ret;
    
    ret = HAL_I2C_Master_Receive(&hi2c1, addr << 1, data, len, 1000);
    if (ret != HAL_OK) {
        return -1;
    }
    
    return 0;
}