# Input libraries

| Need | Use | Details |
|---|---|---|
| Button debounce/events | `button` | `.agents/libraries/input/button.md` |
| Shared-line segment keys | `segment_keys` | `.agents/libraries/input/segment_keys.md` |
| Other input helpers | search catalog first | extend existing reusable code if fit is close |

## Common dependencies

- `drivers/timers/tick` for button timing
- `drivers/gpio` for direct pin access
- `libraries/display/seven_segment` for shared-line key scanning

## Common conflicts

- Timer1 ownership via `tick`
- display refresh ownership when using `segment_keys`
