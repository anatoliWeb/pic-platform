/*
 * File: examples-projects/xc8/output/multi_channel_relay.X/main.c
 */

#include "project_config.h"

#include "core/compiler.h"
#include "core/types.h"
#include "core/delay.h"

#include "libraries/output/ac_phase_control/ac_phase_control.h"
#include "drivers/interrupts/ext_interrupt/ext_interrupt.h"

#if defined(DRV_COMPILER_C18)
#include <p18f452.h>
#endif

#if defined(DRV_COMPILER_XC8)
#include <xc.h>
#endif

/*
 * PIC18F452 multi-channel phase control with optional relay bypass.
 *
 * RB0 / INT0: fake zero-cross input, one edge every 10 ms (100 Hz at 50 Hz
 *             mains, one pulse per half-cycle).
 * RB1:        emergency all-off input (active low).
 * RD0:        channel 0 triac gate.
 * RD1:        channel 1 triac gate.
 * RD2:        channel 2 triac gate.
 * RD3:        channel 3 triac gate.
 * RD4:        channel 0 relay bypass.
 * RD5:        channel 1 relay bypass.
 * RD6:        channel 2 relay bypass.
 * RD7:        unused.
 */

#define MC_CHANNEL_COUNT        4U
#define MC_CH0_SEQUENCE_COUNT   5U
#define MC_ALL_OFF_MS           3000UL

static ac_phase_control_group_t g_phase_group;

static ac_phase_control_channel_t g_phase_channels[MC_CHANNEL_COUNT];

static const ac_phase_control_config_t g_phase_config =
{
    10000U, /* Half-cycle duration for 50 Hz AC. */
    500U,   /* Minimum firing delay. */
    9500U,  /* Maximum firing delay. */
    120U,   /* Gate pulse duration. */
    0u,     /* Relay on threshold (default 98 %). */
    0u,     /* Relay off threshold (default 96 %). */
    0u,     /* Break-before-make (default 50 ms). */
    0u,     /* Minimum relay ON time (default 200 ms). */
    0u,     /* Minimum relay OFF time (default 200 ms). */
    500U    /* Zero-cross timeout (ms): fail-safe all-off. */
};

/*
 * Channel 0 walks through phase levels and a full-power relay transition.
 */
static const uint8_t g_ch0_sequence[MC_CH0_SEQUENCE_COUNT] =
{
    50U,   /* phase control, mid firing delay. */
    97U,   /* phase control, near min delay. */
    98U,   /* relay ON threshold: relay transition. */
    100U,  /* relay full-power mode. */
    50U    /* back to phase after min-ON / break-before-make. */
};

static const uint16_t g_ch0_dwell_ms[MC_CH0_SEQUENCE_COUNT] =
{
    3000U,
    3000U,
    3000U,
    3000U,
    3000U
};

static uint8_t g_ch0_step = 0U;
static uint32_t g_last_step_ms = 0UL;

static uint8_t g_all_off_seen = 0U;
static uint8_t g_all_off_armed = 0U;

static void outputs_all_low(void)
{
    LATD &= (uint8_t)(~0x7FU);
}

static void enter_error_state(void)
{
    INTCONbits.GIE = 0U;
    INTCONbits.PEIE = 0U;

    outputs_all_low();

    while (1)
    {
        LATD ^= 0x7FU;
        DRV_DELAY_MS(150U);
    }
}

static void io_init(void)
{
    ADCON1 = 0x07U;

#if defined(CMCON)
    CMCON = 0x07U;
#endif

    RCONbits.IPEN = 0U;
    TRISEbits.PSPMODE = 0U;

    TRISBbits.TRISB0 = 1U; /* zero-cross input. */
    TRISBbits.TRISB1 = 1U; /* emergency all-off button. */
    INTCON2bits.RBPU = 0U; /* PORTB weak pull-ups. */

    TRISD &= (uint8_t)(~0x7FU); /* RD0..RD6 outputs. */

    outputs_all_low();
}

static drv_status_t attach_channels(void)
{
    drv_status_t status;
    uint8_t channel;

    static const uint8_t gate_mask[MC_CHANNEL_COUNT] =
    {
        (uint8_t)(1u << 0u),
        (uint8_t)(1u << 1u),
        (uint8_t)(1u << 2u),
        (uint8_t)(1u << 3u)
    };

    static const uint8_t relay_mask[MC_CHANNEL_COUNT] =
    {
        (uint8_t)(1u << 4u),
        (uint8_t)(1u << 5u),
        (uint8_t)(1u << 6u),
        0u /* channel 3 has no relay bypass. */
    };

    for (channel = 0u; channel < MC_CHANNEL_COUNT; channel++)
    {
        status = ac_phase_control_attach_channel(
            &g_phase_group,
            channel,
            &LATD,
            &TRISD,
            gate_mask[channel]
        );

        if (status != DRV_STATUS_OK)
        {
            return status;
        }

        if (relay_mask[channel] != 0u)
        {
            status = ac_phase_control_attach_channel_relay(
                &g_phase_group,
                channel,
                &LATD,
                &TRISD,
                relay_mask[channel]
            );

            if (status != DRV_STATUS_OK)
            {
                return status;
            }
        }
    }

    return DRV_STATUS_OK;
}

static void apply_initial_levels(void)
{
    (void)ac_phase_control_set_power_percent(&g_phase_group, 0u, 50u);
    (void)ac_phase_control_set_power_percent(&g_phase_group, 1u, 50u);
    (void)ac_phase_control_set_power_percent(&g_phase_group, 2u, 100u);
    (void)ac_phase_control_set_power_percent(&g_phase_group, 3u, 30u);
}

static drv_status_t phase_control_init(void)
{
    drv_status_t status;

    status = ac_phase_control_init_group(
        &g_phase_group,
        AC_PHASE_CONTROL_TIMER2,
        &g_phase_config,
        g_phase_channels,
        MC_CHANNEL_COUNT
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

    apply_initial_levels();

    return DRV_STATUS_OK;
}

static void zero_cross_cb(void)
{
    ac_phase_control_on_zero_cross(&g_phase_group);
}

static void update_channel_zero(void)
{
    uint32_t now_ms;

    if (g_all_off_armed != 0u)
    {
        return;
    }

    now_ms = ac_phase_control_get_tick_ms(&g_phase_group);

    if ((uint32_t)(now_ms - g_last_step_ms) <
        g_ch0_dwell_ms[g_ch0_step])
    {
        return;
    }

    g_last_step_ms = now_ms;

    g_ch0_step++;

    if (g_ch0_step >= MC_CH0_SEQUENCE_COUNT)
    {
        g_ch0_step = 0U;
    }

    (void)ac_phase_control_set_power_percent(
        &g_phase_group,
        0u,
        g_ch0_sequence[g_ch0_step]
    );
}

static void update_all_off_button(void)
{
    uint8_t pressed;

    pressed = (PORTBbits.RB1 != 0u) ? 0u : 1u;

    if (pressed != 0u)
    {
        if (g_all_off_seen == 0u)
        {
            g_all_off_seen = 1u;
            g_all_off_armed = 1u;
            ac_phase_control_all_off(&g_phase_group);
        }
        return;
    }

    if (g_all_off_seen != 0u)
    {
        g_all_off_seen = 0u;
    }

    if (g_all_off_armed != 0u)
    {
        if ((ac_phase_control_get_tick_ms(&g_phase_group) -
             g_last_step_ms) >= MC_ALL_OFF_MS)
        {
            g_all_off_armed = 0u;
            g_last_step_ms = ac_phase_control_get_tick_ms(&g_phase_group);
            apply_initial_levels();
        }
    }
}

#if defined(DRV_COMPILER_XC8)

void __interrupt() isr(void)
{
    if ((PIE1bits.TMR2IE != 0u) &&
        (PIR1bits.TMR2IF != 0u))
    {
        ac_phase_control_irq_handler();
    }

    ext_interrupt_irq_handler();
}

#endif

#if defined(DRV_COMPILER_C18)

#pragma interrupt high_isr
void high_isr(void)
{
    if ((PIE1bits.TMR2IE != 0u) &&
        (PIR1bits.TMR2IF != 0u))
    {
        ac_phase_control_irq_handler();
    }

    ext_interrupt_irq_handler();
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

    INTCONbits.GIE = 0U;
    INTCONbits.PEIE = 0U;
    INTCONbits.INT0IE = 0U;

    io_init();

    status = phase_control_init();

    if (status != DRV_STATUS_OK)
    {
        enter_error_state();
    }

    g_last_step_ms = 0UL;

    ext_interrupt_init();
    ext_interrupt_set_edge(EXT_INT0, EXT_INT_EDGE_RISING);
    ext_interrupt_set_callback(EXT_INT0, zero_cross_cb);

    PIR1bits.TMR2IF = 0U;
    INTCONbits.INT0IF = 0U;

    INTCONbits.PEIE = 1U;
    ext_interrupt_enable(EXT_INT0);
    INTCONbits.GIE = 1U;

    while (1)
    {
        ac_phase_control_process(&g_phase_group);
        update_channel_zero();
        update_all_off_button();
    }
}
