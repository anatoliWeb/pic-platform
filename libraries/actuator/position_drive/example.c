/*
 * File: libraries/actuator/position_drive/example.c
 */

#include "core/compiler.h"
#include "drivers/analog/adc/adc.h"
#include "drivers/timers/tick/tick.h"
#include "drivers/gpio/gpio.h"
#include "libraries/actuator/position_drive/position_drive.h"

#define EXAMPLE_ADC_CHANNEL      0U
#define EXAMPLE_RAW_MIN          0U
#define EXAMPLE_RAW_MAX          1023U
#define EXAMPLE_ANGLE_MIN        0
#define EXAMPLE_ANGLE_MAX        270
#define EXAMPLE_TOLERANCE_DEG    2U
#define EXAMPLE_MOVE_TIMEOUT_MS  5000U
#define EXAMPLE_STUCK_TIMEOUT_MS 1000U
#define EXAMPLE_STUCK_DELTA_RAW  4U

#define EXAMPLE_MOTOR_FORWARD_PIN  0U
#define EXAMPLE_MOTOR_REVERSE_PIN  1U
#define EXAMPLE_MOTOR_TRIS         TRISD
#define EXAMPLE_MOTOR_LAT          LATD

#define EXAMPLE_TARGET_1_DEG      30
#define EXAMPLE_TARGET_2_DEG      120

static position_drive_t g_drive;

static drv_status_t example_read_raw(void* context, uint16_t* raw_value)
{
    uint16_t value;

    (void)context;

    if (raw_value == (uint16_t*)0)
    {
        return DRV_STATUS_ERROR;
    }

    value = adc_read(EXAMPLE_ADC_CHANNEL);

    if (value > EXAMPLE_RAW_MAX)
    {
        value = EXAMPLE_RAW_MAX;
    }

    *raw_value = value;
    return DRV_STATUS_OK;
}

static uint32_t example_get_tick(void* context)
{
    (void)context;
    return tick_get();
}

static void example_motor(void* context, position_drive_direction_t direction)
{
    (void)context;

    switch (direction)
    {
    case POSITION_DRIVE_DIRECTION_FORWARD:
        gpio_write_high(&EXAMPLE_MOTOR_LAT, EXAMPLE_MOTOR_FORWARD_PIN);
        gpio_write_low(&EXAMPLE_MOTOR_LAT, EXAMPLE_MOTOR_REVERSE_PIN);
        break;

    case POSITION_DRIVE_DIRECTION_REVERSE:
        gpio_write_low(&EXAMPLE_MOTOR_LAT, EXAMPLE_MOTOR_FORWARD_PIN);
        gpio_write_high(&EXAMPLE_MOTOR_LAT, EXAMPLE_MOTOR_REVERSE_PIN);
        break;

    default:
        gpio_write_low(&EXAMPLE_MOTOR_LAT, EXAMPLE_MOTOR_FORWARD_PIN);
        gpio_write_low(&EXAMPLE_MOTOR_LAT, EXAMPLE_MOTOR_REVERSE_PIN);
        break;
    }
}

static void example_wait_for_move(position_drive_t* drive)
{
    while (position_drive_get_state(drive) == POSITION_DRIVE_STATE_MOVING)
    {
        position_drive_process(drive);
    }
}

void main(void)
{
    position_drive_config_t cfg;

    cfg.get_tick_cb = example_get_tick;
    cfg.read_raw_cb = example_read_raw;
    cfg.motor_cb = example_motor;
    cfg.set_speed_cb = (position_drive_set_speed_cb_t)0;
    cfg.debug_cb = (position_drive_debug_cb_t)0;
    cfg.context = (void*)0;

    cfg.sensor_raw_min = EXAMPLE_RAW_MIN;
    cfg.sensor_raw_max = EXAMPLE_RAW_MAX;
    cfg.angle_min_deg = EXAMPLE_ANGLE_MIN;
    cfg.angle_max_deg = EXAMPLE_ANGLE_MAX;
    cfg.target_tolerance_deg = EXAMPLE_TOLERANCE_DEG;
    cfg.move_timeout_ms = EXAMPLE_MOVE_TIMEOUT_MS;
    cfg.stuck_timeout_ms = EXAMPLE_STUCK_TIMEOUT_MS;
    cfg.stuck_min_delta_raw = EXAMPLE_STUCK_DELTA_RAW;
    cfg.direction_inverted = 0U;
    cfg.speed_min_percent = 50U;
    cfg.speed_max_percent = 100U;
    cfg.speed_default_percent = 80U;

    adc_init();
    tick_init();

    gpio_set_output(&EXAMPLE_MOTOR_TRIS, EXAMPLE_MOTOR_FORWARD_PIN);
    gpio_set_output(&EXAMPLE_MOTOR_TRIS, EXAMPLE_MOTOR_REVERSE_PIN);

    (void)position_drive_init(&g_drive, &cfg);

    while (1)
    {
        (void)position_drive_move_to_deg(&g_drive, EXAMPLE_TARGET_1_DEG);
        example_wait_for_move(&g_drive);

        (void)position_drive_move_to_deg(&g_drive, EXAMPLE_TARGET_2_DEG);
        example_wait_for_move(&g_drive);
    }
}
