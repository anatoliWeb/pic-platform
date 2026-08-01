# bit_utils

## Purpose

Portable bit manipulation macros for GPIO and state fields.

## Location

```text
core/bit_utils.h
```

## Public API

| Function/type/macro | Exact signature or type | Purpose | Arguments | Returns/output | Side effects | Notes |
| --- | --- | --- | --- | --- | --- | --- |
| `SET_BIT` | macro | set a bit | register, bit | macro value | modifies register | `1UL << bit` |
| `CLEAR_BIT` | macro | clear a bit | register, bit | macro value | modifies register |  |
| `TOGGLE_BIT` | macro | toggle a bit | register, bit | macro value | modifies register |  |
| `READ_BIT` | macro | read a bit | register, bit | macro value | none | returns 0/1 |

## Source inclusion strategy

```text
header-only
```
