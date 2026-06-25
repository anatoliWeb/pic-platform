/*
 * File: XC8/drivers/communication/onewire/onewire.c
 */

/* XC8/drivers/communication/onewire/onewire.c */
#include "drivers/communication/onewire/onewire.h"
#include "core/delay.h"
#include "core/interrupts.h"
#include "core/bit_utils.h"

static volatile uint8_t* g_onewire_port = (volatile uint8_t*)0;
static volatile uint8_t* g_onewire_latch = (volatile uint8_t*)0;
static volatile uint8_t* g_onewire_tris = (volatile uint8_t*)0;
static uint8_t g_onewire_pin = 0u;

/*
 * Local 1-Wire tuning delay.
 *
 * This delay intentionally uses raw tuning ticks, not real microseconds.
 * There is no minimum clamp here:
 *   0 = no delay
 *   1 = smallest available software delay step
 *   10 = ten tuning steps
 *
 * This is used only for very short 1-Wire slots where function-call based
 * microsecond delays are too coarse.
 */
#define ONEWIRE_TUNING_DELAY(ticks)         \
    do                                      \
    {                                       \
        uint16_t ow_delay_ticks_;           \
        ow_delay_ticks_ = (uint16_t)(ticks);\
        while (ow_delay_ticks_ > 0u)        \
        {                                   \
            NOP();                          \
            ow_delay_ticks_--;              \
        }                                   \
    } while (0)

static const uint8_t OW_CMD_SEARCH_ROM = 0xF0u;
static const uint8_t OW_CMD_READ_ROM   = 0x33u;
static const uint8_t OW_CMD_MATCH_ROM  = 0x55u;
static const uint8_t OW_CMD_SKIP_ROM   = 0xCCu;

/*
 * Standard 1-Wire timing according to DS18B20-style bus timing.
 * Use this for real hardware first.
 */
static const onewire_timing_t g_standard_timing =
{
    480u, 70u, 410u,

    6u, 64u,
    60u, 10u,

    6u, 9u, 55u
};

/*
 * Proteus/PIC18F452 working timing preset.
 *
 * Reset and read use real microseconds.
 * Write slots keep local tuning ticks because the validated direct test
 * showed that these values are stable only as short raw tuning windows.
 */
static const onewire_timing_t g_proteus_pic18f452_timing =
{
    580u, 50u, 480u,

    2u, 50u,
    50u, 2u,

    2u, 6u, 20u
};

static onewire_timing_t g_timing =
{
    480u, 70u, 410u,

    6u, 64u,
    60u, 10u,

    6u, 9u, 55u
};

typedef enum
{
    ONEWIRE_TIMING_PROFILE_STANDARD = 0u,
    ONEWIRE_TIMING_PROFILE_CUSTOM = 1u,
    ONEWIRE_TIMING_PROFILE_PROTEUS_PIC18F452 = 2u
} onewire_timing_profile_t;

static onewire_timing_profile_t g_timing_profile = ONEWIRE_TIMING_PROFILE_STANDARD;
static void onewire_resolve_latch(volatile uint8_t* port)
{
#if defined(PORTA) && defined(LATA)
    if (port == &PORTA)
    {
        g_onewire_latch = &LATA;
        return;
    }
#endif

#if defined(PORTB) && defined(LATB)
    if (port == &PORTB)
    {
        g_onewire_latch = &LATB;
        return;
    }
#endif

#if defined(PORTC) && defined(LATC)
    if (port == &PORTC)
    {
        g_onewire_latch = &LATC;
        return;
    }
#endif

#if defined(PORTD) && defined(LATD)
    if (port == &PORTD)
    {
        g_onewire_latch = &LATD;
        return;
    }
#endif

#if defined(PORTE) && defined(LATE)
    if (port == &PORTE)
    {
        g_onewire_latch = &LATE;
        return;
    }
#endif

    g_onewire_latch = port;
}

static void onewire_drive_low(void)
{
    /*
     * 1-Wire is open-drain style.
     * Drive only LOW.
     */
    CLEAR_BIT((*g_onewire_latch), g_onewire_pin);
    CLEAR_BIT((*g_onewire_tris), g_onewire_pin);
}

static void onewire_release_line(void)
{
    /*
     * Release bus.
     * External pull-up pulls DQ HIGH.
     */
    SET_BIT((*g_onewire_tris), g_onewire_pin);
}

static uint8_t onewire_read_line(void)
{
    /*
     * Read physical pin from PORT, not LAT.
     */
    return (uint8_t)READ_BIT((*g_onewire_port), g_onewire_pin);
}

#if defined(DRV_COMPILER_XC8)
/*
 * Proteus direct-test timing preset.
 * Reset/read/search use literal __delay_us() values to match the validated
 * scope timing from the working SEARCH_ROM test.
 */
static uint8_t onewire_reset_proteus_pic18f452(void)
{
    uint8_t present;

    DRV_INT_DISABLE();
    onewire_drive_low();
    __delay_us(580u);

    onewire_release_line();
    __delay_us(50u);

    present = (onewire_read_line() == 0u) ? 1u : 0u;

    __delay_us(480u);
    DRV_INT_ENABLE();

    return present;
}

static uint8_t onewire_read_bit_proteus_pic18f452(void)
{
    uint8_t value;

    DRV_INT_DISABLE();
    onewire_drive_low();
    __delay_us(2u);

    onewire_release_line();
    __delay_us(6u);

    value = (onewire_read_line() != 0u) ? 1u : 0u;

    __delay_us(20u);
    DRV_INT_ENABLE();

    return value;
}

static void onewire_write_bit_proteus_pic18f452(uint8_t bit)
{
    DRV_INT_DISABLE();

    if ((bit & 0x01u) != 0u)
    {
        onewire_drive_low();
        __delay_us(2u);
        onewire_release_line();
        __delay_us(50u);
    }
    else
    {
        onewire_drive_low();
        __delay_us(50u);
        onewire_release_line();
        __delay_us(2u);
    }

    DRV_INT_ENABLE();
}

static void onewire_write_byte_proteus_pic18f452(uint8_t byte)
{
    uint8_t i;

    for (i = 0u; i < 8u; i++)
    {
        onewire_write_bit_proteus_pic18f452((uint8_t)(byte & 0x01u));
        byte >>= 1u;
    }
}

static uint8_t onewire_read_byte_proteus_pic18f452(void)
{
    uint8_t i;
    uint8_t value = 0u;

    for (i = 0u; i < 8u; i++)
    {
        if (onewire_read_bit_proteus_pic18f452() != 0u)
        {
            value |= (uint8_t)(1u << i);
        }
    }

    return value;
}
#endif
static void onewire_copy_timing(onewire_timing_t* dst, const onewire_timing_t* src)
{
    if ((dst == (onewire_timing_t*)0) || (src == (const onewire_timing_t*)0))
    {
        return;
    }

    dst->reset_low_us = src->reset_low_us;
    dst->presence_sample_us = src->presence_sample_us;
    dst->reset_finish_us = src->reset_finish_us;

    dst->write_1_low_us = src->write_1_low_us;
    dst->write_1_release_us = src->write_1_release_us;

    dst->write_0_low_us = src->write_0_low_us;
    dst->write_0_release_us = src->write_0_release_us;

    dst->read_low_us = src->read_low_us;
    dst->read_sample_us = src->read_sample_us;
    dst->read_finish_us = src->read_finish_us;
}

void onewire_init(volatile uint8_t* port, volatile uint8_t* tris, uint8_t pin)
{
    g_onewire_port = port;
    g_onewire_tris = tris;
    g_onewire_pin = pin;

    onewire_resolve_latch(port);
    onewire_release_line();
}

void onewire_set_timing(const onewire_timing_t* timing)
{
    if (timing == (const onewire_timing_t*)0)
    {
        return;
    }

    onewire_copy_timing(&g_timing, timing);
    g_timing_profile = ONEWIRE_TIMING_PROFILE_CUSTOM;
}

void onewire_use_standard_timing(void)
{
    onewire_copy_timing(&g_timing, &g_standard_timing);
    g_timing_profile = ONEWIRE_TIMING_PROFILE_STANDARD;
}

void onewire_use_proteus_pic18f452_timing(void)
{
    onewire_copy_timing(&g_timing, &g_proteus_pic18f452_timing);
    g_timing_profile = ONEWIRE_TIMING_PROFILE_PROTEUS_PIC18F452;
}

void onewire_use_proteus_pic18f452_8mhz_timing(void)
{
    onewire_use_proteus_pic18f452_timing();
}
uint8_t onewire_reset(void)
{
#if defined(DRV_COMPILER_XC8)
    if (g_timing_profile == ONEWIRE_TIMING_PROFILE_PROTEUS_PIC18F452)
    {
        return onewire_reset_proteus_pic18f452();
    }
#endif

    {
        uint8_t present;

        DRV_INT_DISABLE();
        onewire_drive_low();
        DRV_MICRO_DELAY_US(g_timing.reset_low_us);

        onewire_release_line();
        DRV_MICRO_DELAY_US(g_timing.presence_sample_us);

        present = (onewire_read_line() == 0u) ? 1u : 0u;

        DRV_MICRO_DELAY_US(g_timing.reset_finish_us);
        DRV_INT_ENABLE();

        return present;
    }
}

void onewire_write_bit(uint8_t bit)
{
#if defined(DRV_COMPILER_XC8)
    if (g_timing_profile == ONEWIRE_TIMING_PROFILE_PROTEUS_PIC18F452)
    {
        onewire_write_bit_proteus_pic18f452(bit);
        return;
    }
#endif

    DRV_INT_DISABLE();

    if ((bit & 0x01u) != 0u)
    {
        /*
         * Write 1 slot.
         *
         * Timing values are tuning ticks, not real microseconds.
         * For the validated Proteus preset, write slot values are raw tuning
         * windows, not microsecond delays.
         * Example:
         *   write_1_low_us     = 1
         *   write_1_release_us = 10
         *
         * There is no minimum clamp. Small values must remain small.
         * Do not call UART or debug printing inside timing-critical 1-Wire
         * slots.
         */
        onewire_drive_low();
        ONEWIRE_TUNING_DELAY(g_timing.write_1_low_us);

        onewire_release_line();
        ONEWIRE_TUNING_DELAY(g_timing.write_1_release_us);
    }
    else
    {
        /*
         * Write 0 slot.
         *
         * Timing values are tuning ticks, not real microseconds.
         */
        onewire_drive_low();
        ONEWIRE_TUNING_DELAY(g_timing.write_0_low_us);

        onewire_release_line();
        ONEWIRE_TUNING_DELAY(g_timing.write_0_release_us);
    }

    DRV_INT_ENABLE();
}

uint8_t onewire_read_bit(void)
{
#if defined(DRV_COMPILER_XC8)
    if (g_timing_profile == ONEWIRE_TIMING_PROFILE_PROTEUS_PIC18F452)
    {
        return onewire_read_bit_proteus_pic18f452();
    }
#endif

    {
        uint8_t value;

        DRV_INT_DISABLE();
        onewire_drive_low();
        DRV_MICRO_DELAY_US(g_timing.read_low_us);

        onewire_release_line();
        DRV_MICRO_DELAY_US(g_timing.read_sample_us);

        value = (onewire_read_line() != 0u) ? 1u : 0u;

        DRV_MICRO_DELAY_US(g_timing.read_finish_us);
        DRV_INT_ENABLE();

        return value;
    }
}

void onewire_write_byte(uint8_t byte)
{
#if defined(DRV_COMPILER_XC8)
    if (g_timing_profile == ONEWIRE_TIMING_PROFILE_PROTEUS_PIC18F452)
    {
        onewire_write_byte_proteus_pic18f452(byte);
        return;
    }
#endif

    uint8_t i;

    for (i = 0u; i < 8u; i++)
    {
        onewire_write_bit((uint8_t)(byte & 0x01u));
        byte >>= 1u;
    }
}

uint8_t onewire_read_byte(void)
{
#if defined(DRV_COMPILER_XC8)
    if (g_timing_profile == ONEWIRE_TIMING_PROFILE_PROTEUS_PIC18F452)
    {
        return onewire_read_byte_proteus_pic18f452();
    }
#endif

    uint8_t i;
    uint8_t value = 0u;

    for (i = 0u; i < 8u; i++)
    {
        if (onewire_read_bit() != 0u)
        {
            value |= (uint8_t)(1u << i);
        }
    }

    return value;
}

void onewire_skip_rom(void)
{
    onewire_write_byte(OW_CMD_SKIP_ROM);
}

void onewire_match_rom(uint8_t* rom)
{
    uint8_t i;

    if (rom == (uint8_t*)0)
    {
        return;
    }

    onewire_write_byte(OW_CMD_MATCH_ROM);

    for (i = 0u; i < 8u; i++)
    {
        onewire_write_byte(rom[i]);
    }
}

void onewire_read_rom(uint8_t* rom)
{
    uint8_t i;

    if (rom == (uint8_t*)0)
    {
        return;
    }

    onewire_write_byte(OW_CMD_READ_ROM);

    for (i = 0u; i < 8u; i++)
    {
        rom[i] = onewire_read_byte();
    }
}
uint8_t onewire_search_rom(uint8_t (*roms)[8], uint8_t max_devices)
{
    uint8_t found_devices = 0u;
    uint8_t search_rom[8] = {0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u};
    uint8_t last_discrepancy = 0u;
    uint8_t last_device_flag = 0u;
    uint8_t id_bit_number;
    uint8_t last_zero;
    uint8_t rom_byte_number;
    uint8_t rom_byte_mask;
    uint8_t id_bit;
    uint8_t cmp_id_bit;
    uint8_t search_direction;
    uint8_t i;

    if ((roms == (uint8_t (*)[8])0) || (max_devices == 0u))
    {
        return 0u;
    }

    while ((last_device_flag == 0u) && (found_devices < max_devices))
    {
        if (onewire_reset() == 0u)
        {
            break;
        }

        onewire_write_byte(OW_CMD_SEARCH_ROM);

        last_zero = 0u;
        id_bit_number = 1u;
        rom_byte_number = 0u;
        rom_byte_mask = 1u;

        while (rom_byte_number < 8u)
        {
            id_bit = onewire_read_bit();
            cmp_id_bit = onewire_read_bit();

            if ((id_bit != 0u) && (cmp_id_bit != 0u))
            {
                return found_devices;
            }

            if (id_bit != cmp_id_bit)
            {
                search_direction = id_bit;
            }
            else
            {
                if (id_bit_number < last_discrepancy)
                {
                    search_direction = (uint8_t)((search_rom[rom_byte_number] & rom_byte_mask) != 0u);
                }
                else
                {
                    search_direction = (uint8_t)((id_bit_number == last_discrepancy) ? 1u : 0u);
                }

                if (search_direction == 0u)
                {
                    last_zero = id_bit_number;
                }
            }

            if (search_direction != 0u)
            {
                search_rom[rom_byte_number] |= rom_byte_mask;
            }
            else
            {
                search_rom[rom_byte_number] &= (uint8_t)(~rom_byte_mask);
            }

            onewire_write_bit(search_direction);

            id_bit_number++;
            rom_byte_mask <<= 1u;

            if (rom_byte_mask == 0u)
            {
                rom_byte_number++;
                rom_byte_mask = 1u;
            }
        }

        last_discrepancy = last_zero;

        if (last_discrepancy == 0u)
        {
            last_device_flag = 1u;
        }

        if (onewire_crc8(search_rom, 8u) == 0u)
        {
            for (i = 0u; i < 8u; i++)
            {
                roms[found_devices][i] = search_rom[i];
            }

            found_devices++;
        }
    }

    return found_devices;
}

uint8_t onewire_crc8(const uint8_t* data, uint8_t len)
{
    uint8_t i;
    uint8_t j;
    uint8_t crc = 0u;
    uint8_t in_byte;
    uint8_t mix;

    if (data == (const uint8_t*)0)
    {
        return 0u;
    }

    for (i = 0u; i < len; i++)
    {
        in_byte = data[i];

        for (j = 0u; j < 8u; j++)
        {
            mix = (uint8_t)((crc ^ in_byte) & 0x01u);

            crc >>= 1u;

            if (mix != 0u)
            {
                crc ^= 0x8Cu;
            }

            in_byte >>= 1u;
        }
    }

    return crc;
}
