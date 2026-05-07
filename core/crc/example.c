#include "core/compiler.h"
#include "core/crc/crc.h"
#include "drivers/communication/uart/uart.h"
#include "libraries/system/uart_debug/uart_debug.h"

static void print_crc16(uint16_t value)
{
    DBG_PRINT_HEX((uint8_t)(value >> 8u));
    DBG_PRINT_HEX((uint8_t)(value & 0x00FFu));
}

void main(void)
{
    uint8_t frame[] = {0xAAu, 0x03u, 0x10u, 0x20u};
    uint16_t crc;
    uint8_t crc8;
    uint8_t sum;

    uart_init(9600u);

    crc8 = crc8_dallas(frame, 4u);
    crc = crc16_modbus(frame, 4u);
    sum = checksum8(frame, 4u);

    DBG_PRINT("CRC8: 0x");
    DBG_PRINT_HEX(crc8);
    DBG_PRINT("\r\n");

    DBG_PRINT("CRC16: 0x");
    print_crc16(crc);
    DBG_PRINT("\r\n");

    DBG_PRINT("SUM8: 0x");
    DBG_PRINT_HEX(sum);
    DBG_PRINT("\r\n");

    if (crc16_modbus(frame, 4u) == crc)
    {
        DBG_PRINT("Frame CRC OK\r\n");
    }

    while (1)
    {
    }
}
