#include "project_config.h"

#include "core/compiler.h"
#include "core/types.h"
#include "core/delay.h"

#include "libraries/output/ac_phase_control/ac_phase_control.h"

#if defined(DRV_COMPILER_C18)
#include <p18f452.h>
#endif

#if defined(DRV_COMPILER_XC8)
#include <xc.h>
#endif

/*
 * PIC18F452 multi-channel low-voltage phase-control test.
 *
 * RB0:
 *   Fake zero-cross input.
 *   One rising edge every 10 ms.
 *
 * RD0:
 *   Gate pulse after 1500 us.
 *
 * RD1:
 *   Gate pulse after 3500 us.
 *
 * RD2:
 *   Gate pulse after 5500 us.
 *
 * RD3:
 *   Gate pulse after 7500 us.
 *
 * This diagnostic version detects the RB0 rising edge by polling.
 * Timer2 remains interrupt-driven.
 */

#define AC_PHASE_CHANNEL_COUNT     4U
#define AC_PHASE_PATTERN_COUNT     4U
#define AC_PHASE_PATTERN_TIME_MS   1000UL

static ac_phase_control_group_t g_phase_group;

static ac_phase_control_channel_t
    g_phase_channels[AC_PHASE_CHANNEL_COUNT];

static const ac_phase_control_config_t g_phase_config =
{
    10000U, /* Half-cycle duration for 50 Hz AC. */
    500U,   /* Minimum firing delay. */
    9500U,  /* Maximum firing delay. */
    500U    /* Gate pulse duration for Proteus visibility. */
};

static const uint16_t
    g_delay_patterns[AC_PHASE_PATTERN_COUNT][AC_PHASE_CHANNEL_COUNT] =
{
    { 1500U, 3500U, 5500U, 7500U },
    { 3500U, 5500U, 7500U, 1500U },
    { 5500U, 7500U, 1500U, 3500U },
    { 7500U, 1500U, 3500U, 5500U }
};

static uint8_t g_current_pattern = 0U;
static uint32_t g_last_pattern_ms = 0UL;

/*
 * Previous RB0 state used for rising-edge detection.
 */
static uint8_t g_previous_zero_cross_state = 0U;

static void outputs_all_low(void)
{
    LATD &= (uint8_t)(~0x0FU);
}

static void enter_error_state(void)
{
    INTCONbits.GIE = 0U;
    INTCONbits.PEIE = 0U;

    outputs_all_low();

    while (1)
    {
        /*
         * Initialization error indication.
         */
        LATD ^= 0x0FU;
        DRV_DELAY_MS(150U);
    }
}

static void gpio_startup_test(void)
{
    outputs_all_low();

    LATDbits.LATD0 = 1U;
    DRV_DELAY_MS(300U);
    LATDbits.LATD0 = 0U;

    LATDbits.LATD1 = 1U;
    DRV_DELAY_MS(300U);
    LATDbits.LATD1 = 0U;

    LATDbits.LATD2 = 1U;
    DRV_DELAY_MS(300U);
    LATDbits.LATD2 = 0U;

    LATDbits.LATD3 = 1U;
    DRV_DELAY_MS(300U);
    LATDbits.LATD3 = 0U;

    LATD |= 0x0FU;
    DRV_DELAY_MS(500U);

    outputs_all_low();
    DRV_DELAY_MS(300U);
}

static void io_init(void)
{
    /*
     * Configure analog-capable pins as digital.
     */
    ADCON1 = 0x07U;

#if defined(CMCON)
    CMCON = 0x07U;
#endif

    /*
     * Disable interrupt priority mode.
     */
    RCONbits.IPEN = 0U;

    /*
     * Disable Parallel Slave Port mode.
     */
    TRISEbits.PSPMODE = 0U;

    /*
     * RB0 is the zero-cross input.
     */
    TRISBbits.TRISB0 = 1U;

    /*
     * RD0..RD3 are phase-control outputs.
     */
    TRISD &= (uint8_t)(~0x0FU);

    outputs_all_low();
}

static drv_status_t attach_channels(void)
{
    drv_status_t status;

    status = ac_phase_control_attach_channel(
        &g_phase_group,
        0U,
        &LATD,
        &TRISD,
        0x01U
    );

    if (status != DRV_STATUS_OK)
    {
        return status;
    }

    status = ac_phase_control_attach_channel(
        &g_phase_group,
        1U,
        &LATD,
        &TRISD,
        0x02U
    );

    if (status != DRV_STATUS_OK)
    {
        return status;
    }

    status = ac_phase_control_attach_channel(
        &g_phase_group,
        2U,
        &LATD,
        &TRISD,
        0x04U
    );

    if (status != DRV_STATUS_OK)
    {
        return status;
    }

    status = ac_phase_control_attach_channel(
        &g_phase_group,
        3U,
        &LATD,
        &TRISD,
        0x08U
    );

    return status;
}

static drv_status_t apply_pattern(uint8_t pattern_index)
{
    drv_status_t status;
    uint8_t channel;
    uint8_t gie_state;

    if (pattern_index >= AC_PHASE_PATTERN_COUNT)
    {
        return DRV_STATUS_ERROR;
    }

    /*
     * Protect multi-byte channel values from Timer2 ISR access.
     */
    gie_state = INTCONbits.GIE;
    INTCONbits.GIE = 0U;

    for (channel = 0U;
         channel < AC_PHASE_CHANNEL_COUNT;
         channel++)
    {
        status = ac_phase_control_set_delay_us(
            &g_phase_group,
            channel,
            g_delay_patterns[pattern_index][channel]
        );

        if (status != DRV_STATUS_OK)
        {
            INTCONbits.GIE = gie_state;
            return status;
        }
    }

    INTCONbits.GIE = gie_state;

    return DRV_STATUS_OK;
}

static drv_status_t phase_control_init(void)
{
    drv_status_t status;

    status = ac_phase_control_init_group(
        &g_phase_group,
        AC_PHASE_CONTROL_TIMER2,
        &g_phase_config,
        g_phase_channels,
        AC_PHASE_CHANNEL_COUNT
    );

    if (status != DRV_STATUS_OK)
    {
        return status;
    }

    status = attach_channels();

    if (status != DRV_STATUS_OK)
    {
        return status;
    }

    return apply_pattern(0U);
}

static void zero_cross_poll(void)
{
    uint8_t current_state;

    current_state =
        (PORTBbits.RB0 != 0U) ? 1U : 0U;

    /*
     * Detect only LOW-to-HIGH transition.
     */
    if ((current_state != 0U) &&
        (g_previous_zero_cross_state == 0U))
    {
        ac_phase_control_on_zero_cross(
            &g_phase_group
        );
    }

    g_previous_zero_cross_state = current_state;
}

static void update_pattern(void)
{
    uint32_t now_ms;

    now_ms =
        ac_phase_control_get_tick_ms(&g_phase_group);

    if ((uint32_t)(now_ms - g_last_pattern_ms) <
        AC_PHASE_PATTERN_TIME_MS)
    {
        return;
    }

    g_last_pattern_ms = now_ms;

    g_current_pattern++;

    if (g_current_pattern >= AC_PHASE_PATTERN_COUNT)
    {
        g_current_pattern = 0U;
    }

    if (apply_pattern(g_current_pattern) !=
        DRV_STATUS_OK)
    {
        enter_error_state();
    }
}

#if defined(DRV_COMPILER_XC8)

void __interrupt() isr(void)
{
    /*
     * Only Timer2 is interrupt-driven in this diagnostic example.
     */
    if ((PIE1bits.TMR2IE != 0U) &&
        (PIR1bits.TMR2IF != 0U))
    {
        ac_phase_control_irq_handler();
    }
}

#endif

#if defined(DRV_COMPILER_C18)

#pragma interrupt high_isr
void high_isr(void)
{
    if ((PIE1bits.TMR2IE != 0U) &&
        (PIR1bits.TMR2IF != 0U))
    {
        ac_phase_control_irq_handler();
    }
}

#pragma code high_vector = 0x08
void high_vector(void)
{
    _asm goto high_isr _endasm
}
#pragma code

#endif

void main(void)
{
    drv_status_t status;

    /*
     * Keep interrupts disabled during initialization.
     */
    INTCONbits.GIE = 0U;
    INTCONbits.PEIE = 0U;
    INTCONbits.INT0IE = 0U;

    io_init();

    /*
     * Verify RD0..RD3 before starting phase control.
     */
    gpio_startup_test();

    status = phase_control_init();

    if (status != DRV_STATUS_OK)
    {
        enter_error_state();
    }

    /*
     * Initialize edge detector from the current RB0 level.
     */
    g_previous_zero_cross_state =
        (PORTBbits.RB0 != 0U) ? 1U : 0U;

    /*
     * Clear pending Timer2 interrupt before enabling interrupts.
     */
    PIR1bits.TMR2IF = 0U;

    INTCONbits.PEIE = 1U;
    INTCONbits.GIE = 1U;

    while (1)
    {
        /*
         * Detect zero-cross events from RB0.
         */
        zero_cross_poll();

        /*
         * Rotate firing-delay patterns every second.
         */
        update_pattern();
    }
}