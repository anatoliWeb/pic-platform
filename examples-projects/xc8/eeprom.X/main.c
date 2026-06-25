/*
 * File: examples-projects/xc8/eeprom.X/main.c
 */

#include "project_config.h"

#include "core/compiler.h"
#include "core/delay.h"
#include "drivers/communication/uart/uart.h"
#include "drivers/memory/eeprom/eeprom.h"
#include "libraries/system/uart_debug/uart_debug.h"

/*
 * PIC18F452 internal EEPROM example.
 *
 * This example writes one byte to internal Data EEPROM,
 * reads it back, and prints the result through UART.
 *
 * Proteus:
 *   - No external EEPROM chip is required.
 *   - This uses internal EEPROM inside PIC18F452.
 *   - UART TX: RC6 / pin 25 -> Virtual Terminal RXD.
 */

#define EEPROM_TEST_ADDRESS  0x0001u
#define EEPROM_TEST_VALUE    0x5Au

static void print_result(uint8_t value)
{
    DBG_PRINT("EEPROM READ=");
    DBG_PRINT_HEX(value);

    if (value == EEPROM_TEST_VALUE)
    {
        DBG_PRINTLN(" OK");
    }
    else
    {
        DBG_PRINTLN(" FAIL");
    }
}

void main(void)
{
    uint8_t value;

    uart_init(9600u);
    eeprom_init();

    DBG_PRINTLN("");
    DBG_PRINTLN("PIC18F452 internal EEPROM test");
    DBG_PRINTLN("Write 0x5A to address 0x0001");

    eeprom_write_byte(EEPROM_TEST_ADDRESS, EEPROM_TEST_VALUE);

    /*
     * Small delay after EEPROM write for safety.
     * The driver should already wait for write completion,
     * but this keeps the example friendly for simulation.
     */
    delay_ms(10u);

    value = eeprom_read_byte(EEPROM_TEST_ADDRESS);

    print_result(value);

    while (1)
    {
        delay_ms(1000u);
    }
}