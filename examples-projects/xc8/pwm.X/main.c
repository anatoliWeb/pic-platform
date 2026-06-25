/*
 * File: examples-projects/xc8/pwm.X/main.c
 */

#include "project_config.h"

#include "core/compiler.h"
#include "core/delay.h"

#include "drivers/timers/pwm/pwm.h"

#define PWM_MAX_DUTY   1023u
#define PWM_STEP       64u
#define PWM_DELAY_MS   80u

void main(void)
{
    /*
     * PIC18F452 PWM mirror fade test.
     *
     * PWM_CHANNEL_1:
     *   CCP1 output
     *   RC2 / DIP-40 pin 17
     *
     * PWM_CHANNEL_2:
     *   CCP2 output
     *   RC1 / DIP-40 pin 16
     *
     * Proteus:
     *   RC2 / pin 17 -> 330R -> LED1 -> GND
     *   RC1 / pin 16 -> 330R -> LED2 -> GND
     *
     * Oscilloscope:
     *   RC2 / pin 17 -> CH-A
     *   RC1 / pin 16 -> CH-B
     *
     * Behavior:
     *   Channel 1 fades up while Channel 2 fades down.
     *   Then Channel 1 fades down while Channel 2 fades up.
     *
     * PWM does not require global interrupts.
     */

    uint16_t duty = 0u;
    uint8_t direction_up = 1u;

    /*
     * If pwm.c already configures RC2 and RC1 as outputs,
     * these lines are not strictly required.
     * They are kept here to make the example self-checking.
     */
    TRISCbits.TRISC2 = 0u;  /* CCP1 / PWM_CHANNEL_1 */
    TRISCbits.TRISC1 = 0u;  /* CCP2 / PWM_CHANNEL_2 */

    /*
     * Initialize PWM at 1 kHz.
     */
    pwm_init(1000u);

    /*
     * Start both PWM channels.
     */
    pwm_start(PWM_CHANNEL_1);
    pwm_start(PWM_CHANNEL_2);

    while (1)
    {
        /*
         * Mirror output:
         *
         * Channel 1 = duty
         * Channel 2 = inverted duty
         *
         * Example:
         *   duty = 0      -> CH1 0%,   CH2 100%
         *   duty = 512    -> CH1 50%,  CH2 50%
         *   duty = 1023   -> CH1 100%, CH2 0%
         */
        pwm_set_duty(PWM_CHANNEL_1, duty);
        pwm_set_duty(PWM_CHANNEL_2, (uint16_t)(PWM_MAX_DUTY - duty));

        /*
         * Move duty up and down.
         */
        if (direction_up != 0u)
        {
            if (duty >= (PWM_MAX_DUTY - PWM_STEP))
            {
                duty = PWM_MAX_DUTY;
                direction_up = 0u;
            }
            else
            {
                duty = (uint16_t)(duty + PWM_STEP);
            }
        }
        else
        {
            if (duty <= PWM_STEP)
            {
                duty = 0u;
                direction_up = 1u;
            }
            else
            {
                duty = (uint16_t)(duty - PWM_STEP);
            }
        }

        /*
         * Delay controls fade speed.
         */
        DRV_DELAY_MS(PWM_DELAY_MS);
    }
}