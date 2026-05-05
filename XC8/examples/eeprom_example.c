#include <xc.h>
#include "core/compiler.h"
#include "drivers/uart/uart.h"
#include "drivers/uart_debug/uart_debug.h"
#include "drivers/eeprom/eeprom.h"

void main(void)
{
    uint8_t value;

    uart_init(9600u);
    eeprom_init();

    eeprom_write_byte(0x0001u, 0x5Au);
    value = eeprom_read_byte(0x0001u);

    DBG_PRINT("XC8 EEPROM=");
    DBG_PRINT_HEX(value);
    DBG_PRINTLN("");

    while (1)
    {
    }
}
