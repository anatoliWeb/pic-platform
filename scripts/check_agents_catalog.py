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

IGNORED_PUBLIC_HEADERS = {
    "core/bit_utils.h",
    "core/compiler.h",
    "core/config.h",
    "core/device.h",
    "core/debug.h",
    "core/delay.h",
    "core/interrupts.h",
    "core/pic_platform_config.h",
    "core/types.h",
    "core/config/platform_config_check.h",
    "core/config/project_config_template.h",
    "drivers/_template/template.h",
    "drivers/timers/timer/timer.h",
}

NON_STANDALONE_HEADERS = set(IGNORED_PUBLIC_HEADERS)
HEADER_ONLY_MODULES: set[str] = set()
CARD_ALIASES = {
    "tick": ".agents/core/timebase.md",
}

# Every discovered reusable module must have a manifest entry unless it is
# explicitly exempted here. Only genuine non-route modules belong here; real
# shared/target implementations must be added to the manifest instead.
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


def is_ignored_header(path: Path) -> bool:
    return rel(path) in NON_STANDALONE_HEADERS


def discover_headers() -> list[Path]:
    headers: list[Path] = []
    for root_name in MODULE_ROOTS:
        root = ROOT / root_name
        if not root.exists():
            continue
        for header in root.rglob("*.h"):
            if is_ignored_header(header):
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
    """Return (modules, collision_errors).

    A stable path-based identifier is used to detect two headers that would
    collapse into the same short module name.
    """
    by_dir: dict[str, str] = {}
    for header in discover_headers():
        mid = module_id_from_header(header)
        by_dir.setdefault(mid.dirpath, mid.module)

    by_module: dict[str, list[str]] = {}
    for dirpath, module in sorted(by_dir.items()):
        by_module.setdefault(module, []).append(dirpath)

    modules = sorted(by_module)
    errors: list[str] = []
    for module, dirs in sorted(by_module.items()):
        if len(set(dirs)) > 1:
            errors.append(f"duplicate discovered module id: {module} ({' vs '.join(dirs)})")
    return modules, errors


def parse_mapper_table(path: Path) -> list[MapperEntry]:
    text = read_text(path)
    if "Detailed card" not in text:
        return []

    entries: list[MapperEntry] = []
    for line in text.splitlines():
        if not line.startswith("|"):
            continue
        cells = [cell.strip() for cell in line.strip().strip("|").split("|")]
        if len(cells) < 4:
            continue
        if cells[0] == "Need" or set(cells[0]) == {"-"}:
            continue

        module_cell = cells[1].replace("`", "")
        card_cell = cells[2].replace("`", "")
        source_cell = cells[3].replace("`", "")

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


def mapper_lists_module(path: Path, module: str, card: str) -> bool:
    """True when the mapper file lists the module by name or references its card.

    The mapper tables use several layouts, so membership is decided by whether
    the mapper text actually names the module (in a parseable row) or mentions
    the module's canonical card path.
    """
    text = read_text(path)
    if card and card in text:
        return True
    for entry in parse_mapper_table(path):
        if entry.module == module:
            return True
    return False


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
            if module in HEADER_ONLY_MODULES:
                continue
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
    """Mapper membership and manifest-vs-mapper card agreement."""
    errors: list[str] = []
    mapper = ROOT / entry["mapper"]
    card = entry["card"]

    if not mapper.exists():
        return errors

    if not mapper_lists_module(mapper, module, card):
        errors.append(f"manifest mapper mismatch: {module} not listed in {entry['mapper']}")
        return errors

    listed = [e for e in parse_mapper_table(mapper) if e.module == module]
    if listed and card not in {e.card for e in listed}:
        mapped_card = listed[0].card
        errors.append(f"manifest card mismatch: {module} mapper points to {mapped_card}, manifest points to {card}")

    return errors


def validate_manifest() -> tuple[list[str], int]:
    errors: list[str] = []
    manifest = load_manifest()
    checked = 0

    for module, entry in manifest.items():
        checked += 1
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
        xc8_source = ROOT / entry["xc8_source"] if entry.get("xc8_source") else None
        c18_source = ROOT / entry["c18_source"] if entry.get("c18_source") else None

        if shared_source is not None and not shared_source.exists():
            errors.append(f"missing source path: {entry['shared_source']}")
        if xc8_source is not None and not xc8_source.exists():
            errors.append(f"missing source path: {entry['xc8_source']}")
        if c18_source is not None and not c18_source.exists():
            errors.append(f"missing source path: {entry['c18_source']}")

        if shared_source is not None and shared_source.exists():
            shared_actual = classify_shared_source(shared_source)
            shared_declared = entry.get("shared_pattern")
            if shared_declared and shared_actual != shared_declared:
                errors.append(f"route mismatch: {module} shared declared {shared_declared} but source is {shared_actual}")

        for key, source, kind in (
            ("xc8_source", xc8_source, "XC8"),
            ("c18_source", c18_source, "C18"),
        ):
            declared = entry.get(f"{kind.lower()}_pattern", "absent")
            if source is None:
                if declared != "absent":
                    errors.append(f"route mismatch: {module} {kind} declared {declared} but source is absent")
                continue
            actual = classify_compiler_source(source, shared_source)
            if actual != declared:
                errors.append(f"route mismatch: {module} {kind} declared {declared} but source is {actual}")

    return errors, checked


def validate_manifest_completeness() -> tuple[list[str], int, int, int]:
    """Coverage checks between discovered modules and the manifest.

    Returns (errors, discovered_module_count, manifest_entry_count,
    manifest_exempt_count). Every discovered module must have a manifest
    entry or an explicit exemption; manifest entries that name a module no
    longer discovered are reported stale.
    """
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

    return errors, len(modules), len(manifest), len(MANIFEST_EXEMPT_MODULES)


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

    module_errors, module_count = validate_cards_and_modules()
    errors.extend(module_errors)

    route_errors, route_count = validate_manifest()
    errors.extend(route_errors)

    completeness_errors, discovered_count, manifest_count, exempt_count = validate_manifest_completeness()
    errors.extend(completeness_errors)

    orphan_count = len(orphan_cards())
    errors.extend(orphan_cards())

    if errors:
        for error in errors:
            print(error)
        return 1

    print("agents catalog ok")
    print(f"modules checked: {module_count}")
    print(f"routes checked: {route_count}")
    print(f"manifest entries: {manifest_count}")
    print(f"orphan cards checked: {orphan_count}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())