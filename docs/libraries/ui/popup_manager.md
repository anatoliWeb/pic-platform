[🇺🇦 Ukrainian version](./popup_manager.ua.md)

# Popup Manager Library

## Description

`popup_manager` is a reusable modal popup/dialog manager for embedded UI systems.
It is display-agnostic and input-agnostic.

The module supports one active popup plus optional queued popups.

## Public API

- `popup_manager_init()`
- `popup_show()`
- `popup_close()`
- `popup_update()`
- `popup_draw()`
- `popup_is_active()` (optional)
- `popup_get_result()` (optional)

## Popup / Modal Flow Notes

Popup model (`popup_t`) includes:

- `type`
- `title`
- `message`
- `icon`
- `buttons`
- `result`
- `timeout_ms`
- `user_data`

Supported popup types:

- info
- warning
- error
- confirmation
- notification

Modal behavior:

- one popup is active at a time
- additional popups can be queued (fixed-size queue)
- closing active popup can auto-open next queued popup

## Overlay / Event Notes

Overlay rendering uses:

- `graphics_t`
- `ui_theme_t`
- `icon_pack`

No direct hardware access is used.

Event integration:

- emits `UI_EVENT_POPUP_OPEN`
- emits `UI_EVENT_POPUP_CLOSE`
- consumes `UI_EVENT_INPUT` for button navigation/selection
- timeout auto-close uses `POPUP_RESULT_TIMEOUT`

Input event mapping inside popup:

- LEFT/RIGHT -> button selection
- SELECT -> confirm selected button
- BACK/HOLD -> cancel popup

## Timeout Handling

- popup starts timeout tracking when first updated
- when `(now_ms - started_ms) >= timeout_ms`, popup closes with timeout result

## Deterministic Memory Model

- fixed popup queue (`POPUP_QUEUE_SIZE`)
- static data only
- no dynamic memory

## Dependencies

- `core/*`
- `libraries/ui/screen_manager`
- `libraries/ui/ui_events`
- `libraries/display/graphics`
- `libraries/ui/ui_themes`
- `libraries/ui/icon_pack`

## Example

`example.c` demonstrates:

- info popup
- warning popup with timeout
- confirmation dialog
- popup queueing
- popup draw/update loop
