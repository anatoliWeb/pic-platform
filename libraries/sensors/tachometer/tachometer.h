/*
 * File: libraries/sensors/tachometer/tachometer.h
 */

#ifndef LIBRARIES_SENSORS_TACHOMETER_H
#define LIBRARIES_SENSORS_TACHOMETER_H

#include "core/compiler.h"
#include "core/types.h"

/*
 * Compile-time profile selection.
 *
 * Define TACHOMETER_LIGHTWEIGHT to 1 before including this header to
 * disable expensive RPM arithmetic (64-bit division). In lightweight mode:
 *   - tachometer_get_rpm() always returns 0
 *   - The rpm field in tachometer_t is not computed
 *   - All pulse filtering, startup grace, timeout, and presence detection
 *     remain fully functional
 *   - The 64-bit division runtime helper is eliminated from the linked image
 *
 * Default (TACHOMETER_LIGHTWEIGHT not defined or 0) preserves full behavior.
 */
#ifndef TACHOMETER_LIGHTWEIGHT
#define TACHOMETER_LIGHTWEIGHT 0
#endif

/*
 * Critical section for ISR/main shared data.
 *
 * Uses DRV_INT_SAVE_AND_DISABLE / DRV_INT_RESTORE from core/compiler.h.
 * These save the previous GIE state and restore it exactly, so they are
 * safe whether called from main loop (GIE=1) or from ISR context (GIE=0).
 *
 * For polling-only usage where no ISR is involved, these macros still work
 * correctly: they save GIE=1, disable, then restore GIE=1.
 */

/*
 * Diagnostic state of the tachometer.
 *
 *   STOPPED      - running is not expected in the first place.
 *   STARTING     - running is expected, but a fresh RPM is not yet formed
 *                  (still in the startup grace window or only one pulse seen).
 *   RUNNING      - a valid RPM is present and equal to or above minimum_rpm.
 *   TOO_SLOW     - pulses are arriving, but RPM is below minimum_rpm.
 *   NO_SIGNAL    - running is expected, but no new pulse within the timeout.
 *   CONFIG_ERROR - configuration is invalid (rejected at init).
 */
typedef enum
{
    TACHOMETER_STATUS_NOT_INITIALIZED = 0,
    TACHOMETER_STATUS_STOPPED,
    TACHOMETER_STATUS_STARTING,
    TACHOMETER_STATUS_RUNNING,
    TACHOMETER_STATUS_TOO_SLOW,
    TACHOMETER_STATUS_NO_SIGNAL,
    TACHOMETER_STATUS_CONFIG_ERROR
} tachometer_status_t;

typedef struct
{
    uint8_t pulses_per_revolution;       /* pulses per shaft revolution; 0 invalidates config */
    uint16_t minimum_rpm;                /* below this the status is TOO_SLOW; 0 disables the check */
    uint16_t startup_grace_ms;           /* window after arming where TOO_SLOW is suppressed */
    uint16_t signal_timeout_ms;          /* no new pulse for this long is treated as signal loss; 0 disables */
    uint16_t minimum_pulse_interval_us;  /* shorter intervals are treated as noise; 0 disables */
} tachometer_config_t;

/*
 * Internal measurement-session phase. This is NOT a counter: it saturates at
 * ACTIVE after the second accepted pulse and stays there until a re-arm.
 */
typedef enum
{
    TACHOMETER_SESSION_UNARMED = 0,   /* no pulse accepted for the current session */
    TACHOMETER_SESSION_FIRST_PULSE,   /* one pulse accepted; interval unknown yet */
    TACHOMETER_SESSION_ACTIVE         /* two-plus pulses; interval and RPM computable */
} tachometer_session_state_t;

typedef struct
{
    tachometer_config_t config;
    uint8_t initialized;
    uint8_t expected_running;                 /* 1 = caller expects the rotor to be turning */
    tachometer_status_t status;
    uint32_t expected_running_since_us;       /* monotonic time the running expectation last changed */
    uint32_t last_pulse_us;                   /* monotonic time of the last accepted pulse; ignored while UNARMED */
    uint32_t pulse_count;                     /* cumulative accepted pulses for the current expected-running session */
    uint16_t rpm;                             /* last computed RPM in 1/min; 0 until the second pulse */
    tachometer_session_state_t session_state; /* measurement-session phase, not a cumulative counter */
} tachometer_t;

drv_status_t tachometer_init(tachometer_t* tachometer,
                             const tachometer_config_t* config);
void tachometer_set_expected_running(tachometer_t* tachometer,
                                     uint8_t expected_running,
                                     uint32_t now_us);
uint8_t tachometer_on_pulse(tachometer_t* tachometer, uint32_t now_us);
void tachometer_process(tachometer_t* tachometer, uint32_t now_us);
#if TACHOMETER_LIGHTWEIGHT
#define tachometer_get_rpm(tachometer) ((uint16_t)0u)
#else
uint16_t tachometer_get_rpm(const tachometer_t* tachometer);
#endif
tachometer_status_t tachometer_get_status(const tachometer_t* tachometer);
uint32_t tachometer_get_pulse_count(const tachometer_t* tachometer);
void tachometer_reset(tachometer_t* tachometer);

/*
 * ISR/polling contract
 * ====================
 *
 * tachometer_on_pulse()
 *   - Safe to call from a timer or external interrupt (ISR context).
 *   - Uses DRV_INT_SAVE_AND_DISABLE / DRV_INT_RESTORE to protect shared
 *     fields against concurrent main-loop reads. The save/restore pair is
 *     safe from ISR context because it preserves the previous GIE state
 *     (which is 0 inside an ISR) rather than unconditionally re-enabling.
 *   - Writes: session_state, last_pulse_us, pulse_count, rpm, status.
 *
 * tachometer_process()
 *   - Call from the main loop or a periodic timer task.
 *   - Takes an atomic snapshot of ISR-written fields (last_pulse_us,
 *     session_state, rpm, expected_running, expected_running_since_us)
 *     under a short critical section.
 *   - Computes timeout and status transitions using the snapshot.
 *   - Before committing any state change (rearm, status update), re-verifies
 *     that last_pulse_us has not changed since the snapshot. If it changed,
 *     a newer pulse arrived after the snapshot and on_pulse() already updated
 *     the session — the stale result is discarded.
 *   - This prevents a torn read of last_pulse_us from causing a false
 *     NO_SIGNAL timeout that would overwrite a fresh pulse state.
 *
 * tachometer_get_rpm() / tachometer_get_status() / tachometer_get_pulse_count()
 *   - Main-loop context only.
 *   - Each returns a consistent single-field snapshot protected by a short
 *     critical section. The values may change on the next ISR after the restore.
 *
 * tachometer_set_expected_running() / tachometer_init() / tachometer_reset()
 *   - Main-loop context only.
 *   - Do not call from ISR.
 *
 * Multiple instances:
 *   - Each tachometer_t is fully independent; no shared global state.
 *   - Critical sections protect per-instance fields only.
 *
 * Polling support:
 *   - When no hardware pulse source is available, the caller can simulate
 *     pulses by calling tachometer_on_pulse() at known intervals from a
 *     timer interrupt or a precise main-loop timer. The module treats
 *     simulated pulses identically to hardware captures.
 */

#endif /* LIBRARIES_SENSORS_TACHOMETER_H */
