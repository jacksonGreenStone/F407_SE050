/**
 * @file board.h
 * @brief Board configuration header
 */

#ifndef BOARD_H
#define BOARD_H

#include "stm32f4xx_hal.h"

/* I2C handle for SE050 communication */
#define I2C_SE050_HANDLE hi2c1
extern I2C_HandleTypeDef I2C_SE050_HANDLE;

/**
 * @brief Initialize I2C for SE050 communication
 */
void board_i2c_init(void);

/**
 * @brief Write data to SE050 over I2C
 * @param addr I2C address
 * @param data Data to write
 * @param len Length of data
 * @retval 0 if successful, non-zero otherwise
 */
int board_i2c_write(uint8_t addr, uint8_t *data, size_t len);

/**
 * @brief Read data from SE050 over I2C
 * @param addr I2C address
 * @param data Buffer to store read data
 * @param len Length of data to read
 * @retval 0 if successful, non-zero otherwise
 */
int board_i2c_read(uint8_t addr, uint8_t *data, size_t len);

#endif /* BOARD_H */