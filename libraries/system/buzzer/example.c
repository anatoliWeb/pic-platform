/*
 * File: libraries/system/buzzer/example.c
 */

#include "core/compiler.h"
#include "drivers/timers/tick/tick.h"
#include "libraries/system/buzzer/buzzer.h"

/* Optional PWM backend for passive buzzers. */
#include "drivers/timers/pwm/pwm.h"

static buzzer_t g_active_buzzer;
static buzzer_t g_passive_buzzer;

static void passive_tone_start(void* ctx, uint16_t frequency_hz)
{
    DRV_UNUSED(ctx);

    pwm_init(frequency_hz);
    pwm_set_duty(PWM_CHANNEL_1, 512u);
    pwm_start(PWM_CHANNEL_1);
}

static void passive_tone_stop(void* ctx)
{
    DRV_UNUSED(ctx);

    pwm_stop(PWM_CHANNEL_1);
}

void main(void)
{
    static const buzzer_pattern_step_t pattern[] =
    {
        { 80u, 1u, 2400u },
        { 80u, 0u, 0u    },
        { 80u, 1u, 2400u },
        { 300u, 0u, 0u   }
    };

    uint32_t now;

    /* Active buzzer on RB0, active high. */
    (void)buzzer_init(&g_active_buzzer, &PORTB, &TRISB, 0u, BUZZER_MODE_ACTIVE, 1u);

    /* Passive buzzer on RB1 with PWM callback backend. */
    (void)buzzer_init(&g_passive_buzzer, &PORTB, &TRISB, 1u, BUZZER_MODE_PASSIVE, 1u);
    buzzer_set_tone_backend(&g_passive_buzzer, passive_tone_start, passive_tone_stop, (void*)0);

    now = tick_get();

    /* Non-blocking single beep. */
    buzzer_beep(&g_active_buzzer, 100u, now);

    /* Non-blocking repeating pattern for passive mode. */
    buzzer_play_pattern(&g_passive_buzzer, pattern, (uint8_t)(sizeof(pattern) / sizeof(pattern[0])), 1u, now);

    while (1)
    {
        now = tick_get();

        /* Call from loop, scheduler task, or timer hook. */
        buzzer_update(&g_active_buzzer, now);
        buzzer_update(&g_passive_buzzer, now);
    }
}
