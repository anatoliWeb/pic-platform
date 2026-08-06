#include "core/compiler.h"
#include "core/types.h"

#include "core/device.h"
#include "core/delay.h"
#include "drivers/gpio/gpio.h"
#include "drivers/communication/uart/uart.h"
#include "core/crc/crc.h"

void delay_us(uint16_t us) { (void)us; }
void delay_ms(uint16_t ms) { (void)ms; }

void gpio_set_output(volatile uint8_t* tris, uint8_t pin) { (void)tris; (void)pin; }
void gpio_write_high(volatile uint8_t* port, uint8_t pin) { (void)port; (void)pin; }
void gpio_write_low(volatile uint8_t* port, uint8_t pin) { (void)port; (void)pin; }

void uart_init(uint32_t baudrate) { (void)baudrate; }
void uart_write_byte(uint8_t data) { (void)data; }
uint8_t uart_read_byte(void) { return 0u; }
uint8_t uart_is_data_ready(void) { return 0u; }

uint16_t crc16_modbus(const uint8_t* data, uint16_t len) { (void)data; (void)len; return 0u; }

#include "XC8/drivers/communication/rs485/rs485.c"

void main(void)
{
    uint8_t data[2] = {0x01u, 0x02u};
    uint8_t result = rs485_send_frame(data, 2u);
    (void)result;
    rs485_finish_tx();
}
