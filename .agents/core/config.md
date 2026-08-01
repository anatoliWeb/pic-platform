# config

## Purpose

Global compile-time feature flags.

## Location

```text
core/config.h
core/config/project_config_template.h
core/pic_platform_config.h
```

## Use when

- you need shared debug flags;
- you need default feature flags;
- you need project-level configuration templates.

## Public API

| Function/type | Purpose | What to pass | Returns / output | Notes |
| --- | --- | --- | --- | --- |
| `DRV_DEBUG_ENABLE` | enables debug layer | define value | compile-time flag | used by `uart_debug` |
| `DRV_USE_UART` | gates UART debug transport | define value | compile-time flag | used by `uart_debug` |
| `SEVEN_SEGMENT_ENABLE_TIMER0..3` | enables timer backends | define value | compile-time flag | used by `seven_segment` |
| `POSITION_DRIVE_*` defines | actuator behavior flags | define value | compile-time flag | should be visible to library TU |

## Required files

```text
core/config.h
core/config/project_config_template.h
core/pic_platform_config.h
```

## Dependencies

```text
none
```

## Integration recipe

1. Copy the project config template into the application.
2. Keep literal config bits in `config_bits.c`.
3. Put reusable defaults in `core/pic_platform_config.h`.
4. Override with compiler `-D` flags when library translation units must see the same value.

## Human documentation

```text
docs/configuration/*
docs/libraries/actuator/position_drive.md
docs/libraries/display/seven_segment.md
```
