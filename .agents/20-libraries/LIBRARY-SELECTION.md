# Library Selection

Use this before writing new functionality.

If the task is already covered by a detailed library card, open that card next.

## Flow

1. Define the requested functionality.
2. Search this library catalog.
3. Search by library name.
4. Search by peripheral name.
5. Search by API terms.
6. Search examples and Proteus notes.
7. Decide:
   - use existing library;
   - extend existing library;
   - create a new reusable library;
   - write project-local logic only if device-specific.
8. Report why.

## Search commands

```bash
grep -R "keyword" -n core drivers libraries examples-projects docs
```

```powershell
Get-ChildItem -Recurse -File | Select-String -Pattern "keyword"
```

## Examples

- Need seven-segment display -> search the catalog -> use `seven_segment`.
- Need buttons on segment lines -> use `segment_keys` plus `button`.
- Need DC motor angle control -> use or extend `position_drive`.
- Need UART console debug -> use `uart_debug` or an application-level callback adapter.
- Need tick/timeouts -> open `.agents/core/timebase.md` and `.agents/drivers/timers.md`.
