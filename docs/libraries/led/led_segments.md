# LED Segments Library

## Description

`led_segments` provides logical LED regions over an existing WS2812 strip or WS2812 matrix.
It allows independent UI zones (status bars, progress bars, animation regions) without creating extra framebuffers.

## Public API

- `led_segment_init()`
- `led_segment_set_pixel()`
- `led_segment_fill()`
- `led_segment_clear()`
- `led_segment_show()`
- `led_segment_reverse()` (optional helper)
- `led_segment_mirror()` (optional helper)

## Segment Mapping Model

- Segment is a logical view over a parent object:
  - linear strip section (`start_index`, `length`)
  - matrix region (`region_x`, `region_y`, `region_width`, `region_height`)
- Logical index `0..length-1` is mapped to physical LED index.
- `reverse` flag inverts logical direction.

## Shared Framebuffer Model

- No duplicated pixel memory.
- Segment writes directly into parent strip/matrix framebuffer.
- `led_segment_show()` delegates output to `ws2812_show()` or `ws2812_matrix_show()`.

## Example Use Cases

- Status zones on one strip.
- Segmented progress bar.
- Mirrored left/right UI regions.
- Matrix sub-regions for independent widgets.

## Dependencies

- `core/*`
- `libraries/led/ws2812`
- `libraries/led/rgb_utils`
- `libraries/led/ws2812_matrix` (for matrix region mode)

## Performance Notes

- No dynamic memory.
- No blocking delays.
- Integer-only mapping.
- Suitable for incremental updates from loop/scheduler/RTOS task.
