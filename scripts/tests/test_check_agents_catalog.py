from __future__ import annotations

import importlib
import json
import tempfile
import unittest
from pathlib import Path


checker = importlib.import_module("scripts.check_agents_catalog")


def write(path: Path, text: str) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(text, encoding="utf-8")


class CheckAgentsCatalogTests(unittest.TestCase):
    def setUp(self) -> None:
        self.tempdir = tempfile.TemporaryDirectory()
        self.root = Path(self.tempdir.name)
        self.agents = self.root / ".agents"

        self.original_root = checker.ROOT
        self.original_agents_root = checker.AGENTS_ROOT
        self.original_manifest_path = checker.MANIFEST_PATH
        checker.ROOT = self.root
        checker.AGENTS_ROOT = self.agents
        checker.MANIFEST_PATH = self.root / "scripts" / "agents_catalog_manifest.json"

    def tearDown(self) -> None:
        checker.ROOT = self.original_root
        checker.AGENTS_ROOT = self.original_agents_root
        checker.MANIFEST_PATH = self.original_manifest_path
        self.tempdir.cleanup()

    def build_minimal_catalog(self, manifest: dict[str, dict[str, str]]) -> None:
        write(self.root / "AGENTS.md", "- `.agents/core/README.md`\n")
        write(self.agents / "README.md", "# Catalog\n")
        write(
            self.agents / "core" / "README.md",
            "# Core\n\n| Need | Module | Detailed card | Source | Status |\n|---|---|---|---|---|\n| demo | `demo` | `.agents/core/demo.md` | `core/demo.h` | detailed |\n",
        )
        write(
            self.agents / "core" / "demo.md",
            "# demo\n\n```text\ncore/demo.h\n```\n",
        )
        write(self.root / "core" / "demo.h", "#pragma once\n")
        write(self.root / "core" / "demo.c", "void demo(void) {}\n")
        write(self.checker_manifest_path(), json.dumps(manifest, indent=2))

    def checker_manifest_path(self) -> Path:
        return self.root / "scripts" / "agents_catalog_manifest.json"

    def test_valid_catalog_passes(self) -> None:
        self.build_minimal_catalog(
            {
                "demo": {
                    "header": "core/demo.h",
                    "card": ".agents/core/demo.md",
                    "mapper": ".agents/core/README.md",
                    "shared_source": "core/demo.c",
                    "shared_pattern": "implementation",
                }
            }
        )

        errors, modules = checker.validate_cards_and_modules()
        self.assertEqual(errors, [])
        self.assertEqual(modules, 1)

        errors, routes = checker.validate_manifest()
        self.assertEqual(errors, [])
        self.assertEqual(routes, 1)

    def test_missing_card_fails(self) -> None:
        write(self.root / "AGENTS.md", "- `.agents/core/README.md`\n")
        write(
            self.agents / "core" / "README.md",
            "# Core\n\n| Need | Module | Detailed card | Source | Status |\n|---|---|---|---|---|\n| demo | `demo` | `.agents/core/demo.md` | `core/demo.h` | detailed |\n",
        )
        write(self.root / "core" / "demo.h", "#pragma once\n")

        errors, _ = checker.validate_cards_and_modules()
        self.assertIn("missing card: core/demo.h", errors)

    def test_orphan_card_fails(self) -> None:
        self.build_minimal_catalog(
            {
                "demo": {
                    "header": "core/demo.h",
                    "card": ".agents/core/demo.md",
                    "mapper": ".agents/core/README.md",
                    "shared_source": "core/demo.c",
                    "shared_pattern": "implementation",
                }
            }
        )
        write(self.agents / "core" / "orphan.md", "# orphan\n")

        errors = checker.orphan_cards()
        self.assertIn("orphan card: .agents/core/orphan.md", errors)

    def test_broken_mapper_link_fails(self) -> None:
        write(self.root / "AGENTS.md", "- `.agents/core/README.md`\n")
        write(self.agents / "core" / "README.md", "# Core\n\n[Broken](.agents/core/missing.md)\n")
        write(self.root / "core" / "demo.h", "#pragma once\n")

        errors = checker.validate_links()
        self.assertTrue(any("broken link" in error for error in errors))

    def test_include_wrapper_classification_passes(self) -> None:
        self.build_minimal_catalog(
            {
                "demo": {
                    "header": "core/demo.h",
                    "card": ".agents/core/demo.md",
                    "mapper": ".agents/core/README.md",
                    "shared_source": "core/demo.c",
                    "xc8_source": "XC8/core/demo.c",
                    "c18_source": "C18/core/demo.c",
                    "shared_pattern": "implementation",
                    "xc8_pattern": "include-wrapper",
                    "c18_pattern": "include-wrapper",
                }
            }
        )
        write(self.root / "XC8" / "core" / "demo.c", '#include "../../core/demo.c"\n')
        write(self.root / "C18" / "core" / "demo.c", '#include "../../core/demo.c"\n')

        errors, routes = checker.validate_manifest()
        self.assertEqual(errors, [])
        self.assertEqual(routes, 1)

    def test_independent_classification_passes(self) -> None:
        self.build_minimal_catalog(
            {
                "demo": {
                    "header": "core/demo.h",
                    "card": ".agents/core/demo.md",
                    "mapper": ".agents/core/README.md",
                    "shared_source": "core/demo.c",
                    "xc8_source": "XC8/core/demo.c",
                    "c18_source": "C18/core/demo.c",
                    "shared_pattern": "implementation",
                    "xc8_pattern": "independent",
                    "c18_pattern": "independent",
                }
            }
        )
        write(self.root / "XC8" / "core" / "demo.c", "void demo_xc8(void) {}\n")
        write(self.root / "C18" / "core" / "demo.c", "void demo_c18(void) {}\n")

        errors, routes = checker.validate_manifest()
        self.assertEqual(errors, [])
        self.assertEqual(routes, 1)

    def test_route_mismatch_fails(self) -> None:
        self.build_minimal_catalog(
            {
                "demo": {
                    "header": "core/demo.h",
                    "card": ".agents/core/demo.md",
                    "mapper": ".agents/core/README.md",
                    "shared_source": "core/demo.c",
                    "xc8_source": "XC8/core/demo.c",
                    "shared_pattern": "implementation",
                    "xc8_pattern": "include-wrapper",
                }
            }
        )
        write(self.root / "XC8" / "core" / "demo.c", "void demo_xc8(void) {}\n")

        errors, _ = checker.validate_manifest()
        self.assertTrue(any("route mismatch: demo XC8" in error for error in errors))


if __name__ == "__main__":
    unittest.main()
