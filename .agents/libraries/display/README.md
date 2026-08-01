# Display libraries

| Need | Use | Details |
|---|---|---|
| Seven-segment display | `seven_segment` | `.agents/libraries/display/seven_segment.md` |
| Other display modules | search catalog first | library-specific docs may be added later |

## Common dependencies

- `drivers/gpio`
- timer drivers for timer-owned refresh
- core types/config

## Common conflicts

- timer ownership collisions with `tick`
- pin ownership in the project
