/*
 * File: libraries/system/buzzer/buzzer.h
 */

#ifndef LIBRARIES_SYSTEM_BUZZER_H
#define LIBRARIES_SYSTEM_BUZZER_H

#include "core/compiler.h"
#include "core/types.h"

typedef enum
{
    BUZZER_MODE_ACTIVE = 0,
    BUZZER_MODE_PASSIVE
} buzzer_mode_t;

typedef void (*buzzer_tone_start_cb_t)(void* ctx, uint16_t frequency_hz);
typedef void (*buzzer_tone_stop_cb_t)(void* ctx);

typedef struct
{
    uint16_t duration_ms;
    uint8_t state;
    uint16_t frequency_hz;
} buzzer_pattern_step_t;

typedef struct
{
    volatile uint8_t* port;
    volatile uint8_t* tris;
    uint8_t pin;

    buzzer_mode_t mode;
    uint8_t active_high;

    uint8_t is_on;

    uint8_t timed_active;
    uint32_t timed_end_ms;

    const buzzer_pattern_step_t* pattern;
    uint8_t pattern_len;
    uint8_t pattern_index;
    uint8_t pattern_repeat;
    uint8_t pattern_running;
    uint32_t pattern_step_end_ms;

    uint16_t current_frequency_hz;

    buzzer_tone_start_cb_t tone_start_cb;
    buzzer_tone_stop_cb_t tone_stop_cb;
    void* tone_ctx;
} buzzer_t;

uint8_t buzzer_init(buzzer_t* buzzer,
                    volatile uint8_t* port,
                    volatile uint8_t* tris,
                    uint8_t pin,
                    buzzer_mode_t mode,
                    uint8_t active_high);

void buzzer_on(buzzer_t* buzzer);
void buzzer_off(buzzer_t* buzzer);
void buzzer_toggle(buzzer_t* buzzer);

void buzzer_beep(buzzer_t* buzzer, uint16_t duration_ms, uint32_t now_ms);
void buzzer_update(buzzer_t* buzzer, uint32_t now_ms);

void buzzer_set_tone_backend(buzzer_t* buzzer,
                             buzzer_tone_start_cb_t start_cb,
                             buzzer_tone_stop_cb_t stop_cb,
                             void* tone_ctx);

void buzzer_play_tone(buzzer_t* buzzer, uint16_t frequency_hz);
void buzzer_play_pattern(buzzer_t* buzzer,
                         const buzzer_pattern_step_t* pattern,
                         uint8_t pattern_len,
                         uint8_t repeat,
                         uint32_t now_ms);
void buzzer_stop(buzzer_t* buzzer);

#endif /* LIBRARIES_SYSTEM_BUZZER_H */
