/*
 * File: libraries/actuator/position_drive/position_drive.c
 */

#include "libraries/actuator/position_drive/position_drive.h"

/*
 * The ADC backend is the only fully implemented sensor backend.
 *
 * POSITION_DRIVE_SENSOR_ENCODER is a compile-time placeholder:
 * init() reports DRV_STATUS_UNSUPPORTED and process() does nothing.
 */

#if (POSITION_DRIVE_ENABLE_UART_DEBUG == 1)

static void position_drive_debug_send(position_drive_t* drive, const char* message)
{
    if ((drive != (position_drive_t*)0) &&
        (drive->cfg.debug_cb != (position_drive_debug_cb_t)0))
    {
        drive->cfg.debug_cb(drive->cfg.context, message);
    }
}

static void position_drive_debug_error(position_drive_t* drive, position_drive_error_t error)
{
    char buffer[20];
    char digits[4];
    uint8_t i;
    uint8_t n;
    uint16_t value;

    i = 0u;
    buffer[i++] = 'P';
    buffer[i++] = 'D';
    buffer[i++] = ':';
    buffer[i++] = 'E';
    buffer[i++] = 'R';
    buffer[i++] = 'R';
    buffer[i++] = ' ';

    value = (uint16_t)error;
    if (value == 0u)
    {
        buffer[i++] = '0';
    }
    else
    {
        n = 0u;
        while ((value > 0u) && (n < 4u))
        {
            digits[n] = (char)('0' + (value % 10u));
            n++;
            value /= 10u;
        }
        while (n > 0u)
        {
            n--;
            buffer[i++] = digits[n];
        }
    }

    buffer[i] = '\0';
    position_drive_debug_send(drive, buffer);
}

#else

static void position_drive_debug_send(position_drive_t* drive, const char* message)
{
    DRV_UNUSED(drive);
    DRV_UNUSED(message);
}

static void position_drive_debug_error(position_drive_t* drive, position_drive_error_t error)
{
    DRV_UNUSED(drive);
    DRV_UNUSED(error);
}

#endif /* POSITION_DRIVE_ENABLE_UART_DEBUG */

static int32_t position_drive_deg_distance(int16_t a, int16_t b)
{
    int32_t diff = (int32_t)a - (int32_t)b;

    if (diff < 0)
    {
        diff = -diff;
    }
    return diff;
}

static int16_t position_drive_raw_to_deg(const position_drive_t* drive, uint16_t raw)
{
    int32_t raw_delta;
    int32_t angle_span;
    int32_t num;
    int32_t deg;

    raw_delta = (int32_t)raw - (int32_t)drive->cfg.sensor_raw_min;
    angle_span = (int32_t)drive->cfg.angle_max_deg - (int32_t)drive->cfg.angle_min_deg;

    num = raw_delta * angle_span;
    deg = (int32_t)drive->cfg.angle_min_deg +
          (num / (int32_t)(drive->cfg.sensor_raw_max - drive->cfg.sensor_raw_min));

    return (int16_t)deg;
}

static void position_drive_motor_stop(position_drive_t* drive)
{
    drive->commanded_direction = POSITION_DRIVE_DIRECTION_STOP;

    if (drive->cfg.motor_cb != (position_drive_motor_cb_t)0)
    {
        drive->cfg.motor_cb(drive->cfg.context, POSITION_DRIVE_DIRECTION_STOP);
    }

#if (POSITION_DRIVE_ENABLE_PWM == 1)
    if (drive->cfg.set_speed_cb != (position_drive_set_speed_cb_t)0)
    {
        drive->cfg.set_speed_cb(drive->cfg.context, 0u);
    }
#endif
}

static void position_drive_fail(position_drive_t* drive, position_drive_error_t error)
{
    drive->error = error;
    drive->state = POSITION_DRIVE_STATE_ERROR;
    position_drive_motor_stop(drive);
    position_drive_debug_error(drive, error);
}

#if (POSITION_DRIVE_SENSOR_TYPE == POSITION_DRIVE_SENSOR_ADC)

static drv_status_t position_drive_init_adc(
    position_drive_t* drive,
    const position_drive_config_t* config)
{
    uint32_t raw_span;
    uint32_t angle_span;
    uint16_t raw;
    drv_status_t read_status;

    if (config == (const position_drive_config_t*)0)
    {
        drive->error = POSITION_DRIVE_ERROR_INVALID_CONFIG;
        return DRV_STATUS_ERROR;
    }

    if ((config->get_tick_cb == (position_drive_get_tick_fn_t)0) ||
        (config->read_raw_cb == (position_drive_read_raw_fn_t)0) ||
        (config->motor_cb == (position_drive_motor_cb_t)0))
    {
        drive->error = POSITION_DRIVE_ERROR_INVALID_CONFIG;
        return DRV_STATUS_ERROR;
    }

    if (config->sensor_raw_min >= config->sensor_raw_max)
    {
        drive->error = POSITION_DRIVE_ERROR_INVALID_CONFIG;
        return DRV_STATUS_ERROR;
    }

    if (config->angle_min_deg >= config->angle_max_deg)
    {
        drive->error = POSITION_DRIVE_ERROR_INVALID_CONFIG;
        return DRV_STATUS_ERROR;
    }

    if (config->target_tolerance_deg == 0u)
    {
        drive->error = POSITION_DRIVE_ERROR_INVALID_CONFIG;
        return DRV_STATUS_ERROR;
    }

#if (POSITION_DRIVE_ENABLE_TIMEOUT == 1)
    if (config->move_timeout_ms == 0u)
    {
        drive->error = POSITION_DRIVE_ERROR_INVALID_CONFIG;
        return DRV_STATUS_ERROR;
    }
#endif

#if (POSITION_DRIVE_ENABLE_STUCK_DETECTION == 1)
    if ((config->stuck_timeout_ms == 0u) || (config->stuck_min_delta_raw == 0u))
    {
        drive->error = POSITION_DRIVE_ERROR_INVALID_CONFIG;
        return DRV_STATUS_ERROR;
    }
#endif

#if (POSITION_DRIVE_ENABLE_PWM == 1)
    if (config->set_speed_cb == (position_drive_set_speed_cb_t)0)
    {
        drive->error = POSITION_DRIVE_ERROR_INVALID_CONFIG;
        return DRV_STATUS_ERROR;
    }
    if ((config->speed_min_percent > config->speed_max_percent) ||
        (config->speed_max_percent > 100u) ||
        (config->speed_default_percent < config->speed_min_percent) ||
        (config->speed_default_percent > config->speed_max_percent))
    {
        drive->error = POSITION_DRIVE_ERROR_INVALID_CONFIG;
        return DRV_STATUS_ERROR;
    }
#endif

    /*
     * Overflow guard for the int32 raw-to-degree conversion:
     * raw_span * angle_span must fit in a signed 32-bit value.
     */
    raw_span = (uint32_t)(config->sensor_raw_max - config->sensor_raw_min);
    angle_span = (uint32_t)((int32_t)config->angle_max_deg - (int32_t)config->angle_min_deg);
    if ((raw_span * angle_span) > 0x7FFFFFFFUL)
    {
        drive->error = POSITION_DRIVE_ERROR_INVALID_CONFIG;
        return DRV_STATUS_ERROR;
    }

    drive->cfg = *config;

    read_status = drive->cfg.read_raw_cb(drive->cfg.context, &raw);
    if (read_status != DRV_STATUS_OK)
    {
        drive->error = POSITION_DRIVE_ERROR_SENSOR_READ;
        return DRV_STATUS_ERROR;
    }
    if (raw < drive->cfg.sensor_raw_min)
    {
        drive->error = POSITION_DRIVE_ERROR_SENSOR_LOW;
        return DRV_STATUS_ERROR;
    }
    if (raw > drive->cfg.sensor_raw_max)
    {
        drive->error = POSITION_DRIVE_ERROR_SENSOR_HIGH;
        return DRV_STATUS_ERROR;
    }

    drive->current_raw = raw;
    drive->current_deg = position_drive_raw_to_deg(drive, raw);
    drive->target_deg = drive->current_deg;

    drive->state = POSITION_DRIVE_STATE_IDLE;
    drive->error = POSITION_DRIVE_ERROR_NONE;
    drive->initialized = 1u;

    position_drive_debug_send(drive, "PD:INIT");

    return DRV_STATUS_OK;
}

static void position_drive_process_adc(position_drive_t* drive)
{
    uint16_t raw;
    drv_status_t read_status;
    int32_t remaining;
    int32_t margin;
    int32_t raw_delta;
    uint32_t now;
    position_drive_direction_t desired;

    read_status = drive->cfg.read_raw_cb(drive->cfg.context, &raw);
    if (read_status != DRV_STATUS_OK)
    {
        position_drive_fail(drive, POSITION_DRIVE_ERROR_SENSOR_READ);
        return;
    }
    if (raw < drive->cfg.sensor_raw_min)
    {
        position_drive_fail(drive, POSITION_DRIVE_ERROR_SENSOR_LOW);
        return;
    }
    if (raw > drive->cfg.sensor_raw_max)
    {
        position_drive_fail(drive, POSITION_DRIVE_ERROR_SENSOR_HIGH);
        return;
    }

    drive->current_raw = raw;
    drive->current_deg = position_drive_raw_to_deg(drive, raw);

    if (drive->state != POSITION_DRIVE_STATE_MOVING)
    {
        return;
    }

    now = drive->cfg.get_tick_cb(drive->cfg.context);

    remaining = (int32_t)drive->target_deg - (int32_t)drive->current_deg;
    if (position_drive_deg_distance(drive->current_deg, drive->target_deg) <=
        (int32_t)drive->cfg.target_tolerance_deg)
    {
        position_drive_motor_stop(drive);
        drive->state = POSITION_DRIVE_STATE_TARGET_REACHED;
        drive->error = POSITION_DRIVE_ERROR_NONE;
        position_drive_debug_send(drive, "PD:REACHED");
        return;
    }

    /*
     * Bang-bang direction: the desired direction follows the sign of the
     * remaining error, so an overshoot is corrected without user action.
     */
    if (remaining > 0)
    {
        desired = POSITION_DRIVE_DIRECTION_FORWARD;
    }
    else
    {
        desired = POSITION_DRIVE_DIRECTION_REVERSE;
    }

    if (drive->cfg.direction_inverted != 0u)
    {
        if (desired == POSITION_DRIVE_DIRECTION_FORWARD)
        {
            desired = POSITION_DRIVE_DIRECTION_REVERSE;
        }
        else
        {
            desired = POSITION_DRIVE_DIRECTION_FORWARD;
        }
    }

    if (drive->commanded_direction != desired)
    {
        drive->commanded_direction = desired;
        if (drive->cfg.motor_cb != (position_drive_motor_cb_t)0)
        {
            drive->cfg.motor_cb(drive->cfg.context, desired);
        }
    }
    else if (drive->cfg.motor_cb != (position_drive_motor_cb_t)0)
    {
        /*
         * Re-assert the direction every step so the motor output can never
         * stay in an unknown state after a transient.
         */
        drive->cfg.motor_cb(drive->cfg.context, desired);
    }

#if (POSITION_DRIVE_ENABLE_PWM == 1)
    if (drive->cfg.set_speed_cb != (position_drive_set_speed_cb_t)0)
    {
        drive->cfg.set_speed_cb(drive->cfg.context, drive->cfg.speed_default_percent);
    }
#endif

#if (POSITION_DRIVE_ENABLE_TIMEOUT == 1)
    if ((uint32_t)(now - drive->move_start_ms) >= (uint32_t)drive->cfg.move_timeout_ms)
    {
        position_drive_fail(drive, POSITION_DRIVE_ERROR_TIMEOUT);
        return;
    }
#endif

#if (POSITION_DRIVE_ENABLE_STUCK_DETECTION == 1)
    raw_delta = (int32_t)raw - (int32_t)drive->stuck_start_raw;
    if (raw_delta < 0)
    {
        raw_delta = -raw_delta;
    }
    if (raw_delta < (int32_t)drive->cfg.stuck_min_delta_raw)
    {
        if ((uint32_t)(now - drive->stuck_start_ms) >= (uint32_t)drive->cfg.stuck_timeout_ms)
        {
            position_drive_fail(drive, POSITION_DRIVE_ERROR_SENSOR_STUCK);
            return;
        }
    }
    else
    {
        drive->stuck_start_ms = now;
        drive->stuck_start_raw = raw;
    }
#endif

#if (POSITION_DRIVE_ENABLE_DIRECTION_CHECK == 1)
    /*
     * A raw movement against the commanded direction is only an error when
     * the drive is clearly away from the target and the movement exceeds the
     * noise threshold. Near the target overshoot corrections are allowed.
     */
    margin = (int32_t)drive->cfg.target_tolerance_deg * 2;

    if (remaining > margin)
    {
        raw_delta = (int32_t)raw - (int32_t)drive->last_raw;
        if (raw_delta < -(int32_t)drive->cfg.stuck_min_delta_raw)
        {
            position_drive_fail(drive, POSITION_DRIVE_ERROR_DIRECTION_MISMATCH);
            return;
        }
    }
    else if (remaining < -margin)
    {
        raw_delta = (int32_t)raw - (int32_t)drive->last_raw;
        if (raw_delta > (int32_t)drive->cfg.stuck_min_delta_raw)
        {
            position_drive_fail(drive, POSITION_DRIVE_ERROR_DIRECTION_MISMATCH);
            return;
        }
    }
#endif

    drive->last_raw = raw;
}

#endif /* POSITION_DRIVE_SENSOR_TYPE == POSITION_DRIVE_SENSOR_ADC */

drv_status_t position_drive_init(
    position_drive_t* drive,
    const position_drive_config_t* config)
{
    if (drive == (position_drive_t*)0)
    {
        return DRV_STATUS_ERROR;
    }

    /*
     * Force a safe motor state before any validation. The motor must never
     * stay active after a failed init.
     */
    if (config != (const position_drive_config_t*)0)
    {
        if (config->motor_cb != (position_drive_motor_cb_t)0)
        {
            config->motor_cb(config->context, POSITION_DRIVE_DIRECTION_STOP);
        }
#if (POSITION_DRIVE_ENABLE_PWM == 1)
        if (config->set_speed_cb != (position_drive_set_speed_cb_t)0)
        {
            config->set_speed_cb(config->context, 0u);
        }
#endif
    }

    drive->initialized = 0u;
    drive->state = POSITION_DRIVE_STATE_IDLE;
    drive->error = POSITION_DRIVE_ERROR_NONE;
    drive->commanded_direction = POSITION_DRIVE_DIRECTION_STOP;
    drive->current_raw = 0u;
    drive->current_deg = 0;
    drive->target_deg = 0;
    drive->move_start_ms = 0UL;
    drive->stuck_start_ms = 0UL;
    drive->stuck_start_raw = 0u;
    drive->last_raw = 0u;

#if (POSITION_DRIVE_SENSOR_TYPE == POSITION_DRIVE_SENSOR_ADC)
    return position_drive_init_adc(drive, config);
#else
    DRV_UNUSED(config);
    drive->error = POSITION_DRIVE_ERROR_UNSUPPORTED_SENSOR;
    return DRV_STATUS_UNSUPPORTED;
#endif
}

void position_drive_process(position_drive_t* drive)
{
    if ((drive == (position_drive_t*)0) || (drive->initialized == 0u))
    {
        return;
    }

#if (POSITION_DRIVE_SENSOR_TYPE == POSITION_DRIVE_SENSOR_ADC)
    position_drive_process_adc(drive);
#else
    DRV_UNUSED(drive);
#endif
}

drv_status_t position_drive_move_to_deg(position_drive_t* drive, int16_t target_deg)
{
    uint16_t raw;
    drv_status_t read_status;
    int32_t remaining;
    position_drive_direction_t desired;
    uint32_t now;

    if (drive == (position_drive_t*)0)
    {
        return DRV_STATUS_ERROR;
    }

    if (drive->initialized == 0u)
    {
        drive->error = POSITION_DRIVE_ERROR_NOT_INITIALIZED;
        return DRV_STATUS_ERROR;
    }

#if (POSITION_DRIVE_SENSOR_TYPE != POSITION_DRIVE_SENSOR_ADC)
    drive->error = POSITION_DRIVE_ERROR_UNSUPPORTED_SENSOR;
    return DRV_STATUS_UNSUPPORTED;
#else
    if ((target_deg < drive->cfg.angle_min_deg) || (target_deg > drive->cfg.angle_max_deg))
    {
        position_drive_motor_stop(drive);
        drive->error = POSITION_DRIVE_ERROR_TARGET_OUT_OF_RANGE;
        drive->state = POSITION_DRIVE_STATE_IDLE;
        return DRV_STATUS_ERROR;
    }

    read_status = drive->cfg.read_raw_cb(drive->cfg.context, &raw);
    if (read_status != DRV_STATUS_OK)
    {
        position_drive_fail(drive, POSITION_DRIVE_ERROR_SENSOR_READ);
        return DRV_STATUS_ERROR;
    }
    if (raw < drive->cfg.sensor_raw_min)
    {
        position_drive_fail(drive, POSITION_DRIVE_ERROR_SENSOR_LOW);
        return DRV_STATUS_ERROR;
    }
    if (raw > drive->cfg.sensor_raw_max)
    {
        position_drive_fail(drive, POSITION_DRIVE_ERROR_SENSOR_HIGH);
        return DRV_STATUS_ERROR;
    }

    drive->current_raw = raw;
    drive->current_deg = position_drive_raw_to_deg(drive, raw);
    drive->target_deg = target_deg;

    if (position_drive_deg_distance(drive->current_deg, drive->target_deg) <=
        (int32_t)drive->cfg.target_tolerance_deg)
    {
        position_drive_motor_stop(drive);
        drive->state = POSITION_DRIVE_STATE_TARGET_REACHED;
        drive->error = POSITION_DRIVE_ERROR_NONE;
        position_drive_debug_send(drive, "PD:REACHED");
        return DRV_STATUS_OK;
    }

    if ((int32_t)drive->target_deg > (int32_t)drive->current_deg)
    {
        desired = POSITION_DRIVE_DIRECTION_FORWARD;
    }
    else
    {
        desired = POSITION_DRIVE_DIRECTION_REVERSE;
    }

    if (drive->cfg.direction_inverted != 0u)
    {
        if (desired == POSITION_DRIVE_DIRECTION_FORWARD)
        {
            desired = POSITION_DRIVE_DIRECTION_REVERSE;
        }
        else
        {
            desired = POSITION_DRIVE_DIRECTION_FORWARD;
        }
    }

    now = drive->cfg.get_tick_cb(drive->cfg.context);

    drive->commanded_direction = desired;
    drive->move_start_ms = now;
    drive->stuck_start_ms = now;
    drive->stuck_start_raw = raw;
    drive->last_raw = raw;

    if (drive->cfg.motor_cb != (position_drive_motor_cb_t)0)
    {
        drive->cfg.motor_cb(drive->cfg.context, desired);
    }

#if (POSITION_DRIVE_ENABLE_PWM == 1)
    if (drive->cfg.set_speed_cb != (position_drive_set_speed_cb_t)0)
    {
        drive->cfg.set_speed_cb(drive->cfg.context, drive->cfg.speed_default_percent);
    }
#endif

    drive->state = POSITION_DRIVE_STATE_MOVING;
    drive->error = POSITION_DRIVE_ERROR_NONE;

    position_drive_debug_send(drive, "PD:MOVE");

    return DRV_STATUS_OK;
#endif
}

drv_status_t position_drive_stop(position_drive_t* drive)
{
    if (drive == (position_drive_t*)0)
    {
        return DRV_STATUS_ERROR;
    }

    if (drive->initialized == 0u)
    {
        drive->error = POSITION_DRIVE_ERROR_NOT_INITIALIZED;
        return DRV_STATUS_ERROR;
    }

    position_drive_motor_stop(drive);
    drive->state = POSITION_DRIVE_STATE_IDLE;
    drive->error = POSITION_DRIVE_ERROR_NONE;

    position_drive_debug_send(drive, "PD:STOP");

    return DRV_STATUS_OK;
}

drv_status_t position_drive_emergency_stop(position_drive_t* drive)
{
    if (drive == (position_drive_t*)0)
    {
        return DRV_STATUS_ERROR;
    }

    if (drive->initialized == 0u)
    {
        drive->error = POSITION_DRIVE_ERROR_NOT_INITIALIZED;
        return DRV_STATUS_ERROR;
    }

    /*
     * Motor and PWM are stopped immediately. The error state is preserved so
     * the application can inspect the cause before calling clear_error().
     */
    position_drive_motor_stop(drive);

    if (drive->state == POSITION_DRIVE_STATE_MOVING)
    {
        drive->state = POSITION_DRIVE_STATE_IDLE;
    }

    position_drive_debug_send(drive, "PD:ESTOP");

    return DRV_STATUS_OK;
}

drv_status_t position_drive_set_speed_percent(position_drive_t* drive, uint8_t speed_percent)
{
    uint8_t speed;

    if (drive == (position_drive_t*)0)
    {
        return DRV_STATUS_ERROR;
    }

    if (drive->initialized == 0u)
    {
        drive->error = POSITION_DRIVE_ERROR_NOT_INITIALIZED;
        return DRV_STATUS_ERROR;
    }

    speed = speed_percent;
    if (speed < drive->cfg.speed_min_percent)
    {
        speed = drive->cfg.speed_min_percent;
    }
    if (speed > drive->cfg.speed_max_percent)
    {
        speed = drive->cfg.speed_max_percent;
    }

    drive->cfg.speed_default_percent = speed;

#if (POSITION_DRIVE_ENABLE_PWM == 1)
    if ((drive->state == POSITION_DRIVE_STATE_MOVING) &&
        (drive->cfg.set_speed_cb != (position_drive_set_speed_cb_t)0))
    {
        drive->cfg.set_speed_cb(drive->cfg.context, speed);
    }
#endif

    return DRV_STATUS_OK;
}

int16_t position_drive_get_current_deg(const position_drive_t* drive)
{
    if (drive == (const position_drive_t*)0)
    {
        return 0;
    }
    return drive->current_deg;
}

int16_t position_drive_get_target_deg(const position_drive_t* drive)
{
    if (drive == (const position_drive_t*)0)
    {
        return 0;
    }
    return drive->target_deg;
}

uint16_t position_drive_get_current_raw(const position_drive_t* drive)
{
    if (drive == (const position_drive_t*)0)
    {
        return 0u;
    }
    return drive->current_raw;
}

position_drive_state_t position_drive_get_state(const position_drive_t* drive)
{
    if (drive == (const position_drive_t*)0)
    {
        return POSITION_DRIVE_STATE_IDLE;
    }
    return drive->state;
}

position_drive_error_t position_drive_get_error(const position_drive_t* drive)
{
    if (drive == (const position_drive_t*)0)
    {
        return POSITION_DRIVE_ERROR_NONE;
    }
    return drive->error;
}

void position_drive_clear_error(position_drive_t* drive)
{
    if (drive == (position_drive_t*)0)
    {
        return;
    }

    drive->error = POSITION_DRIVE_ERROR_NONE;

    if (drive->state == POSITION_DRIVE_STATE_ERROR)
    {
        drive->state = POSITION_DRIVE_STATE_IDLE;
    }
}
