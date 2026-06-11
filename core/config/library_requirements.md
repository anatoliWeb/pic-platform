# Library Configuration Requirements

This document defines the lightweight convention used by `pic-platform`
libraries when they need project-level configuration values.

## Rules

- Libraries must not emit `#pragma config`.
- Libraries may declare requirements or recommendations.
- The project keeps final control over the selected CONFIG bits.
- `config_bits.c` is the only file that emits the real CONFIG values.

## Common Requirement Macros

Libraries may define one or more of these markers before including the
configuration checker:

- `PIC_PLATFORM_REQUIRES_CLOCK_HZ`
- `PIC_PLATFORM_REQUIRES_WDT_CONFIG`
- `PIC_PLATFORM_REQUIRES_CCP2MUX_CONFIG`

If a required value is missing, the checker may raise `#error`.

## Recommendation Macros

Recommendations are informational. They should not override the project.

- `PIC_PLATFORM_RECOMMENDS_WDT_ON`

Recommendations may be documented in comments or handled by the application.
They should stay compiler-safe for older MPLAB C18 projects.

## Typical Project Flow

1. Create `project_config.h` in the application project.
2. Set the clock and CONFIG bit values there.
3. Include the configuration checker from the project-level `config_bits.c`.
4. Build the project with the real CONFIG values selected by the application.

## Notes

- Keep the checker simple.
- Avoid compiler-specific tricks.
- Prefer clear project-level configuration over hidden library behavior.
