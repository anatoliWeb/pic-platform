#include "libraries/system/buzzer/buzzer.h"

#include "drivers/gpio/gpio.h"

static void buzzer_apply_hw(buzzer_t* buzzer)
{
    uint8_t high_level;

    if (buzzer == (buzzer_t*)0)
    {
        return;
    }

    high_level = (uint8_t)(buzzer->is_on != 0u ? buzzer->active_high : (uint8_t)(buzzer->active_high == 0u ? 1u : 0u));

    if (high_level != 0u)
    {
        gpio_write_high(buzzer->port, buzzer->pin);
    }
    else
    {
        gpio_write_low(buzzer->port, buzzer->pin);
    }
}

static void buzzer_apply_state(buzzer_t* buzzer)
{
    if (buzzer == (buzzer_t*)0)
    {
        return;
    }

    if ((buzzer->mode == BUZZER_MODE_PASSIVE) &&
        (buzzer->tone_start_cb != (buzzer_tone_start_cb_t)0) &&
        (buzzer->tone_stop_cb != (buzzer_tone_stop_cb_t)0))
    {
        if (buzzer->is_on != 0u)
        {
            buzzer->tone_start_cb(buzzer->tone_ctx, buzzer->current_frequency_hz);
        }
        else
        {
            buzzer->tone_stop_cb(buzzer->tone_ctx);
        }
        return;
    }

    /* Active buzzer or passive fallback without PWM callbacks. */
    buzzer_apply_hw(buzzer);
}

static void buzzer_start_step(buzzer_t* buzzer, uint32_t now_ms)
{
    const buzzer_pattern_step_t* step;

    if ((buzzer == (buzzer_t*)0) || (buzzer->pattern == (const buzzer_pattern_step_t*)0) || (buzzer->pattern_len == 0u))
    {
        return;
    }

    if (buzzer->pattern_index >= buzzer->pattern_len)
    {
        if (buzzer->pattern_repeat == 0u)
        {
            buzzer->pattern_running = 0u;
            buzzer_off(buzzer);
            return;
        }
        buzzer->pattern_index = 0u;
    }

    step = &buzzer->pattern[buzzer->pattern_index];
    buzzer->current_frequency_hz = (step->frequency_hz == 0u) ? 2000u : step->frequency_hz;
    buzzer->is_on = (uint8_t)(step->state != 0u ? 1u : 0u);
    buzzer_apply_state(buzzer);

    buzzer->pattern_step_end_ms = (uint32_t)(now_ms + step->duration_ms);
}

uint8_t buzzer_init(buzzer_t* buzzer,
                    volatile uint8_t* port,
                    volatile uint8_t* tris,
                    uint8_t pin,
                    buzzer_mode_t mode,
                    uint8_t active_high)
{
    if ((buzzer == (buzzer_t*)0) || (port == (volatile uint8_t*)0) || (tris == (volatile uint8_t*)0))
    {
        return 0u;
    }

    buzzer->port = port;
    buzzer->tris = tris;
    buzzer->pin = pin;
    buzzer->mode = mode;
    buzzer->active_high = (uint8_t)(active_high != 0u ? 1u : 0u);

    buzzer->is_on = 0u;
    buzzer->timed_active = 0u;
    buzzer->timed_end_ms = 0u;

    buzzer->pattern = (const buzzer_pattern_step_t*)0;
    buzzer->pattern_len = 0u;
    buzzer->pattern_index = 0u;
    buzzer->pattern_repeat = 0u;
    buzzer->pattern_running = 0u;
    buzzer->pattern_step_end_ms = 0u;

    buzzer->current_frequency_hz = 2000u;

    buzzer->tone_start_cb = (buzzer_tone_start_cb_t)0;
    buzzer->tone_stop_cb = (buzzer_tone_stop_cb_t)0;
    buzzer->tone_ctx = (void*)0;

    gpio_set_output(buzzer->tris, buzzer->pin);
    buzzer_apply_hw(buzzer);

    return 1u;
}

void buzzer_on(buzzer_t* buzzer)
{
    if (buzzer == (buzzer_t*)0)
    {
        return;
    }

    buzzer->timed_active = 0u;
    buzzer->pattern_running = 0u;
    buzzer->is_on = 1u;
    buzzer_apply_state(buzzer);
}

void buzzer_off(buzzer_t* buzzer)
{
    if (buzzer == (buzzer_t*)0)
    {
        return;
    }

    buzzer->timed_active = 0u;
    buzzer->pattern_running = 0u;
    buzzer->is_on = 0u;
    buzzer_apply_state(buzzer);
}

void buzzer_toggle(buzzer_t* buzzer)
{
    if (buzzer == (buzzer_t*)0)
    {
        return;
    }

    buzzer->timed_active = 0u;
    buzzer->pattern_running = 0u;
    buzzer->is_on = (uint8_t)(buzzer->is_on == 0u ? 1u : 0u);
    buzzer_apply_state(buzzer);
}

void buzzer_beep(buzzer_t* buzzer, uint16_t duration_ms, uint32_t now_ms)
{
    if (buzzer == (buzzer_t*)0)
    {
        return;
    }

    buzzer->pattern_running = 0u;
    buzzer->is_on = 1u;
    buzzer_apply_state(buzzer);

    buzzer->timed_active = 1u;
    buzzer->timed_end_ms = (uint32_t)(now_ms + duration_ms);
}

void buzzer_update(buzzer_t* buzzer, uint32_t now_ms)
{
    if (buzzer == (buzzer_t*)0)
    {
        return;
    }

    if ((buzzer->pattern_running != 0u) && ((int32_t)(now_ms - buzzer->pattern_step_end_ms) >= 0))
    {
        buzzer->pattern_index++;
        buzzer_start_step(buzzer, now_ms);
    }

    if ((buzzer->timed_active != 0u) && ((int32_t)(now_ms - buzzer->timed_end_ms) >= 0))
    {
        buzzer->timed_active = 0u;
        buzzer->is_on = 0u;
        buzzer_apply_state(buzzer);
    }
}

void buzzer_set_tone_backend(buzzer_t* buzzer,
                             buzzer_tone_start_cb_t start_cb,
                             buzzer_tone_stop_cb_t stop_cb,
                             void* tone_ctx)
{
    if (buzzer == (buzzer_t*)0)
    {
        return;
    }

    buzzer->tone_start_cb = start_cb;
    buzzer->tone_stop_cb = stop_cb;
    buzzer->tone_ctx = tone_ctx;
}

void buzzer_play_tone(buzzer_t* buzzer, uint16_t frequency_hz)
{
    if (buzzer == (buzzer_t*)0)
    {
        return;
    }

    buzzer->current_frequency_hz = (frequency_hz == 0u) ? 2000u : frequency_hz;
    buzzer->timed_active = 0u;
    buzzer->pattern_running = 0u;
    buzzer->is_on = 1u;
    buzzer_apply_state(buzzer);
}

void buzzer_play_pattern(buzzer_t* buzzer,
                         const buzzer_pattern_step_t* pattern,
                         uint8_t pattern_len,
                         uint8_t repeat,
                         uint32_t now_ms)
{
    if ((buzzer == (buzzer_t*)0) || (pattern == (const buzzer_pattern_step_t*)0) || (pattern_len == 0u))
    {
        return;
    }

    buzzer->pattern = pattern;
    buzzer->pattern_len = pattern_len;
    buzzer->pattern_index = 0u;
    buzzer->pattern_repeat = (uint8_t)(repeat != 0u ? 1u : 0u);
    buzzer->pattern_running = 1u;
    buzzer->timed_active = 0u;

    buzzer_start_step(buzzer, now_ms);
}

void buzzer_stop(buzzer_t* buzzer)
{
    buzzer_off(buzzer);
}
