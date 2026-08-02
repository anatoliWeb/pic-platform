# Documentation

## Where docs live

- Human docs live in `README.md`, `README.ua.md`, and `docs/`.
- AI instructions live in `AGENTS.md` and `.agents/`.
- Example-specific documentation lives in example folders.
- Proteus wiring notes live in `examples-projects/proteus/`.

## Language pattern

- Keep EN and UA docs aligned when both exist.
- Ukrainian docs must use Ukrainian labels in Mermaid diagrams unless a symbol or API name must stay English.

## Writing rules

- Do not duplicate large blocks across many docs.
- Keep planned and implemented behavior clearly separated.
- Use Mermaid when it helps explain dependencies, wiring, or flow.
- Update docs when behavior changes.

## Catalog Validation

- Run `python scripts/check_agents_catalog.py` for `.agents/` changes.
- Do not commit with broken links or orphan cards.
- Keep canonical cards, category mappers, and `scripts/agents_catalog_manifest.json` aligned.
- Verify XC8 and C18 source contents before claiming a wrapper or independent route.
- Declaring a compiler route `absent` does not skip the check: the real `XC8/`
  and `C18/` trees are probed and a matching source still fails the route claim.
- The checker covers module discovery, card coverage, mapper membership,
  manifest completeness, route consistency, orphan cards, and broken links and
  paths. Every reusable module needs a manifest entry or an explicit exemption;
  header-only core helpers are validated too unless exempt. Mapper membership is
  enforced per exact card, so a module mentioned only in mapper prose fails.
- The success report separates public headers checked from modules discovered,
  so headers and the modules they belong to are not conflated.
