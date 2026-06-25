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

#endif /* CORE_PIC_PLATFORM_CONFIG_H */
