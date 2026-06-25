/*
 * File: drivers/communication/uart/uart.h
 */

#ifndef DRIVERS_UART_UART_H
#define DRIVERS_UART_UART_H

#include "core/compiler.h"
#include "core/types.h"

/* Initialize UART with requested baudrate. */
void uart_init(uint32_t baudrate);

/* Transmit one byte or zero-terminated string. */
void uart_write_byte(uint8_t data);
void uart_write_string(const char* str);

/* Receive one byte and check RX availability. */
uint8_t uart_read_byte(void);
uint8_t uart_is_data_ready(void);

#endif /* DRIVERS_UART_UART_H */
