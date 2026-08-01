/*
 * File: libraries/actuator/position_drive/position_drive.h
 */

#ifndef LIBRARIES_ACTUATOR_POSITION_DRIVE_H
#define LIBRARIES_ACTUATOR_POSITION_DRIVE_H

/*
 * position_drive - non-blocking closed-loop position control for a DC gear
 * motor with a position sensor.
 *
 * First version supports a potentiometer through an ADC as the position
 * sensor. The sensor backend is selected at compile time through
 * POSITION_DRIVE_SENSOR_TYPE (default POSITION_DRIVE_SENSOR_ADC).
 *
 * The library never touches hardware directly. All hardware access goes
 * through callbacks supplied by the application:
 *
 *   - get_tick_cb : millisecond time source (for example tick_get())
 *   - read_raw_cb : position sensor raw read
 *   - motor_cb    : DC motor direction (H-bridge IN1/IN2 style)
 *   - set_speed_cb: optional PWM speed, required when PWM support is enabled
 *
 * Compile-time options live in core/pic_platform_config.h and can be
 * overridden with compiler -D flags:
 *
 *   POSITION_DRIVE_SENSOR_TYPE              (ADC default)
 *   POSITION_DRIVE_ENABLE_PWM               (0 default)
 *   POSITION_DRIVE_ENABLE_TIMEOUT           (1 default)
 *   POSITION_DRIVE_ENABLE_STUCK_DETECTION   (1 default)
 *   POSITION_DRIVE_ENABLE_DIRECTION_CHECK   (1 default)
 *   POSITION_DRIVE_ENABLE_UART_DEBUG        (0 default)
 *   POSITION_DRIVE_DEBUG_LEVEL              (info when debug is enabled)
 *
 * The debug level is module-local. Keep the enable flag at 0 for the smallest
 * code size.
 */

#include "core/compiler.h"
#include "core/types.h"

/*
 * Sensor backend identifiers.
 *
 * POSITION_DRIVE_SENSOR_ENCODER is a compile-time placeholder for future
 * encoder support. The encoder backend is not implemented yet: init reports
 * DRV_STATUS_UNSUPPORTED instead of silently doing something wrong.
 */
#define POSITION_DRIVE_SENSOR_NONE     0U
#define POSITION_DRIVE_SENSOR_ADC      1U
#define POSITION_DRIVE_SENSOR_ENCODER  2U

#define POSITION_DRIVE_DEBUG_LEVEL_ERROR 1U
#define POSITION_DRIVE_DEBUG_LEVEL_INFO  2U
#define POSITION_DRIVE_DEBUG_LEVEL_TRACE 3U

#ifndef POSITION_DRIVE_DEBUG_LEVEL
#if (POSITION_DRIVE_ENABLE_UART_DEBUG == 1)
#define POSITION_DRIVE_DEBUG_LEVEL POSITION_DRIVE_DEBUG_LEVEL_INFO
#else
#define POSITION_DRIVE_DEBUG_LEVEL 0U
#endif
#endif

#include "core/pic_platform_config.h"

typedef enum
{
    POSITION_DRIVE_STATE_IDLE = 0,
    POSITION_DRIVE_STATE_MOVING,
    POSITION_DRIVE_STATE_TARGET_REACHED,
    POSITION_DRIVE_STATE_ERROR
} position_drive_state_t;

typedef enum
{
    POSITION_DRIVE_DIRECTION_STOP = 0,
    POSITION_DRIVE_DIRECTION_FORWARD,
    POSITION_DRIVE_DIRECTION_REVERSE
} position_drive_direction_t;

typedef enum
{
    POSITION_DRIVE_ERROR_NONE = 0,
    POSITION_DRIVE_ERROR_NOT_INITIALIZED,
    POSITION_DRIVE_ERROR_TARGET_OUT_OF_RANGE,
    POSITION_DRIVE_ERROR_SENSOR_LOW,
    POSITION_DRIVE_ERROR_SENSOR_HIGH,
    POSITION_DRIVE_ERROR_SENSOR_READ,
    POSITION_DRIVE_ERROR_SENSOR_STUCK,
    POSITION_DRIVE_ERROR_TIMEOUT,
    POSITION_DRIVE_ERROR_DIRECTION_MISMATCH,
    POSITION_DRIVE_ERROR_UNSUPPORTED_SENSOR,
    POSITION_DRIVE_ERROR_INVALID_CONFIG
} position_drive_error_t;

/*
 * Reads the raw position sensor value into *raw_value.
 * Return DRV_STATUS_OK on success.
 */
typedef drv_status_t (*position_drive_read_raw_fn_t)(void* context, uint16_t* raw_value);

/*
 * Returns the current time in milliseconds. The value is used for move
 * timeout and stuck detection.
 */
typedef uint32_t (*position_drive_get_tick_fn_t)(void* context);

/*
 * Commands the DC motor direction. STOP disables both H-bridge inputs.
 * The application maps FORWARD/REVERSE to its physical pins.
 */
typedef void (*position_drive_motor_cb_t)(void* context, position_drive_direction_t direction);

/*
 * Optional PWM speed command (0..100 percent). Required when
 * POSITION_DRIVE_ENABLE_PWM == 1.
 */
typedef void (*position_drive_set_speed_cb_t)(void* context, uint8_t speed_percent);

/*
 * Optional debug output. Invoked only when POSITION_DRIVE_ENABLE_UART_DEBUG
 * is enabled at compile time. The application can forward messages to UART.
 */
typedef void (*position_drive_debug_cb_t)(void* context, const char* message);

typedef struct
{
    /*
     * Millisecond time source. Required.
     */
    position_drive_get_tick_fn_t get_tick_cb;

    /*
     * Position sensor read. Required for the ADC backend.
     */
    position_drive_read_raw_fn_t read_raw_cb;

    /*
     * DC motor direction output. Required.
     */
    position_drive_motor_cb_t motor_cb;

    /*
     * Optional PWM speed output. Required only when PWM support is enabled.
     */
    position_drive_set_speed_cb_t set_speed_cb;

    /*
     * Optional debug output. Used only when POSITION_DRIVE_ENABLE_UART_DEBUG
     * is enabled.
     */
    position_drive_debug_cb_t debug_cb;

    /*
     * Context pointer passed unchanged to every callback.
     */
    void* context;

    /*
     * Sensor raw values that map to angle_min_deg / angle_max_deg.
     * Must satisfy sensor_raw_min < sensor_raw_max.
     */
    uint16_t sensor_raw_min;
    uint16_t sensor_raw_max;

    /*
     * Mechanical travel limits in degrees.
     * Must satisfy angle_min_deg < angle_max_deg.
     */
    int16_t angle_min_deg;
    int16_t angle_max_deg;

    /*
     * Deadband in degrees around the target. Must be > 0.
     */
    uint8_t target_tolerance_deg;

    /*
     * Maximum allowed move time. Required when POSITION_DRIVE_ENABLE_TIMEOUT
     * is enabled. 0 is rejected.
     */
    uint16_t move_timeout_ms;

    /*
     * Time without enough raw movement before the motor is declared stuck.
     * Required when POSITION_DRIVE_ENABLE_STUCK_DETECTION is enabled.
     */
    uint16_t stuck_timeout_ms;

    /*
     * Minimum raw change considered as movement for stuck detection and
     * direction sanity check. Required when stuck detection is enabled.
     */
    uint16_t stuck_min_delta_raw;

    /*
     * 1 reverses the FORWARD/REVERSE polarity to match the wiring.
     */
    uint8_t direction_inverted;

    /*
     * Speed range in percent (0..100). Used only when PWM support is enabled.
     */
    uint8_t speed_min_percent;
    uint8_t speed_max_percent;
    uint8_t speed_default_percent;
} position_drive_config_t;

typedef struct
{
    position_drive_config_t cfg;

    uint8_t initialized;

    uint16_t current_raw;
    int16_t current_deg;
    int16_t target_deg;

    position_drive_state_t state;
    position_drive_error_t error;

    position_drive_direction_t commanded_direction;

    uint32_t move_start_ms;
    uint32_t stuck_start_ms;
    uint16_t stuck_start_raw;
    uint16_t last_raw;
} position_drive_t;

/*
 * Validates config, stops the motor, reads the sensor and enters IDLE.
 * Returns DRV_STATUS_OK on success.
 */
drv_status_t position_drive_init(
    position_drive_t* drive,
    const position_drive_config_t* config
);

/*
 * Non-blocking control step. Must be called regularly from the main loop.
 */
void position_drive_process(
    position_drive_t* drive
);

/*
 * Starts an asynchronous move to target_deg (must be inside the angle
 * limits). Returns DRV_STATUS_OK when the move was scheduled.
 */
drv_status_t position_drive_move_to_deg(
    position_drive_t* drive,
    int16_t target_deg
);

/*
 * Manual stop. Motor off, state IDLE, error cleared.
 */
drv_status_t position_drive_stop(
    position_drive_t* drive
);

/*
 * Emergency stop. Motor and PWM off immediately. The error state is left
 * intact so the application can inspect it. Does not restart the motor.
 */
drv_status_t position_drive_emergency_stop(
    position_drive_t* drive
);

/*
 * Sets the speed used for subsequent and current moves. Clamped to
 * speed_min_percent..speed_max_percent. Only meaningful when PWM support is
 * enabled.
 */
drv_status_t position_drive_set_speed_percent(
    position_drive_t* drive,
    uint8_t speed_percent
);

int16_t position_drive_get_current_deg(
    const position_drive_t* drive
);

int16_t position_drive_get_target_deg(
    const position_drive_t* drive
);

uint16_t position_drive_get_current_raw(
    const position_drive_t* drive
);

position_drive_state_t position_drive_get_state(
    const position_drive_t* drive
);

position_drive_error_t position_drive_get_error(
    const position_drive_t* drive
);

/*
 * Clears the error and returns to IDLE. Does not restart the motor.
 */
void position_drive_clear_error(
    position_drive_t* drive
);

#endif /* LIBRARIES_ACTUATOR_POSITION_DRIVE_H */
