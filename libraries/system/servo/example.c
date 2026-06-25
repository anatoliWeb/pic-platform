/*
 * File: libraries/system/servo/example.c
 */

#include "core/compiler.h"
#include "drivers/timers/tick/tick.h"
#include "drivers/timers/pwm/pwm.h"
#include "libraries/system/servo/servo.h"

typedef struct
{
    pwm_channel_t channel;
    uint32_t base_freq_hz;
} pwm_servo_ctx_t;

static servo_t g_servo1;
static servo_t g_servo2;

static pwm_servo_ctx_t g_ctx1 = { PWM_CHANNEL_1, 50u };
static pwm_servo_ctx_t g_ctx2 = { PWM_CHANNEL_2, 50u };

static uint16_t pulse_us_to_duty10(uint16_t pulse_us, uint16_t period_us)
{
    uint32_t duty;

    if (period_us == 0u)
    {
        return 0u;
    }

    duty = ((uint32_t)pulse_us * 1023u) / (uint32_t)period_us;
    if (duty > 1023u)
    {
        duty = 1023u;
    }

    return (uint16_t)duty;
}

static void servo_pwm_apply(void* ctx, uint16_t period_us, uint16_t pulse_us)
{
    pwm_servo_ctx_t* p = (pwm_servo_ctx_t*)ctx;
    uint16_t duty;

    if (p == (pwm_servo_ctx_t*)0)
    {
        return;
    }

    /* This uses shared PWM setup for demo purposes. */
    pwm_init(p->base_freq_hz);

    duty = pulse_us_to_duty10(pulse_us, period_us);
    pwm_set_duty(p->channel, duty);
}

static void servo_pwm_enable(void* ctx)
{
    pwm_servo_ctx_t* p = (pwm_servo_ctx_t*)ctx;

    if (p == (pwm_servo_ctx_t*)0)
    {
        return;
    }

    pwm_start(p->channel);
}

static void servo_pwm_disable(void* ctx)
{
    pwm_servo_ctx_t* p = (pwm_servo_ctx_t*)ctx;

    if (p == (pwm_servo_ctx_t*)0)
    {
        return;
    }

    pwm_stop(p->channel);
}

void main(void)
{
    servo_config_t cfg;
    uint32_t now;
    uint32_t last_sweep_ms;
    uint8_t sweep_dir;

    cfg.pulse_min_us = 1000u;
    cfg.pulse_max_us = 2000u;
    cfg.period_us = 20000u;
    cfg.angle_min_deg = 0u;
    cfg.angle_max_deg = 180u;

    (void)servo_init(&g_servo1, &cfg);
    (void)servo_init(&g_servo2, &cfg);

    servo_set_backend(&g_servo1, servo_pwm_apply, servo_pwm_enable, servo_pwm_disable, &g_ctx1);
    servo_set_backend(&g_servo2, servo_pwm_apply, servo_pwm_enable, servo_pwm_disable, &g_ctx2);

    servo_enable(&g_servo1);
    servo_enable(&g_servo2);

    /* Basic direct positioning. */
    servo_set_angle(&g_servo1, 90u);

    /* Smooth move demo. */
    now = tick_get();
    servo_move_smooth(&g_servo1, 150u, 2u, 20u, now);

    /* Multi-servo setup with second channel. */
    servo_set_angle(&g_servo2, 45u);

    last_sweep_ms = now;
    sweep_dir = 0u;

    while (1)
    {
        now = tick_get();

        servo_update(&g_servo1, now);
        servo_update(&g_servo2, now);

        /* Periodic sweep command without blocking delays. */
        if ((now - last_sweep_ms) >= 3000u)
        {
            last_sweep_ms = now;
            if (sweep_dir == 0u)
            {
                servo_move_smooth(&g_servo2, 160u, 1u, 25u, now);
                sweep_dir = 1u;
            }
            else
            {
                servo_move_smooth(&g_servo2, 20u, 1u, 25u, now);
                sweep_dir = 0u;
            }
        }
    }
}
