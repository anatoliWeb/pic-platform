#include "core/compiler.h"
#include "core/delay.h"
#include "drivers/timers/pwm/pwm.h"
#include "drivers/timers/timer2/timer2.h"

static void timer2_update_pwm_cb(void)
{
    static uint16_t duty = 0u;

    pwm_set_duty(PWM_CHANNEL_1, duty);
    duty += 64u;
    if (duty > 1023u)
    {
        duty = 0u;
    }
}

void main(void)
{
    pwm_init(1000u);
    pwm_start(PWM_CHANNEL_1);

    timer2_init(16u);
    timer2_set_callback(timer2_update_pwm_cb);
    timer2_enable_interrupt();
    timer2_start();

    while (1)
    {
    }
}
