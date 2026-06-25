/*
 * File: examples-projects/c18/pwm.X/main.c
 */

#include "project_config.h"
#include "core/compiler.h"
#include "core/delay.h"
#include "drivers/timers/pwm/pwm.h"

void main(void)
{
    uint16_t duty = 0u;

    pwm_init(1000u);
    pwm_start(PWM_CHANNEL_1);

    while (1)
    {
        pwm_set_duty(PWM_CHANNEL_1, duty);
        duty += 128u;

        if (duty > 1023u)
        {
            duty = 0u;
        }

        DRV_DELAY_MS(200);
    }
}

