/**
 * @file board_log.h
 * @brief Board logging functions header
 */

#ifndef BOARD_LOG_H
#define BOARD_LOG_H

/**
 * @brief Initialize board logging
 */
void board_log_init(void);

/**
 * @brief Log a message
 * @param format Format string
 * @param ... Arguments
 */
void board_log(const char* format, ...);

#endif /* BOARD_LOG_H */