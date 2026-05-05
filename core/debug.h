#ifndef CORE_DEBUG_H
#define CORE_DEBUG_H

/*
 * Debug macro template.
 * Enable by defining DRV_DEBUG_ENABLE at project level.
 */

#ifdef DRV_DEBUG_ENABLE

    #define DRV_DEBUG_INIT()           do { } while (0)
    #define DRV_DEBUG_LOG(msg)         do { DRV_UNUSED(msg); } while (0)
    #define DRV_DEBUG_LOG_HEX(v)       do { DRV_UNUSED(v); } while (0)

#else

    #define DRV_DEBUG_INIT()           do { } while (0)
    #define DRV_DEBUG_LOG(msg)         do { } while (0)
    #define DRV_DEBUG_LOG_HEX(v)       do { } while (0)

#endif

#endif /* CORE_DEBUG_H */
