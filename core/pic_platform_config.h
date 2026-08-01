#ifndef CORE_PIC_PLATFORM_CONFIG_H
#define CORE_PIC_PLATFORM_CONFIG_H

/*
 * Common platform feature configuration.
 *
 * These defaults are intentionally conservative. Example projects or build
 * metadata may override them with compiler macros when a feature backend must
 * be enabled for all translation units.
 */

#ifndef SEVEN_SEGMENT_ENABLE_TIMER0
#define SEVEN_SEGMENT_ENABLE_TIMER0 0
#endif

#ifndef SEVEN_SEGMENT_ENABLE_TIMER1
#define SEVEN_SEGMENT_ENABLE_TIMER1 0
#endif

#ifndef SEVEN_SEGMENT_ENABLE_TIMER2
#define SEVEN_SEGMENT_ENABLE_TIMER2 0
#endif

#ifndef SEVEN_SEGMENT_ENABLE_TIMER3
#define SEVEN_SEGMENT_ENABLE_TIMER3 0
#endif

/*
 * position_drive (libraries/actuator/position_drive) compile-time options.
 *
 * POSITION_DRIVE_SENSOR_TYPE must be one of the POSITION_DRIVE_SENSOR_*
 * constants defined in libraries/actuator/position_drive/position_drive.h.
 *
 * Override any of these defines with compiler -D flags so every translation
 * unit (including libraries/actuator/position_drive/position_drive.c) sees the
 * same value. Do not override them from project_config.h only: the library .c
 * does not include project_config.h.
 */
#ifndef POSITION_DRIVE_SENSOR_TYPE
#define POSITION_DRIVE_SENSOR_TYPE POSITION_DRIVE_SENSOR_ADC
#endif

#ifndef POSITION_DRIVE_ENABLE_PWM
#define POSITION_DRIVE_ENABLE_PWM 0
#endif

#ifndef POSITION_DRIVE_ENABLE_TIMEOUT
#define POSITION_DRIVE_ENABLE_TIMEOUT 1
#endif

#ifndef POSITION_DRIVE_ENABLE_STUCK_DETECTION
#define POSITION_DRIVE_ENABLE_STUCK_DETECTION 1
#endif

#ifndef POSITION_DRIVE_ENABLE_DIRECTION_CHECK
#define POSITION_DRIVE_ENABLE_DIRECTION_CHECK 1
#endif

#ifndef POSITION_DRIVE_ENABLE_UART_DEBUG
#define POSITION_DRIVE_ENABLE_UART_DEBUG 0
#endif

#endif /* CORE_PIC_PLATFORM_CONFIG_H */
