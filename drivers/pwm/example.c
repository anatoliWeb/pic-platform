#include "core/compiler.h"
#include "core/delay.h"
#include "drivers/pwm/pwm.h"

static void pwm_example_led_dimming(void)
{
    uint16_t duty;

    for (duty = 0u; duty <= 1023u; duty += 64u)
    {
        pwm_set_duty(PWM_CHANNEL_1, duty);
        DRV_DELAY_MS(20);
    }

    for (duty = 1023u; duty > 0u; duty -= 64u)
    {
        pwm_set_duty(PWM_CHANNEL_1, duty);
        DRV_DELAY_MS(20);
    }
}

static void pwm_example_fan_speed(void)
{
    pwm_set_duty(PWM_CHANNEL_2, 256u);
    DRV_DELAY_MS(500);

    pwm_set_duty(PWM_CHANNEL_2, 512u);
    DRV_DELAY_MS(500);

    pwm_set_duty(PWM_CHANNEL_2, 900u);
    DRV_DELAY_MS(500);
}

void main(void)
{
    pwm_init(1000u);
    pwm_start(PWM_CHANNEL_1);
    pwm_start(PWM_CHANNEL_2);

    while (1)
    {
        pwm_example_led_dimming();
        pwm_example_fan_speed();
    }
}
