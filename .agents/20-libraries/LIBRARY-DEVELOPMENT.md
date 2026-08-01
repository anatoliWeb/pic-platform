# Library Development

## Rules

- Reusable code goes under `libraries/<category>/<name>/`.
- Public API goes in `.h`.
- Implementation goes in `.c`.
- Follow the repo pattern if `example.c` is used for the module.
- Put compiler-specific code under `XC8/` or `C18/` when needed.
- Do not hardcode project pins.
- Use callbacks for hardware binding when the library must stay reusable.
- Use config structs for backend selection and hardware parameters.
- Define defaults in `core/pic_platform_config.h`.
- Unsupported backends must return `DRV_STATUS_UNSUPPORTED`.
- Add docs in EN and UA when the library already follows that pattern.
- Add an example if it is practical and reusable.
- Add HEX if the example is built and the repo tracks HEX artifacts for it.
- Update architecture docs and the library catalog.

## Development checklist

- Search existing modules first.
- Keep the public API minimal.
- Keep project logic out of the library.
- Prefer non-blocking operation when the module participates in the main loop.
