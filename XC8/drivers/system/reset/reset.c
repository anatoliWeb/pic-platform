/*
 * File: XC8/drivers/system/reset/reset.c
 */

#include "drivers/system/reset/reset.h"

/*
 * PIC18F452 reset cause driver for XC8.
 *
 * RCON flags on PIC18F452:
 *
 *   bit0 BOR  - Brown-out Reset status
 *   bit1 POR  - Power-on Reset status
 *   bit2 PD   - Power-down status
 *   bit3 TO   - Watchdog time-out status
 *   bit4 RI   - RESET instruction status
 *
 * Most of these flags are active-low for reset cause detection:
 *
 *   POR == 0 -> Power-on Reset occurred
 *   BOR == 0 -> Brown-out Reset occurred
 *   TO  == 0 -> Watchdog time-out occurred
 *   RI  == 0 -> RESET instruction occurred
 *
 * MCLR does not have a simple dedicated cause flag here.
 * After reset flags were previously cleared, an external MCLR reset can
 * leave the specific flags in their normal state. For a practical example,
 * if no specific cause is detected, we report EXTERNAL.
 */

#define RESET_RCON_BOR_MASK  0x01u
#define RESET_RCON_POR_MASK  0x02u
#define RESET_RCON_PD_MASK   0x04u
#define RESET_RCON_TO_MASK   0x08u
#define RESET_RCON_RI_MASK   0x10u

static reset_cause_t g_reset_cause = RESET_CAUSE_UNKNOWN;

void reset_init(void)
{
    /*
     * Store reset cause immediately at startup.
     * This must happen before reset_clear_flags().
     */
    g_reset_cause = reset_get_cause();
}

reset_cause_t reset_get_cause(void)
{
    uint8_t rcon_value;

    /*
     * Read RCON once.
     * This makes the logic easier to debug and avoids repeated SFR reads.
     */
    rcon_value = RCON;

    /*
     * Power-on Reset.
     */
    if ((rcon_value & RESET_RCON_POR_MASK) == 0u)
    {
        return RESET_CAUSE_POWER_ON;
    }

    /*
     * Brown-out Reset.
     */
    if ((rcon_value & RESET_RCON_BOR_MASK) == 0u)
    {
        return RESET_CAUSE_BROWN_OUT;
    }

    /*
     * Watchdog Reset.
     */
    if ((rcon_value & RESET_RCON_TO_MASK) == 0u)
    {
        return RESET_CAUSE_WATCHDOG;
    }

    /*
     * Software reset by RESET instruction.
     */
    if ((rcon_value & RESET_RCON_RI_MASK) == 0u)
    {
        return RESET_CAUSE_SOFTWARE;
    }

    /*
     * PIC18F452 does not expose a clean separate MCLR cause flag.
     * If all specific flags look normal, treat it as external reset.
     *
     * This makes the Proteus MCLR button test useful:
     *   first start can be POWER_ON or EXTERNAL depending on simulator flags
     *   MCLR button usually becomes EXTERNAL
     */
    return RESET_CAUSE_EXTERNAL;
}

void reset_clear_flags(void)
{
    /*
     * Set reset flags back to normal state.
     *
     * This prepares the next reset event so reset_get_cause()
     * can detect POR/BOR/WDT/RESET instruction when supported.
     */
    RCON |= RESET_RCON_BOR_MASK;
    RCON |= RESET_RCON_POR_MASK;
    RCON |= RESET_RCON_PD_MASK;
    RCON |= RESET_RCON_TO_MASK;
    RCON |= RESET_RCON_RI_MASK;
}