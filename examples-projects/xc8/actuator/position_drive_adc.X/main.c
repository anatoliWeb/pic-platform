/*
 * File: examples-projects/xc8/actuator/position_drive_adc.X/main.c
 */

#include "project_config.h"

#include "core/compiler.h"
#include "core/delay.h"

#include "drivers/communication/uart/uart.h"
#include "libraries/system/uart_debug/uart_debug.h"

#include "drivers/timers/tick/tick.h"
#include "drivers/timers/timer1/timer1.h"

#include "drivers/analog/adc/adc.h"
#include "drivers/gpio/gpio.h"

#include "libraries/actuator/position_drive/position_drive.h"

#if defined(DRV_COMPILER_C18)
#include <p18f452.h>
#endif

#if defined(DRV_COMPILER_XC8)
#include <xc.h>
#endif

/*
 * PIC18F452 DIP-40
 *
 * Closed-loop position drive with a DC gear motor and a potentiometer
 * used as the position sensor (ADC backend).
 *
 * Wiring:
 *
 *   Potentiometer wiper -> RA0/AN0, DIP pin 2
 *   Potentiometer ends  -> +5V and GND
 *
 *   H-bridge IN1 -> RD0, DIP pin 19
 *   H-bridge IN2 -> RD1, DIP pin 20
 *
 *   UART TX -> RC6, DIP pin 25 -> Proteus Virtual Terminal RXD
 *   9600 baud, 8 data bits, no parity, 1 stop bit
 *
 * Timer ownership:
 *
 *   Timer1 -> tick library
 *
 * The position drive is non-blocking. position_drive_process() must be
 * called regularly from the main loop.
 *
 * The following files must be present in the MPLAB project:
 *
 *   timer1.c
 *   tick.c
 *   adc.c
 *   gpio.c
 *   uart.c
 *   uart_debug.c
 *   position_drive.c
 *   delay.c
 */

#define APP_ADC_CHANNEL           0U

#define APP_RAW_MIN               0U
#define APP_RAW_MAX               1023U
#define APP_ANGLE_MIN             0
#define APP_ANGLE_MAX             270
#define APP_TOLERANCE_DEG         2U
#define APP_MOVE_TIMEOUT_MS       5000U
#define APP_STUCK_TIMEOUT_MS      1000U
#define APP_STUCK_DELTA_RAW       4U

#define APP_MOTOR_FORWARD_PIN     0U
#define APP_MOTOR_REVERSE_PIN     1U
#define APP_MOTOR_TRIS            TRISD
#define APP_MOTOR_LAT             LATD

#define APP_MAIN_LOOP_DELAY_MS    1U
#define APP_UART_BAUD_RATE        9600UL

#define APP_TARGET_1_DEG          30
#define APP_TARGET_2_DEG          120

static position_drive_t g_drive;

static drv_status_t app_read_raw(void* context, uint16_t* raw_value)
{
    uint16_t value;

    (void)context;

    if (raw_value == (uint16_t*)0)
    {
        return DRV_STATUS_ERROR;
    }

    value = adc_read(APP_ADC_CHANNEL);

    if (value > APP_RAW_MAX)
    {
        value = APP_RAW_MAX;
    }

    *raw_value = value;
    return DRV_STATUS_OK;
}

static uint32_t app_get_tick(void* context)
{
    (void)context;
    return tick_get();
}

static void app_motor(void* context, position_drive_direction_t direction)
{
    (void)context;

    switch (direction)
    {
    case POSITION_DRIVE_DIRECTION_FORWARD:
        gpio_write_high(&APP_MOTOR_LAT, APP_MOTOR_FORWARD_PIN);
        gpio_write_low(&APP_MOTOR_LAT, APP_MOTOR_REVERSE_PIN);
        break;

    case POSITION_DRIVE_DIRECTION_REVERSE:
        gpio_write_low(&APP_MOTOR_LAT, APP_MOTOR_FORWARD_PIN);
        gpio_write_high(&APP_MOTOR_LAT, APP_MOTOR_REVERSE_PIN);
        break;

    default:
        gpio_write_low(&APP_MOTOR_LAT, APP_MOTOR_FORWARD_PIN);
        gpio_write_low(&APP_MOTOR_LAT, APP_MOTOR_REVERSE_PIN);
        break;
    }
}

#if (POSITION_DRIVE_ENABLE_UART_DEBUG == 1)
static void app_debug(void* context, const char* message)
{
    (void)context;

    if (message != (const char*)0)
    {
        DBG_PRINTLN(message);
    }
}
#endif

static void app_print_state(void)
{
    DBG_PRINT("STATE deg=");
    DBG_PRINT_INT((int)position_drive_get_current_deg(&g_drive));
    DBG_PRINT(" target=");
    DBG_PRINT_INT((int)position_drive_get_target_deg(&g_drive));
    DBG_PRINT(" raw=");
    DBG_PRINT_INT((int)position_drive_get_current_raw(&g_drive));
    DBG_PRINT(" state=");
    DBG_PRINT_INT((int)position_drive_get_state(&g_drive));
    DBG_PRINT(" error=");
    DBG_PRINT_INT((int)position_drive_get_error(&g_drive));
    DBG_PRINTLN("");
}

static void app_move_and_wait(int16_t target_deg)
{
    drv_status_t status;

    status = position_drive_move_to_deg(&g_drive, target_deg);

    DBG_PRINT("MOVE TO ");
    DBG_PRINT_INT((int)target_deg);
    DBG_PRINT(" status=");
    DBG_PRINT_INT((int)status);
    DBG_PRINTLN("");

    while (position_drive_get_state(&g_drive) == POSITION_DRIVE_STATE_MOVING)
    {
        position_drive_process(&g_drive);
        delay_ms(APP_MAIN_LOOP_DELAY_MS);
    }

    app_print_state();
}

/*
 * Timer1 interrupt is owned by the tick library.
 */
#if defined(DRV_COMPILER_XC8)

void __interrupt() app_isr(void)
{
    timer1_irq_handler();
}

#endif

#if defined(DRV_COMPILER_C18)

#pragma code high_vector = 0x08

void app_high_vector(void)
{
    _asm
        goto app_high_isr
    _endasm
}

#pragma code

#pragma interrupt app_high_isr

void app_high_isr(void)
{
    timer1_irq_handler();
}

#endif

void main(void)
{
    position_drive_config_t cfg;
    drv_status_t status;

    /*
     * Disable interrupt gates during initialization.
     */
    INTCONbits.GIE = 0U;
    INTCONbits.PEIE = 0U;

    /*
     * Use the single interrupt vector.
     */
    RCONbits.IPEN = 0U;

    /*
     * Configure RA0 as analog input. The remaining analog-capable pins
     * stay digital.
     */
    ADCON1 = 0x0EU;

#if defined(CMCON)
    /*
     * Disable analog comparators.
     */
    CMCON = 0x07U;
#endif

    /*
     * Disable Parallel Slave Port mode before using PORTD.
     */
    TRISEbits.PSPMODE = 0U;

    /*
     * Safe initial motor state: both H-bridge inputs LOW.
     */
    LATD = 0x00U;
    TRISD = 0x00U;

    uart_init(APP_UART_BAUD_RATE);

    DBG_PRINTLN("");
    DBG_PRINTLN("PIC18F452 position drive ADC UART debug");
    DBG_PRINTLN("BOOT");

    adc_init();
    tick_init();

    cfg.get_tick_cb = app_get_tick;
    cfg.read_raw_cb = app_read_raw;
    cfg.motor_cb = app_motor;
    cfg.set_speed_cb = (position_drive_set_speed_cb_t)0;
#if (POSITION_DRIVE_ENABLE_UART_DEBUG == 1)
    cfg.debug_cb = app_debug;
#else
    cfg.debug_cb = (position_drive_debug_cb_t)0;
#endif
    cfg.context = (void*)0;

    cfg.sensor_raw_min = APP_RAW_MIN;
    cfg.sensor_raw_max = APP_RAW_MAX;
    cfg.angle_min_deg = APP_ANGLE_MIN;
    cfg.angle_max_deg = APP_ANGLE_MAX;
    cfg.target_tolerance_deg = APP_TOLERANCE_DEG;
    cfg.move_timeout_ms = APP_MOVE_TIMEOUT_MS;
    cfg.stuck_timeout_ms = APP_STUCK_TIMEOUT_MS;
    cfg.stuck_min_delta_raw = APP_STUCK_DELTA_RAW;
    cfg.direction_inverted = 0U;
    cfg.speed_min_percent = 50U;
    cfg.speed_max_percent = 100U;
    cfg.speed_default_percent = 80U;

    status = position_drive_init(&g_drive, &cfg);

    DBG_PRINT("POSITION DRIVE INIT status=");
    DBG_PRINT_INT((int)status);
    DBG_PRINTLN("");

    if (status != DRV_STATUS_OK)
    {
        DBG_PRINTLN("POSITION DRIVE INIT FAILED");
        while (1)
        {
            /*
             * Fatal error.
             */
        }
    }

    PIR1bits.TMR1IF = 0U;

    INTCONbits.PEIE = 1U;
    INTCONbits.GIE = 1U;

    DBG_PRINTLN("TICK STARTED");
    DBG_PRINTLN("Move sequence: 30 deg -> 120 deg -> loop");

    while (1)
    {
        position_drive_process(&g_drive);

        app_move_and_wait(APP_TARGET_1_DEG);
        app_move_and_wait(APP_TARGET_2_DEG);

        delay_ms(500U);
    }
}
