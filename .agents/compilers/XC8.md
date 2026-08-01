# XC8

## Purpose

XC8 compiler compatibility map.

## Location

```text
XC8/
examples-projects/xc8/
```

## Wrapper style

- Use `XC8/drivers/...` and `XC8/libraries/...` for XC8-specific implementation details.

## Known limitations

- Keep compiler-specific syntax in XC8 paths, not in shared code.

## Do

- Use shared headers from `core/`, `drivers/`, and `libraries/`.

## Don't

- Claim non-existent compiler support.
