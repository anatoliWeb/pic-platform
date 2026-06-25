/*
 * File: XC8/drivers/memory/i2c_eeprom/i2c_eeprom.c
 */

#include "drivers/memory/i2c_eeprom/i2c_eeprom.h"

#if defined(DRV_COMPILER_C18)
    #include "../../../C18/drivers/memory/i2c_eeprom/i2c_eeprom.c"
#elif defined(DRV_COMPILER_XC8)
    #include "../../../XC8/drivers/memory/i2c_eeprom/i2c_eeprom.c"
#else

#include "core/delay.h"
#include "drivers/communication/i2c/i2c.h"

#define I2C_EEPROM_DEFAULT_DEVICE_ADDR  0x50u
#define I2C_EEPROM_DEFAULT_PAGE_SIZE    16u
#define I2C_EEPROM_DEFAULT_ADDR_WIDTH   I2C_EEPROM_ADDR_WIDTH_8
#define I2C_EEPROM_DEFAULT_RETRIES      32u
#define I2C_EEPROM_DEFAULT_WRITE_MS     5u

static i2c_eeprom_config_t g_cfg =
{
    I2C_EEPROM_DEFAULT_DEVICE_ADDR,
    I2C_EEPROM_DEFAULT_PAGE_SIZE,
    I2C_EEPROM_DEFAULT_ADDR_WIDTH,
    I2C_EEPROM_DEFAULT_RETRIES,
    I2C_EEPROM_DEFAULT_WRITE_MS
};

/* Build effective 7-bit control address for current memory address. */
static uint8_t i2c_eeprom_build_control_addr(uint16_t address)
{
    uint8_t ctrl = g_cfg.device_address;

    if (g_cfg.address_width == I2C_EEPROM_ADDR_WIDTH_8)
    {
        ctrl = (uint8_t)(ctrl | ((address >> 8u) & 0x07u));
    }

    return ctrl;
}

/* Send memory address bytes based on configured address width. */
static uint8_t i2c_eeprom_write_address(uint16_t address)
{
    if (g_cfg.address_width == I2C_EEPROM_ADDR_WIDTH_16)
    {
        if (i2c_write_byte((uint8_t)(address >> 8u)) != 0u)
        {
            return 1u;
        }
    }

    if (i2c_write_byte((uint8_t)(address & 0xFFu)) != 0u)
    {
        return 1u;
    }

    return 0u;
}

/* Poll EEPROM ACK to detect end of internal write cycle. */
static uint8_t i2c_eeprom_wait_ready(uint16_t address)
{
    uint8_t retries = g_cfg.ready_retries;
    uint8_t ctrl = i2c_eeprom_build_control_addr(address);

    while (retries != 0u)
    {
        if (i2c_device_ready(ctrl) != 0u)
        {
            return 1u;
        }

        DRV_DELAY_MS(g_cfg.write_cycle_ms);
        retries--;
    }

    return 0u;
}

void i2c_eeprom_init(const i2c_eeprom_config_t* config)
{
    if (config == (const i2c_eeprom_config_t*)0)
    {
        return;
    }

    /* Store user configuration and sanitize defaults. */
    g_cfg = *config;

    if (g_cfg.page_size == 0u)
    {
        g_cfg.page_size = I2C_EEPROM_DEFAULT_PAGE_SIZE;
    }

    if ((g_cfg.address_width != I2C_EEPROM_ADDR_WIDTH_8) &&
        (g_cfg.address_width != I2C_EEPROM_ADDR_WIDTH_16))
    {
        g_cfg.address_width = I2C_EEPROM_DEFAULT_ADDR_WIDTH;
    }

    if (g_cfg.ready_retries == 0u)
    {
        g_cfg.ready_retries = I2C_EEPROM_DEFAULT_RETRIES;
    }

    if (g_cfg.write_cycle_ms == 0u)
    {
        g_cfg.write_cycle_ms = I2C_EEPROM_DEFAULT_WRITE_MS;
    }
}

uint8_t i2c_eeprom_is_ready(void)
{
    return i2c_eeprom_wait_ready(0u);
}

uint8_t i2c_eeprom_write_byte(uint16_t address, uint8_t data)
{
    uint8_t ctrl = i2c_eeprom_build_control_addr(address);

    i2c_start();
    if (i2c_write_byte((uint8_t)(ctrl << 1u)) != 0u)
    {
        i2c_stop();
        return 0u;
    }

    if (i2c_eeprom_write_address(address) != 0u)
    {
        i2c_stop();
        return 0u;
    }

    if (i2c_write_byte(data) != 0u)
    {
        i2c_stop();
        return 0u;
    }

    i2c_stop();
    return i2c_eeprom_wait_ready(address);
}

uint8_t i2c_eeprom_read_byte(uint16_t address, uint8_t* data)
{
    uint8_t ctrl;

    if (data == (uint8_t*)0)
    {
        return 0u;
    }

    ctrl = i2c_eeprom_build_control_addr(address);

    i2c_start();
    if (i2c_write_byte((uint8_t)(ctrl << 1u)) != 0u)
    {
        i2c_stop();
        return 0u;
    }

    if (i2c_eeprom_write_address(address) != 0u)
    {
        i2c_stop();
        return 0u;
    }

    i2c_restart();
    if (i2c_write_byte((uint8_t)((ctrl << 1u) | 0x01u)) != 0u)
    {
        i2c_stop();
        return 0u;
    }

    *data = i2c_read_byte(0u);
    i2c_stop();

    return 1u;
}

uint8_t i2c_eeprom_write_buffer(uint16_t address, const uint8_t* data, uint16_t length)
{
    uint16_t written = 0u;

    if ((data == (const uint8_t*)0) || (length == 0u))
    {
        return 0u;
    }

    /* Split writes by page boundary to avoid page wrap overwrite. */
    while (written < length)
    {
        uint16_t current = (uint16_t)(address + written);
        uint16_t page_offset = (uint16_t)(current % g_cfg.page_size);
        uint16_t page_space = (uint16_t)(g_cfg.page_size - page_offset);
        uint16_t chunk = (uint16_t)(length - written);
        uint16_t i;
        uint8_t ctrl = i2c_eeprom_build_control_addr(current);

        if (chunk > page_space)
        {
            chunk = page_space;
        }

        i2c_start();
        if (i2c_write_byte((uint8_t)(ctrl << 1u)) != 0u)
        {
            i2c_stop();
            return 0u;
        }

        if (i2c_eeprom_write_address(current) != 0u)
        {
            i2c_stop();
            return 0u;
        }

        for (i = 0u; i < chunk; i++)
        {
            if (i2c_write_byte(data[written + i]) != 0u)
            {
                i2c_stop();
                return 0u;
            }
        }

        i2c_stop();

        if (i2c_eeprom_wait_ready(current) == 0u)
        {
            return 0u;
        }

        written = (uint16_t)(written + chunk);
    }

    return 1u;
}

uint8_t i2c_eeprom_read_buffer(uint16_t address, uint8_t* data, uint16_t length)
{
    uint16_t i;

    if ((data == (uint8_t*)0) || (length == 0u))
    {
        return 0u;
    }

    for (i = 0u; i < length; i++)
    {
        if (i2c_eeprom_read_byte((uint16_t)(address + i), &data[i]) == 0u)
        {
            return 0u;
        }
    }

    return 1u;
}

#endif