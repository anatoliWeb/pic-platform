# Checklists

## Before creating project-local code

- Search the repo first.
- Confirm it is device-specific.
- Confirm it does not belong in `libraries/` or `drivers/`.

## Before creating a new library

- Search existing libraries.
- Check examples and docs.
- Define the public API and config model.
- Keep hardware binding out of the library body.

## Before extending a library

- Check whether the behavior already exists.
- Confirm the change does not break the public API.
- Update docs and examples if behavior changes.

## Before adding an example

- Verify the example demonstrates reusable platform code.
- Keep project glue minimal.
- Add README files and Proteus notes if needed.

## Before changing `project_config.h`

- Check config defaults.
- Check timer and peripheral ownership.
- Check board wiring and pin mapping.

## Before regenerating HEX

- Build the target example only.
- Verify the output matches the tracked artifact.
- Do not commit debug or intermediate files.

## Before writing docs

- Separate implemented behavior from planned behavior.
- Match the real file tree.
- Avoid duplicating the same explanation in multiple places.

## Before commit

- Run targeted checks.
- Review diff and status.
- Keep the commit focused.

## Before final report

- List what changed.
- List checks that ran.
- State whether push was performed.

## Catalog Check

- Run `python scripts/check_agents_catalog.py` after editing `.agents/`.
- Fix broken links and orphan cards before commit.
- Update the route manifest and canonical card before changing compiler route claims.
- Run the targeted checker tests when catalog behavior changes.
- The checker validates module discovery, card coverage, mapper membership,
  manifest coverage, source-route consistency, and orphan cards. Keep
  `scripts/agents_catalog_manifest.json` in sync for every reusable module:
  a discovered module without a manifest entry or an explicit exemption and a
  stale manifest entry both fail the check.
- The checker probes the real `XC8/` and `C18/` trees: a manifest entry that
  declares a compiler route `absent` but whose compiler source actually exists
  fails the check.
- Core helper headers are only skipped when listed in the checker's
  `EXEMPT_PUBLIC_HEADERS` set; a reusable core module (even header-only) needs a
  manifest entry.
- Header-level coverage counts public headers; module-level coverage counts the
  distinct modules those headers map to. Both numbers are reported on success.
