/*
 * File: core/debug.h
 *
 * Public debug facade entry point. Forwards to the extensible debug module
 * in libraries/system/debug while keeping legacy debug macro aliases for
 * backward compatibility.
 */

#ifndef CORE_DEBUG_H
#define CORE_DEBUG_H

#include "libraries/system/debug/debug.h"

#ifndef DRV_DEBUG_INIT
#define DRV_DEBUG_INIT()       debug_init()
#endif

#ifndef DRV_DEBUG_LOG
#define DRV_DEBUG_LOG(msg)     debug_write_line(msg)
#endif

#ifndef DRV_DEBUG_LOG_HEX
#define DRV_DEBUG_LOG_HEX(v)   debug_write_hex8(v)
#endif

#ifndef DBG_PRINT
#define DBG_PRINT(str)         debug_write((str))
#endif

#ifndef DBG_PRINTLN
#define DBG_PRINTLN(str)       debug_write_line((str))
#endif

#ifndef DBG_WRITE_BYTE
#define DBG_WRITE_BYTE(b)      debug_write_char((char)(b))
#endif

#ifndef DBG_PRINT_INT
#define DBG_PRINT_INT(value)   debug_write_i16((int16_t)(value))
#endif

#ifndef DBG_PRINT_HEX
#define DBG_PRINT_HEX(value)   debug_write_hex8((uint8_t)(value))
#endif

#ifndef dbg_print_int
#define dbg_print_int(value)   debug_write_i16((int16_t)(value))
#endif

#ifndef dbg_print_hex
#define dbg_print_hex(value)   debug_write_hex8((uint8_t)(value))
#endif

#endif /* CORE_DEBUG_H */
