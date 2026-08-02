#!/usr/bin/env python3
from __future__ import annotations

import json
import re
import sys
from dataclasses import dataclass
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
AGENTS_ROOT = ROOT / ".agents"
MANIFEST_PATH = Path(__file__).with_name("agents_catalog_manifest.json")

MODULE_ROOTS = ("core", "drivers", "libraries")

IGNORE_CARDS = {
    ".agents/README.md",
    ".agents/00-map/ARCHITECTURE.md",
    ".agents/00-map/REPOSITORY-MAP.md",
    ".agents/10-rules/CODING-RULES.md",
    ".agents/10-rules/CONFIGURATION-RULES.md",
    ".agents/10-rules/GLOBAL-RULES.md",
    ".agents/20-libraries/LIBRARY-CATALOG.md",
    ".agents/20-libraries/LIBRARY-DEVELOPMENT.md",
    ".agents/20-libraries/LIBRARY-INTEGRATION.md",
    ".agents/20-libraries/LIBRARY-SELECTION.md",
    ".agents/30-projects/BOARD-INTEGRATION.md",
    ".agents/30-projects/PROJECT-INTEGRATION.md",
    ".agents/40-examples/EXAMPLES.md",
    ".agents/50-compilers/COMPILER-COMPATIBILITY.md",
    ".agents/60-build/HEX-ARTIFACTS.md",
    ".agents/60-build/TESTING.md",
    ".agents/70-docs/DOCUMENTATION.md",
    ".agents/80-workflow/GIT-WORKFLOW.md",
    ".agents/90-checklists/CHECKLISTS.md",
    ".agents/build/HEX-ARTIFACTS.md",
    ".agents/build/TESTING.md",
    ".agents/checklists/CHECKLISTS.md",
    ".agents/compilers/C18.md",
    ".agents/compilers/README.md",
    ".agents/compilers/XC8.md",
    ".agents/docs/DOCUMENTATION.md",
    ".agents/examples/README.md",
    ".agents/workflow/GIT-WORKFLOW.md",
}

# Path-based exemptions for headers that are intentionally not reusable runtime
# modules. Every entry has a human-readable reason. Reusable core helpers are
# NOT listed here; they get manifest entries and are validated.
EXEMPT_PUBLIC_HEADERS: dict[str, str] = {
    "core/debug.h": "backward-compatible alias facade over the debug module",
    "core/interrupts.h": "interrupt control macro facade, no runtime code",
    "core/pic_platform_config.h": "platform feature-tag configuration header",
    "core/config/platform_config_check.h": "internal config consistency checker",
    "core/config/project_config_template.h": "project configuration template",
    "drivers/_template/template.h": "driver authoring template",
    "drivers/timers/timer/timer.h": "generic timer placeholder superseded by timer0..3",
}

# Path-based facade/config headers that exist as cards but compile to nothing
# on their own. Kept out of the manifest because they carry no source.
EXEMPT_REUSABLE_HEADERS: dict[str, str] = {}

CARD_ALIASES = {
    "tick": ".agents/core/timebase.md",
}

# Modules exempted from a manifest entry for a documented reason. Should stay
# empty unless a module is genuinely non-route but still reusable-only.
MANIFEST_EXEMPT_MODULES: dict[str, str] = {}


@dataclass(frozen=True)
class MapperEntry:
    module: str
    card: str
    source: str
    mapper: str


@dataclass(frozen=True)
class ModuleId:
    module: str
    dirpath: str


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8-sig", errors="ignore")


def rel(path: Path) -> str:
    return path.relative_to(ROOT).as_posix()


def is_exempt_header(path: Path) -> bool:
    return rel(path) in EXEMPT_PUBLIC_HEADERS


def resolve_path(source: Path, target: str) -> Path | None:
    if target.startswith(("http://", "https://")):
        return None
    if target.startswith(("#", "mailto:")):
        return None

    candidates = []

    if target.startswith((".agents/", "AGENTS.md")):
        candidates.append((ROOT / target).resolve())

    candidates.append((source.parent / target).resolve())
    candidates.append((AGENTS_ROOT / target).resolve())
    candidates.append((ROOT / target).resolve())

    if target.startswith("../"):
        candidates.append((source.parent / target).resolve())

    for candidate in candidates:
        if candidate.exists():
            return candidate

    return None


MARKDOWN_LINK_RE = re.compile(r"\(([^)]+?\.md(?:[A-Za-z0-9_.-]*)?)\)")
REPO_PATH_RE = re.compile(
    r"(?<![A-Za-z0-9_])((?:\.\.?[\\/])?(?:[A-Za-z0-9_.-]+[\\/])+[A-Za-z0-9_.-]+\.(?:ua\.md|hex|md|txt|c|h))"
)
INCLUDE_RE = re.compile(r"^\s*#include\s+[\"<]([^\">]+)[\">]", re.MULTILINE)


def extract_markdown_refs(text: str) -> list[str]:
    return MARKDOWN_LINK_RE.findall(text)


def extract_repo_paths(text: str) -> list[str]:
    return REPO_PATH_RE.findall(text)


def extract_includes(text: str) -> list[str]:
    return INCLUDE_RE.findall(text)


def discover_headers() -> list[Path]:
    headers: list[Path] = []
    for root_name in MODULE_ROOTS:
        root = ROOT / root_name
        if not root.exists():
            continue
        for header in root.rglob("*.h"):
            if is_exempt_header(header):
                continue
            headers.append(header)
    return sorted(headers)


def module_id_from_header(header: Path) -> ModuleId:
    parent = header.parent
    if parent == ROOT / "core":
        return ModuleId(module=header.stem, dirpath="core")
    dirpath = parent.relative_to(ROOT).as_posix()
    return ModuleId(module=parent.name, dirpath=dirpath)


def module_name_from_header(header: Path) -> str:
    return module_id_from_header(header).module


def discover_modules() -> tuple[list[str], list[str]]:
    """Return (sorted module ids, errors).

    A distinct module id per discovered header; all core top-level headers must
    be preserved even when they share the `core` directory.
    """
    module_dirs: dict[str, set[str]] = {}
    for header in discover_headers():
        mid = module_id_from_header(header)
        module_dirs.setdefault(mid.module, set()).add(mid.dirpath)

    modules = sorted(module_dirs)
    errors: list[str] = []
    for module, dirs in sorted(module_dirs.items()):
        if len(dirs) > 1:
            errors.append(f"duplicate discovered module id: {module} ({' vs '.join(sorted(dirs))})")
    return modules, errors


def _candidate_compiler_paths(entry: dict[str, str]) -> list[Path]:
    """Derive plausible XC8/C18 counterpart paths for a manifest entry.

    Returns paths relative to BOTH the ROOT and a compiler root so the caller
    can decide which layout to probe. The module's shared source / header maps
    to the same relative location under the compiler directory, so a real
    compiler implementation is caught even when the manifest omits it.
    """
    result: list[Path] = []

    shared = entry.get("shared_source")
    if shared:
        candidate = Path(shared)
        if candidate.suffix:
            candidate = candidate.with_suffix(".c")
        result.append(candidate)

    header_rel = entry.get("header")
    if header_rel:
        hp = Path(header_rel)
        result.append(hp.parent / f"{hp.stem}.c")

    return sorted({p for p in result if str(p)})


def discover_compiler_sources(entry: dict[str, str], kind: str) -> tuple[list[str], bool]:
    """Return (existing_paths, ambiguous) for a compiler-specific source.

    Kind is "XC8" or "C18". Candidate paths are derived from shared/header
    mirrors under the compiler root, then broadened with a sibling search so a
    renamed compiler file is still caught.
    """
    root = ROOT / kind
    existing: set[Path] = set()

    for candidate in _candidate_compiler_paths(entry):
        if (root / candidate).exists():
            existing.add((root / candidate).resolve())

    header = ROOT / entry["header"] if entry.get("header") else None
    if header is not None and header.parent:
        rel_dir = header.parent.relative_to(ROOT)
        module = entry.get("module") or header.stem
        sibling = root / rel_dir / f"{module}.c"
        if sibling.exists():
            existing.add(sibling.resolve())

    found = sorted(str(p) for p in existing)
    ambiguous = len(found) > 1
    return found, ambiguous


def parse_mapper_table(path: Path) -> list[MapperEntry]:
    text = read_text(path)

    # Recognized four-column module mapper:
    # | Need | Module | Detailed card | Source | Status |
    if "Detailed card" in text:
        entries: list[MapperEntry] = []
        for line in text.splitlines():
            if not line.startswith("|"):
                continue
            cells = [cell.strip() for cell in line.strip().strip("|").split("|")]
            if len(cells) < 4:
                continue
            if cells[0] == "Need" or set(cells[0]) == {"-"}:
                continue

            module_cell = cells[1].replace("`", "").strip().removesuffix(" driver")
            card_cell = cells[2].replace("`", "").strip()
            source_cell = cells[3].replace("`", "").strip()

            modules = [item.strip() for item in module_cell.split(",") if item.strip()]
            cards = [item.strip() for item in card_cell.split(",") if item.strip()]

            if not modules or not cards:
                continue
            if len(cards) == 1 and len(modules) > 1:
                cards = cards * len(modules)
            if len(cards) != len(modules):
                continue

            for module, card in zip(modules, cards):
                entries.append(
                    MapperEntry(
                        module=module,
                        card=card,
                        source=source_cell,
                        mapper=rel(path),
                    )
                )
        return entries

    return []


CARD_PATH_RE = re.compile(r"\.agents/[A-Za-z0-9_./-]+\.md")

# Module cells that stand in for a set of tightly linked cards (one concrete
# module per card in the row) rather than a single exact module name. Only
# these values get wildcard behaviour; any other multiword value is an error.
MAPPER_MODULE_WILDCARDS = {
    "matching module",
}


def _row_cards(row: list[str]) -> list[str]:
    """Return the absolute companion map card refs present in a table row."""
    cards: list[str] = []
    for cell in row:
        for ref in CARD_PATH_RE.findall(cell):
            if ref not in cards:
                cards.append(ref)
    return cards


def _row_source_mismatch(row: list[str], module: str, header: str) -> str | None:
    """Report a source/header conflict if a row names a different C/H file."""
    for cell in row:
        compact = cell.replace("`", "").strip()
        if compact.startswith(("core/", "drivers/", "libraries/")) and compact.endswith((".h", ".c")):
            if compact != header:
                return f"manifest source mismatch: {module} mapper lists {compact} vs manifest header {header}"
    return None


def _table_rows(path: Path) -> list[list[str]]:
    return [
        [cell.strip() for cell in line.strip().strip("|").split("|")]
        for line in read_text(path).splitlines()
        if line.lstrip().startswith("|")
    ]


def _find_module_column(table_rows: list[list[str]]) -> int | None:
    """Locate the `Module` column index in a four-column module mapper."""
    for row in table_rows:
        for index, cell in enumerate(row):
            if cell.strip().lower() == "module":
                return index
    return None


def _module_values(cell: str) -> list[str]:
    compact = cell.replace("`", "").strip()
    return [item.strip().removesuffix(" driver") for item in compact.split(",") if item.strip()]


def mapper_row_membership(path: Path, module: str, card: str, header: str) -> str | None:
    """Strict mapper membership check.

    Two layouts are supported:

    A. Standard module mapper (a `Module` column exists): a row is valid only
       when it names the exact module and points at the exact manifest card, with
       any source column agreeing with the manifest header.

    B. Core/Open navigation mapper (no module column): valid when a table row
       points at the exact card and any source path in the row agrees with the
       manifest header.

    Prose-only card mentions never satisfy either layout. Returns None on
    success, otherwise a card mismatch (row present under a different card or in
    the wrong module column) or a plain mapper mismatch (module never listed).
    """
    table_rows = _table_rows(path)
    module_col = _find_module_column(table_rows)

    if module_col is not None:
        for row in table_rows:
            if module_col >= len(row):
                continue
            modules = _module_values(row[module_col])
            if not modules:
                continue
            wildcard = any(value.strip().lower() in MAPPER_MODULE_WILDCARDS for value in modules)
            if not wildcard and module not in modules:
                continue
            if card not in _row_cards(row):
                if wildcard:
                    continue
                return f"manifest card mismatch: {module} mapper points to a different card in {rel(path)}"
            if (reason := _row_source_mismatch(row, module, header)) is not None:
                return reason
            return None
        return f"manifest mapper mismatch: {module} not listed in {rel(path)}"

    for row in table_rows:
        if card not in _row_cards(row):
            continue
        if (reason := _row_source_mismatch(row, module, header)) is not None:
            return reason
        return None

    return f"manifest mapper mismatch: {module} not listed in {rel(path)}"


def load_mapper_index() -> dict[str, MapperEntry]:
    index: dict[str, MapperEntry] = {}
    for path in sorted(AGENTS_ROOT.rglob("README.md")):
        for entry in parse_mapper_table(path):
            index.setdefault(entry.module, entry)
    return index


def resolve_card_path(module: str) -> Path | None:
    alias = CARD_ALIASES.get(module)
    if alias is not None:
        candidate = ROOT / alias
        if candidate.exists():
            return candidate

    matches = sorted(AGENTS_ROOT.rglob(f"{module}.md"))
    if len(matches) == 1:
        return matches[0]

    return None


def load_manifest() -> dict[str, dict[str, str]]:
    if not MANIFEST_PATH.exists():
        return {}
    return json.loads(read_text(MANIFEST_PATH))


def classify_shared_source(path: Path) -> str:
    if not path.exists():
        return "absent"

    text = read_text(path)
    includes = extract_includes(text)
    if any("XC8/" in item or "C18/" in item for item in includes):
        return "dispatcher"
    if "#if defined(DRV_COMPILER_C18)" in text or "#if defined(DRV_COMPILER_XC8)" in text:
        return "dispatcher"
    return "implementation"


def includes_shared_source(source_text: str, shared_source: Path) -> bool:
    shared_rel = shared_source.relative_to(ROOT).as_posix()
    for include in extract_includes(source_text):
        normalized = include.replace("\\", "/")
        while normalized.startswith("../"):
            normalized = normalized[3:]
        if normalized.startswith("./"):
            normalized = normalized[2:]
        if normalized == shared_rel:
            return True
    return False


def classify_compiler_source(path: Path, shared_source: Path | None) -> str:
    if not path.exists():
        return "absent"

    if shared_source is not None and includes_shared_source(read_text(path), shared_source):
        return "include-wrapper"

    return "independent"


def validate_links() -> list[str]:
    errors: list[str] = []

    for path in [ROOT / "AGENTS.md", *AGENTS_ROOT.rglob("*.md")]:
        text = read_text(path)

        for ref in extract_markdown_refs(text):
            resolved = resolve_path(path, ref)
            if resolved is None:
                errors.append(f"broken link: {rel(path)} -> {ref}")

        for ref in extract_repo_paths(text):
            resolved = resolve_path(path, ref)
            if resolved is None:
                errors.append(f"broken path: {rel(path)} -> {ref}")

    return errors


def validate_root_agents() -> list[str]:
    errors: list[str] = []
    text = read_text(ROOT / "AGENTS.md")
    for ref in re.findall(r"`([^`]+?\.md)`", text):
        if ref.startswith((".agents/", "AGENTS.md")) and resolve_path(ROOT / "AGENTS.md", ref) is None:
            errors.append(f"broken root link: {ref}")
    return errors


def validate_cards_and_modules() -> tuple[list[str], int]:
    """Header-level card validation. Returns (errors, public_headers_checked)."""
    errors: list[str] = []
    mapper_index = load_mapper_index()
    discovered_headers = discover_headers()
    checked = 0
    haystack = "\n".join(read_text(path) for path in [ROOT / "AGENTS.md", *AGENTS_ROOT.rglob("*.md")])

    for header in discovered_headers:
        module = module_name_from_header(header)
        entry = mapper_index.get(module)
        card_path: Path | None = None

        if entry is None:
            card_path = resolve_card_path(module)
            if card_path is None:
                errors.append(f"missing mapper entry: {module}")
                continue
            if rel(card_path) not in haystack:
                errors.append(f"missing mapper entry: {module}")
                continue
        else:
            card_path = ROOT / entry.card

        checked += 1
        if not card_path.exists():
            errors.append(f"missing card: {rel(header)}")
            continue

        card_text = read_text(card_path)
        header_rel = rel(header)
        if header_rel not in card_text:
            card_label = entry.card if entry is not None else rel(card_path)
            errors.append(f"card header mismatch: {card_label} -> {header_rel}")

    return errors, checked


def validate_card_mapper_paths(module: str, entry: dict[str, str]) -> list[str]:
    errors: list[str] = []
    mapper = ROOT / entry["mapper"]
    card = entry["card"]

    if not mapper.exists():
        return errors

    reason = mapper_row_membership(mapper, module, card, entry["header"])
    if reason:
        errors.append(reason)
    return errors


def validate_manifest() -> tuple[list[str], int]:
    errors: list[str] = []
    manifest = load_manifest()
    checked = 0

    for module, entry in manifest.items():
        checked += 1
        entry = dict(entry)
        entry["module"] = module

        header = ROOT / entry["header"]
        card = ROOT / entry["card"]
        mapper = ROOT / entry["mapper"]

        for label, path in (("header", header), ("card", card), ("mapper", mapper)):
            if not path.exists():
                errors.append(f"missing {label} path: {entry[label]}")

        if card.exists() and entry["header"] not in read_text(card):
            errors.append(f"card header mismatch: {entry['card']} -> {entry['header']}")

        errors.extend(validate_card_mapper_paths(module, entry))

        shared_source = ROOT / entry["shared_source"] if entry.get("shared_source") else None
        shared_pattern = entry.get("shared_pattern", "header-only")

        if shared_source is not None and not shared_source.exists():
            errors.append(f"missing source path: {entry['shared_source']}")

        if shared_source is not None and shared_source.exists():
            shared_actual = classify_shared_source(shared_source)
            if shared_pattern == "header-only":
                errors.append(f"route mismatch: {module} shared declared header-only but source exists")
            elif shared_actual != shared_pattern:
                errors.append(
                    f"route mismatch: {module} shared declared {shared_pattern} but source is {shared_actual}"
                )

        for kind in ("XC8", "C18"):
            key_source = f"{kind.lower()}_source"
            key_pattern = f"{kind.lower()}_pattern"
            declared = entry.get(key_pattern, "absent")
            declared_source = entry.get(key_source)

            if declared_source:
                source = ROOT / declared_source
                if not source.exists():
                    errors.append(f"missing source path: {declared_source}")
                    continue
                actual = classify_compiler_source(source, shared_source)
                if actual != declared:
                    errors.append(f"route mismatch: {module} {kind} declared {declared} but source is {actual}")
                continue

            if declared == "absent":
                found, ambiguous = discover_compiler_sources(entry, kind)
                if ambiguous:
                    errors.append(f"ambiguous {kind} source: {module} -> {', '.join(found)}")
                elif found:
                    errors.append(f"route mismatch: {module} {kind} declared absent but source exists at {found[0]}")
            else:
                errors.append(f"route mismatch: {module} {kind} declared {declared} but source is absent")

    return errors, checked


def validate_manifest_completeness() -> tuple[list[str], int, int, int]:
    errors: list[str] = []
    modules, collisions = discover_modules()
    errors.extend(collisions)

    manifest = load_manifest()
    manifest_modules = set(manifest)

    missing = [m for m in modules if m not in manifest_modules and m not in MANIFEST_EXEMPT_MODULES]
    for module in sorted(missing):
        errors.append(f"missing manifest entry: {module}")

    discovered_ids = set(modules)
    stale = [m for m in manifest_modules if m not in discovered_ids]
    for module in sorted(stale):
        errors.append(f"stale manifest entry: {module}")

    exempt_count = len(EXEMPT_PUBLIC_HEADERS)
    return errors, len(modules), len(manifest), exempt_count


def orphan_cards() -> list[str]:
    errors: list[str] = []
    haystack = "\n".join(read_text(path) for path in [ROOT / "AGENTS.md", *AGENTS_ROOT.rglob("*.md")])
    for path in AGENTS_ROOT.rglob("*.md"):
        rel_path = rel(path)
        if path.name == "README.md" or rel_path in IGNORE_CARDS:
            continue
        if rel_path not in haystack:
            errors.append(f"orphan card: {rel_path}")
    return errors


def main() -> int:
    errors: list[str] = []
    errors.extend(validate_root_agents())
    errors.extend(validate_links())

    header_errors, headers_checked = validate_cards_and_modules()
    errors.extend(header_errors)

    route_errors, route_count = validate_manifest()
    errors.extend(route_errors)

    completeness_errors, module_count, manifest_count, exempt_count = validate_manifest_completeness()
    errors.extend(completeness_errors)

    orphan_errors = orphan_cards()
    orphan_count = len(orphan_errors)
    errors.extend(orphan_errors)

    if errors:
        for error in errors:
            print(error)
        return 1

    print("agents catalog ok")
    print(f"public headers checked: {headers_checked}")
    print(f"modules discovered: {module_count}")
    print(f"manifest entries: {manifest_count}")
    print(f"routes checked: {route_count}")
    print(f"exempt headers: {exempt_count}")
    print(f"orphan cards checked: {orphan_count}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
