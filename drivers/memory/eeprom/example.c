#include "core/compiler.h"
#include "drivers/memory/eeprom/eeprom.h"
#include "drivers/communication/uart/uart.h"
#include "libraries/system/uart_debug/uart_debug.h"

#define EEPROM_SETTING_ADDR  0x0010u
#define EEPROM_BLOCK_ADDR    0x0020u

void main(void)
{
    uint8_t setting;
    uint8_t tx_block[4] = { 1u, 2u, 3u, 4u };
    uint8_t rx_block[4];

    uart_init(9600u);
    eeprom_init();

    /* Save setting (wear-safe). */
    eeprom_update_byte(EEPROM_SETTING_ADDR, 42u);

    /* Restore setting after reset. */
    setting = eeprom_read_byte(EEPROM_SETTING_ADDR);
    DBG_PRINT("SETTING=");
    DBG_PRINT_INT((int)setting);
    DBG_PRINTLN("");

    /* Block write/read example. */
    eeprom_write_block(EEPROM_BLOCK_ADDR, tx_block, 4u);
    eeprom_read_block(EEPROM_BLOCK_ADDR, rx_block, 4u);

    DBG_PRINT("BLOCK0=");
    DBG_PRINT_INT((int)rx_block[0]);
    DBG_PRINTLN("");

    while (1)
    {
    }
}
