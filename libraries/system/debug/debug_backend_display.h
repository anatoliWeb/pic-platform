/*
 * File: libraries/system/debug/debug_backend_display.h
 *
 * DISPLAY backend for the debug facade. Routes textual output through the
 * display adapter selected by DRV_DEBUG_DISPLAY_TYPE_* / _INTERFACE_*.
 * Compiled only when DRV_DEBUG_BACKEND_DISPLAY is 1.
 * Internal header; part of the debug module.
 */

#ifndef LIBRARIES_SYSTEM_DEBUG_DEBUG_BACKEND_DISPLAY_H
#define LIBRARIES_SYSTEM_DEBUG_DEBUG_BACKEND_DISPLAY_H

#include "core/compiler.h"
#include "core/types.h"
#include "core/config.h"

#if DRV_DEBUG_BACKEND_DISPLAY

void debug_display_init(void);
void debug_display_clear(void);
void debug_display_write_char(char c);
void debug_display_newline(void);

#endif /* DRV_DEBUG_BACKEND_DISPLAY */

#endif /* LIBRARIES_SYSTEM_DEBUG_DEBUG_BACKEND_DISPLAY_H */