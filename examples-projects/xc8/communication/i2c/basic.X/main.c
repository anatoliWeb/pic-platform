/*
 * File: examples-projects/xc8/i2c.X/main.c
 */

#include "project_config.h"

#include "core/compiler.h"
#include "core/delay.h"

#include "drivers/communication/i2c/i2c.h"
#include "drivers/communication/uart/uart.h"
#include "libraries/system/uart_debug/uart_debug.h"

#define I2C_SPEED_HZ        10000u

#define PCF8574_ADDR_START  0x20u
#define PCF8574_ADDR_END    0x27u

#define PCF8574A_ADDR_START 0x38u
#define PCF8574A_ADDR_END   0x3Fu

static uint8_t g_pcf_addr = 0u;

static void print_i2c_address_info(uint8_t addr7)
{
    uint8_t write_addr;
    uint8_t read_addr;

    write_addr = (uint8_t)(addr7 << 1u);
    read_addr = (uint8_t)((addr7 << 1u) | 0x01u);

    DBG_PRINT("7-bit=");
    DBG_PRINT_HEX(addr7);
    DBG_PRINT(" write=");
    DBG_PRINT_HEX(write_addr);
    DBG_PRINT(" read=");
    DBG_PRINT_HEX(read_addr);
    DBG_PRINTLN("");
}

static void scan_i2c_bus(void)
{
    uint8_t addr;
    uint8_t found_count = 0u;

    DBG_PRINTLN("");
    DBG_PRINTLN("Scanning I2C bus 0x03..0x77");

    for (addr = 0x03u; addr <= 0x77u; addr++)
    {
        if (i2c_device_ready(addr) != 0u)
        {
            DBG_PRINT("FOUND ");
            print_i2c_address_info(addr);
            found_count++;
        }

        DRV_DELAY_MS(2);
    }

    DBG_PRINT("Total found: ");
    DBG_PRINT_INT((int)found_count);
    DBG_PRINTLN("");
}

static uint8_t pcf8574_write(uint8_t value)
{
    uint8_t result;

    if (g_pcf_addr == 0u)
    {
        return 1u;
    }

    /*
     * PCF8574 has no register address.
     * Write one byte directly to set P0..P7.
     */
    i2c_start();

    result = i2c_write_byte((uint8_t)(g_pcf_addr << 1u));
    if (result != 0u)
    {
        i2c_stop();
        return 1u;
    }

    result = i2c_write_byte(value);
    if (result != 0u)
    {
        i2c_stop();
        return 1u;
    }

    i2c_stop();

    return 0u;
}

static uint8_t pcf8574_read(uint8_t *value)
{
    uint8_t result;

    if ((g_pcf_addr == 0u) || (value == (uint8_t*)0))
    {
        return 1u;
    }

    /*
     * PCF8574 has no register address.
     * Read one byte directly from P0..P7.
     */
    i2c_start();

    result = i2c_write_byte((uint8_t)((g_pcf_addr << 1u) | 0x01u));
    if (result != 0u)
    {
        i2c_stop();
        return 1u;
    }

    *value = i2c_read_byte(0u);

    i2c_stop();

    return 0u;
}

static uint8_t find_pcf8574_range(uint8_t start_addr, uint8_t end_addr)
{
    uint8_t addr;

    for (addr = start_addr; addr <= end_addr; addr++)
    {
        if (i2c_device_ready(addr) != 0u)
        {
            return addr;
        }
    }

    return 0u;
}

static uint8_t find_pcf8574(void)
{
    uint8_t addr;

    /*
     * Normal PCF8574 address range:
     *   A2 A1 A0 = 000 -> 0x20
     *   A2 A1 A0 = 111 -> 0x27
     */
    addr = find_pcf8574_range(PCF8574_ADDR_START, PCF8574_ADDR_END);

    if (addr != 0u)
    {
        return addr;
    }

    /*
     * PCF8574A address range:
     *   A2 A1 A0 = 000 -> 0x38
     *   A2 A1 A0 = 111 -> 0x3F
     */
    addr = find_pcf8574_range(PCF8574A_ADDR_START, PCF8574A_ADDR_END);

    return addr;
}

static void print_byte_binary(uint8_t value)
{
    int8_t bit;

    for (bit = 7; bit >= 0; bit--)
    {
        if ((value & (uint8_t)(1u << (uint8_t)bit)) != 0u)
        {
            DBG_PRINT("1");
        }
        else
        {
            DBG_PRINT("0");
        }
    }
}

void main(void)
{
    uint8_t value;
    uint8_t pattern;
    uint8_t bit;

    /*
     * PIC18F452 + PCF8574 I2C example.
     *
     * I2C wiring:
     *   RC3/SCL / pin 18 -> PCF8574 SCL
     *   RC4/SDA / pin 23 -> PCF8574 SDA
     *
     * Pull-ups:
     *   SCL -> 4.7k -> VCC
     *   SDA -> 4.7k -> VCC
     *
     * PCF8574 address pins:
     *   A0 -> GND
     *   A1 -> GND
     *   A2 -> GND
     *
     * Expected address:
     *   PCF8574  = 0x20
     *   PCF8574A = 0x38
     *
     * UART debug:
     *   RC6/TX / pin 25 -> Virtual Terminal RXD
     */

    uart_init(9600u);
    i2c_init(I2C_SPEED_HZ);

    DBG_PRINTLN("");
    DBG_PRINTLN("PIC18F452 PCF8574 I2C test");
    DBG_PRINTLN("SCL=RC3/pin18, SDA=RC4/pin23");
    DBG_PRINT("Speed=");
    DBG_PRINT_INT((int)I2C_SPEED_HZ);
    DBG_PRINTLN(" Hz");
    DBG_PRINTLN("Expected PCF8574 address: 0x20..0x27");
    DBG_PRINTLN("Expected PCF8574A address: 0x38..0x3F");

    /*
     * Step 1: scan whole I2C bus.
     */
    scan_i2c_bus();

    /*
     * Step 2: find PCF8574.
     */
    g_pcf_addr = find_pcf8574();

    DBG_PRINTLN("");

    if (g_pcf_addr == 0u)
    {
        DBG_PRINTLN("PCF8574 NOT FOUND");
        DBG_PRINTLN("Check VCC, GND, SCL, SDA, pull-ups, A0/A1/A2.");

        while (1)
        {
            DRV_DELAY_MS(1000);
        }
    }

    DBG_PRINT("PCF8574 FOUND AT ");
    DBG_PRINT_HEX(g_pcf_addr);
    DBG_PRINTLN("");

    /*
     * PCF8574 pins are quasi-bidirectional.
     * Write 1 to a pin to use it as input or weak high.
     * Write 0 to drive it low.
     *
     * Start with all pins high.
     */
    if (pcf8574_write(0xFFu) != 0u)
    {
        DBG_PRINTLN("PCF8574 INITIAL WRITE ERROR");
    }
    else
    {
        DBG_PRINTLN("PCF8574 INITIAL WRITE OK");
    }

    while (1)
    {
        /*
         * Running LED test.
         *
         * This pattern drives one pin low at a time.
         * If LEDs are connected from VCC -> resistor -> PCF pin,
         * the selected LED will turn ON when the pin is LOW.
         */
        for (bit = 0u; bit < 8u; bit++)
        {
            pattern = (uint8_t)(0xFFu & (uint8_t)(~(1u << bit)));

            DBG_PRINT("WRITE pattern=");
            DBG_PRINT_HEX(pattern);
            DBG_PRINT(" active P");
            DBG_PRINT_INT((int)bit);
            DBG_PRINTLN("");

            if (pcf8574_write(pattern) != 0u)
            {
                DBG_PRINTLN("WRITE ERROR");
            }

            DRV_DELAY_MS(300);

            value = 0u;

            if (pcf8574_read(&value) == 0u)
            {
                DBG_PRINT("READ value=");
                DBG_PRINT_HEX(value);
                DBG_PRINT(" bin=");
                print_byte_binary(value);
                DBG_PRINTLN("");
            }
            else
            {
                DBG_PRINTLN("READ ERROR");
            }

            DRV_DELAY_MS(300);
        }

        /*
         * Turn all pins high again.
         */
        pcf8574_write(0xFFu);
        DRV_DELAY_MS(500);
    }
}