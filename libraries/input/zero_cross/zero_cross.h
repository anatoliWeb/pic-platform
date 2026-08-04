/*
 * File: libraries/input/zero_cross/zero_cross.h
 *
 * Reusable zero-cross edge detector with glitch rejection, half-cycle
 * measurement, timeout and recovery. It owns no pin, no timer and no output.
 * The caller feeds edges with a monotonic microsecond time and dispatches the
 * produced event to any number of consumers (ac_phase_control, burst-fire,
 * energy meter, diagnostics).
 */

#ifndef LIBRARIES_INPUT_ZERO_CROSS_H
#define LIBRARIES_INPUT_ZERO_CROSS_H

#include "core/compiler.h"
#include "core/types.h"

/*
 * Optional feature blocks. Each is enabled by default and can be cut from a
 * build by defining the matching ZERO_CROSS_DISABLE_* macro. They never need
 * to be defined explicitly to enable the feature.
 */
#ifndef ZERO_CROSS_DISABLE_TIMEOUT
#define ZERO_CROSS_ENABLE_TIMEOUT
#endif

#ifndef ZERO_CROSS_DISABLE_FREQUENCY_DETECTION
#define ZERO_CROSS_ENABLE_FREQUENCY_DETECTION
#endif

#ifndef ZERO_CROSS_DISABLE_GLITCH_FILTER
#define ZERO_CROSS_ENABLE_GLITCH_FILTER
#endif

typedef enum
{
    ZERO_CROSS_STATUS_NOT_INITIALIZED = 0,
    ZERO_CROSS_STATUS_WAITING,
    ZERO_CROSS_STATUS_ALIVE,
    ZERO_CROSS_STATUS_LOST,
    ZERO_CROSS_STATUS_CONFIG_ERROR
} zero_cross_status_t;

typedef enum
{
    ZERO_CROSS_FREQUENCY_UNKNOWN = 0,
    ZERO_CROSS_FREQUENCY_50_HZ,
    ZERO_CROSS_FREQUENCY_60_HZ
} zero_cross_frequency_t;

typedef struct
{
    uint16_t timeout_ms;
    uint16_t min_half_cycle_us;
    uint16_t max_half_cycle_us;
    uint16_t glitch_reject_us;
    uint8_t recovery_event_count;
} zero_cross_config_t;

typedef struct
{
    uint32_t timestamp_us;
    uint16_t half_cycle_us;
    uint32_t sequence;
    zero_cross_frequency_t frequency;
} zero_cross_event_t;

typedef struct
{
    zero_cross_config_t config;
    uint8_t initialized;
    uint8_t armed;
    zero_cross_status_t status;
    uint32_t last_edge_us;
    uint16_t half_cycle_us;
    uint32_t sequence;
    zero_cross_frequency_t frequency;
    uint8_t recovery_count;
} zero_cross_t;

drv_status_t zero_cross_init(zero_cross_t* zc,
                             const zero_cross_config_t* config);

uint8_t zero_cross_on_edge(zero_cross_t* zc,
                           uint32_t now_us,
                           zero_cross_event_t* event);

void zero_cross_process(zero_cross_t* zc, uint32_t now_us);

uint8_t zero_cross_is_alive(const zero_cross_t* zc);
zero_cross_status_t zero_cross_get_status(const zero_cross_t* zc);
zero_cross_frequency_t zero_cross_get_frequency(const zero_cross_t* zc);
uint16_t zero_cross_get_half_cycle_us(const zero_cross_t* zc);
uint32_t zero_cross_get_sequence(const zero_cross_t* zc);
void zero_cross_reset(zero_cross_t* zc);

#endif /* LIBRARIES_INPUT_ZERO_CROSS_H */
