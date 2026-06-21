#include "project_config.h"

#include "core/compiler.h"
#include "core/types.h"

#include "drivers/timers/timer2/timer2.h"
#include "libraries/output/software_pwm/software_pwm.h"

#if defined(DRV_COMPILER_C18)
#include <p18f452.h>
#endif

#if defined(DRV_COMPILER_XC8)
#include <xc.h>
#endif

/*
 * PIC18F452 software PWM example.
 *
 * Purpose:
 *   Drive four LED channels from one timer-driven software PWM group.
 *   Duty values are changed periodically in the main loop.
 *
 * Proteus wiring:
 *   RB0 / pin 33 -> LED0 -> 330R -> GND
 *   RB1 / pin 34 -> LED1 -> 330R -> GND
 *   RB2 / pin 35 -> LED2 -> 330R -> GND
 *   RB3 / pin 36 -> LED3 -> 330R -> GND
 *
 * Expected:
 *   Four LEDs change brightness in a repeating pattern.
 *   All LEDs use the same PWM frequency because they belong to one PWM group.
 *   Only duty cycle is changed per channel.
 */

#define SOFTWARE_PWM_RESOLUTION        10u
#define SOFTWARE_PWM_CHANNELS          4u

/*
 * This value depends on Timer2 interrupt speed.
 * Increase it if brightness changes too fast.
 * Decrease it if brightness changes too slowly.
 */
#define SOFTWARE_PWM_UPDATE_INTERVAL   90000u

static software_pwm_channel_t g_pwm_channels[SOFTWARE_PWM_CHANNELS] =
{
    { &LATB, &TRISB, (uint8_t)(1u << 0u), 1u, 1u },
    { &LATB, &TRISB, (uint8_t)(1u << 1u), 3u, 1u },
    { &LATB, &TRISB, (uint8_t)(1u << 2u), 6u, 1u },
    { &LATB, &TRISB, (uint8_t)(1u << 3u), 9u, 1u }
};

static software_pwm_group_t g_pwm_group;

/*
 * Duty pattern table.
 *
 * Values are in range:
 *   0  = always OFF
 *   10 = always ON because resolution is 10
 */
static const uint16_t g_duty_patterns[][SOFTWARE_PWM_CHANNELS] =
{
    { 1u, 3u, 6u, 9u },
    { 3u, 6u, 9u, 1u },
    { 6u, 9u, 1u, 3u },
    { 9u, 1u, 3u, 6u },
    { 0u, 2u, 5u, 10u },
    { 10u, 5u, 2u, 0u }
};

#define SOFTWARE_PWM_PATTERN_COUNT \
    ((uint8_t)(sizeof(g_duty_patterns) / sizeof(g_duty_patterns[0])))

static volatile uint16_t g_update_counter = 0u;
static volatile uint8_t g_update_request = 0u;

#if defined(DRV_COMPILER_XC8)
void __interrupt() isr(void)
{
    timer2_irq_handler();
}
#endif

#if defined(DRV_COMPILER_C18)
#pragma interrupt high_isr
void high_isr(void)
{
    timer2_irq_handler();
}

#pragma code high_vector = 0x08
void high_vector(void)
{
    _asm goto high_isr _endasm
}
#pragma code
#endif

static void software_pwm_timer_callback(void)
{
    /*
     * Keep the interrupt callback short and deterministic.
     * PWM output is updated on every Timer2 interrupt.
     */
    software_pwm_tick(&g_pwm_group);

    /*
     * Request a duty-pattern update from the main loop.
     * Do not change all duties inside the ISR.
     */
    g_update_counter++;

    if (g_update_counter >= SOFTWARE_PWM_UPDATE_INTERVAL)
    {
        g_update_counter = 0u;
        g_update_request = 1u;
    }
}

static void software_pwm_apply_pattern(uint8_t pattern_index)
{
    uint8_t i;

    /*
     * Duty is 16-bit, so update it with interrupts disabled
     * to avoid partial read/write while the ISR is running.
     */
    INTCONbits.GIE = 0u;

    for (i = 0u; i < SOFTWARE_PWM_CHANNELS; i++)
    {
        software_pwm_set_duty(&g_pwm_group, i, g_duty_patterns[pattern_index][i]);
    }

    INTCONbits.GIE = 1u;
}

void main(void)
{
    uint8_t current_pattern = 0u;

    software_pwm_init_group(&g_pwm_group,
                            SOFTWARE_PWM_TIMER2,
                            SOFTWARE_PWM_RESOLUTION,
                            g_pwm_channels,
                            SOFTWARE_PWM_CHANNELS);

    /*
     * Prescaler 4 gives a practical Timer2 interrupt rate for a
     * visible LED brightness demonstration at 10 MHz.
     */
    timer2_init(4u);
    timer2_set_callback(software_pwm_timer_callback);
    timer2_enable_interrupt();
    timer2_start();

    INTCONbits.PEIE = 1u;
    INTCONbits.GIE  = 1u;

    while (1)
    {
        if (g_update_request != 0u)
        {
            g_update_request = 0u;

            current_pattern++;

            if (current_pattern >= SOFTWARE_PWM_PATTERN_COUNT)
            {
                current_pattern = 0u;
            }

            software_pwm_apply_pattern(current_pattern);
        }
    }
}