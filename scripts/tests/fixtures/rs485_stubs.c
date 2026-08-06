#include "core/compiler.h"
#include "core/types.h"

typedef unsigned char uint8_t;
typedef unsigned int uint16_t;
typedef unsigned long uint32_t;

typedef struct {
    volatile uint8_t TRMT;
} TXSTAbits_t;

extern volatile TXSTAbits_t TXSTAbits;

void delay_us(uint16_t us) { (void)us; }
void delay_ms(uint16_t ms) { (void)ms; }

void gpio_set_output(volatile uint8_t* tris, uint8_t pin) { (void)tris; (void)pin; }
void gpio_write_high(volatile uint8_t* port, uint8_t pin) { (void)port; (void)pin; }
void gpio_write_low(volatile uint8_t* port, uint8_t pin) { (void)port; (void)pin; }

void uart_init(uint32_t baudrate) { (void)baudrate; }
void uart_write_byte(uint8_t data) { (void)data; }
uint8_t uart_read_byte(void) { return 0u; }
uint8_t uart_is_data_ready(void) { return 0u; }

uint8_t crc8_dallas(uint8_t* data, uint8_t len) { (void)data; (void)len; return 0u; }
uint16_t crc16_modbus(uint8_t* data, uint8_t len) { (void)data; (void)len; return 0u; }
