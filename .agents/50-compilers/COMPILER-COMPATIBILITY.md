# Compiler Compatibility

Only supported compilers should be documented here.

## XC8

- Compiler: MPLAB XC8
- Path: `XC8/`
- Wrapper style: compiler-specific implementations under `XC8/drivers/` and `XC8/libraries/`
- Known limitations: device headers and XC8 syntax stay in the XC8 path; do not assume C18 behavior.
- Examples: XC8 example projects under `examples-projects/xc8/`
- Do: use the unified API from `core/` and top-level module headers.
- Don't: use `__XC8` checks in driver code.

## C18

- Compiler: MPLAB C18
- Path: `C18/`
- Wrapper style: compiler-specific implementations under `C18/drivers/` and `C18/libraries/`
- Known limitations: no full C99 `stdint`/`stdbool` support; `core/compiler.h` provides compatibility typedefs.
- Examples: C18 example projects under `examples-projects/c18/`
- Do: keep compiler branching in wrappers or `core/compiler.h`.
- Don't: claim XC8-only features work in C18 without a real wrapper.

## General rules

- Compiler selection happens in MPLAB project settings.
- Do not declare extra compiler support unless the repo actually contains the wrapper and example path.
