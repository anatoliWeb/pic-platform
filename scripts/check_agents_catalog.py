#!/usr/bin/env python3
from __future__ import annotations

import re
import sys
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
AGENTS_ROOT = ROOT / ".agents"

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


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8", errors="ignore")


def all_markdown_files() -> list[Path]:
    return sorted(AGENTS_ROOT.rglob("*.md")) + [ROOT / "AGENTS.md"]


def resolve_path(source: Path, target: str) -> Path | None:
    if target.startswith(("http://", "https://")):
        return None

    if target.startswith(".agents/"):
        candidate = (ROOT / target).resolve()
        if candidate.exists():
            return candidate

    if target.startswith("AGENTS.md"):
        candidate = (ROOT / target).resolve()
        if candidate.exists():
            return candidate

    candidate = (source.parent / target).resolve()
    if candidate.exists():
        return candidate

    candidate = (AGENTS_ROOT / target).resolve()
    if candidate.exists():
        return candidate

    candidate = (ROOT / target).resolve()
    if candidate.exists():
        return candidate

    if target.startswith("../"):
        candidate = (source.parent / target).resolve()
        if candidate.exists():
            return candidate

    return None


def extract_references(text: str) -> list[str]:
    refs = []
    refs.extend(re.findall(r"\(([^)]+?\.md)\)", text))
    return refs


def validate_links() -> list[str]:
    errors: list[str] = []
    for path in all_markdown_files():
        text = read_text(path)
        for ref in extract_references(text):
            resolved = resolve_path(path, ref)
            if resolved is None:
                errors.append(f"broken link: {path.relative_to(ROOT)} -> {ref}")
    return errors


def referenced_cards() -> set[str]:
    search_space = []
    for path in all_markdown_files():
        search_space.append(read_text(path))
    haystack = "\n".join(search_space)

    cards = set()
    for path in AGENTS_ROOT.rglob("*.md"):
        rel = path.relative_to(ROOT).as_posix()
        if rel in IGNORE_CARDS:
            continue
        if path.name == "README.md":
            continue
        if rel in haystack:
            cards.add(rel)
    return cards


def orphan_cards() -> list[str]:
    errors: list[str] = []
    haystack = "\n".join(read_text(path) for path in all_markdown_files())
    for path in AGENTS_ROOT.rglob("*.md"):
        rel = path.relative_to(ROOT).as_posix()
        if path.name == "README.md" or rel in IGNORE_CARDS:
            continue
        if rel not in haystack:
            errors.append(f"orphan card: {rel}")
    return errors


def validate_root_agents() -> list[str]:
    errors: list[str] = []
    text = read_text(ROOT / "AGENTS.md")
    for ref in re.findall(r"`([^`]+?\.md)`", text):
        if ref.startswith(".agents/") or ref.startswith("AGENTS.md"):
            resolved = resolve_path(ROOT / "AGENTS.md", ref)
            if resolved is None:
                errors.append(f"broken root link: {ref}")
    return errors


def main() -> int:
    errors: list[str] = []
    errors.extend(validate_root_agents())
    errors.extend(validate_links())
    errors.extend(orphan_cards())

    if errors:
        for err in errors:
            print(err)
        return 1

    print("agents catalog ok")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
