/**
 * @file board_log.c
 * @brief Board logging functions
 */

#include "board_log.h"
#include <stdio.h>
#include <stdarg.h>

#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

/**
 * @brief Initialize board logging
 */
void board_log_init(void)
{
    /* UART should be initialized in main.c */
    /* This is just a placeholder for any additional logging initialization */
}

/**
 * @brief Log a message
 * @param format Format string
 * @param ... Arguments
 */
void board_log(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
}

/**
 * @brief Retargets the C library printf function to the USART
 * @param ch Character to send
 * @return Character sent
 */
PUTCHAR_PROTOTYPE
{
    /* Place your implementation of fputc here */
    /* e.g. write a character to the USART and Loop until the end of transmission */
    // HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, 0xFFFF);

    return ch;
}