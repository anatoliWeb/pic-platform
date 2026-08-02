from __future__ import annotations

import contextlib
import importlib
import io
import json
import tempfile
import unittest
from pathlib import Path


checker = importlib.import_module("scripts.check_agents_catalog")


def write(path: Path, text: str) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(text, encoding="utf-8")


def run_main() -> tuple[int, str]:
    buf = io.StringIO()
    with contextlib.redirect_stdout(buf):
        code = checker.main()
    return code, buf.getvalue()


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

    def checker_manifest_path(self) -> Path:
        return self.root / "scripts" / "agents_catalog_manifest.json"

    def write_manifest(self, manifest: dict[str, dict[str, str]]) -> None:
        write(self.checker_manifest_path(), json.dumps(manifest, indent=2))

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
        write(self.root / "XC8" / "core" / "demo.c", "void demo_xc8(void) {}\n")
        write(self.root / "C18" / "core" / "demo.c", "void demo_c18(void) {}\n")
        self.write_manifest(manifest)

    def demo_manifest(self) -> dict[str, dict[str, str]]:
        return {
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

    def test_valid_catalog_passes(self) -> None:
        self.build_minimal_catalog(self.demo_manifest())

        errors, modules = checker.validate_cards_and_modules()
        self.assertEqual(errors, [])
        self.assertEqual(modules, 1)

        errors, routes = checker.validate_manifest()
        self.assertEqual(errors, [])
        self.assertEqual(routes, 1)

    def test_main_passes_on_valid_catalog(self) -> None:
        self.build_minimal_catalog(self.demo_manifest())
        code, out = run_main()
        self.assertEqual(code, 0)
        self.assertIn("agents catalog ok", out)
        self.assertIn("orphan cards checked: 0", out)

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
        self.build_minimal_catalog(self.demo_manifest())
        write(self.agents / "core" / "orphan.md", "# orphan\n")

        errors = checker.orphan_cards()
        self.assertIn("orphan card: .agents/core/orphan.md", errors)

    def test_main_fails_when_orphan_card_exists(self) -> None:
        self.build_minimal_catalog(self.demo_manifest())
        write(self.agents / "core" / "orphan.md", "# orphan\n")

        code, out = run_main()
        self.assertNotEqual(code, 0)
        self.assertIn("orphan card", out)

    def test_main_fails_when_manifest_missing_entry(self) -> None:
        manifest = self.demo_manifest()
        del manifest["demo"]
        self.build_minimal_catalog(manifest)

        code, out = run_main()
        self.assertNotEqual(code, 0)
        self.assertIn("missing manifest entry: demo", out)

    def test_main_fails_when_stale_manifest_entry(self) -> None:
        manifest = self.demo_manifest()
        manifest["stale_demo"] = {
            "header": "core/stale_demo.h",
            "card": ".agents/core/stale_demo.md",
            "mapper": ".agents/core/README.md",
            "shared_source": "core/stale_demo.c",
            "shared_pattern": "implementation",
        }
        self.build_minimal_catalog(manifest)

        code, out = run_main()
        self.assertNotEqual(code, 0)
        self.assertIn("stale manifest entry: stale_demo", out)

    def test_main_fails_when_mapper_mismatch(self) -> None:
        self.build_minimal_catalog(self.demo_manifest())
        # point mapper at an existing file that never lists the module card
        write(self.agents / "sensors" / "README.md", "# Sensors\n\nNo demo module here.\n")
        manifest = self.demo_manifest()
        manifest["demo"]["mapper"] = ".agents/sensors/README.md"
        self.write_manifest(manifest)

        code, out = run_main()
        self.assertNotEqual(code, 0)
        self.assertIn("manifest mapper mismatch", out)

    def test_duplicate_module_id_fails(self) -> None:
        self.build_minimal_catalog(self.demo_manifest())
        # a second real module header whose short name collides applies "demo"
        write(self.root / "drivers" / "analog" / "demo" / "demo.h", "#pragma once\n")
        write(self.root / "drivers" / "analog" / "demo" / "demo.c", "void demo(void) {}\n")

        code, out = run_main()
        self.assertNotEqual(code, 0)
        self.assertIn("duplicate discovered module id: demo", out)

    def test_broken_mapper_link_fails(self) -> None:
        write(self.root / "AGENTS.md", "- `.agents/core/README.md`\n")
        write(self.agents / "core" / "README.md", "# Core\n\n[Broken](.agents/core/missing.md)\n")
        write(self.root / "core" / "demo.h", "#pragma once\n")

        errors = checker.validate_links()
        self.assertTrue(any("broken link" in error for error in errors))

    def test_include_wrapper_classification_passes(self) -> None:
        manifest = self.demo_manifest()
        manifest["demo"]["xc8_pattern"] = "include-wrapper"
        manifest["demo"]["c18_pattern"] = "include-wrapper"
        self.build_minimal_catalog(manifest)
        write(self.root / "XC8" / "core" / "demo.c", '#include "../../core/demo.c"\n')
        write(self.root / "C18" / "core" / "demo.c", '#include "../../core/demo.c"\n')

        errors, routes = checker.validate_manifest()
        self.assertEqual(errors, [])
        self.assertEqual(routes, 1)

    def test_independent_classification_passes(self) -> None:
        self.build_minimal_catalog(self.demo_manifest())

        errors, routes = checker.validate_manifest()
        self.assertEqual(errors, [])
        self.assertEqual(routes, 1)

    def test_route_mismatch_fails(self) -> None:
        manifest = self.demo_manifest()
        manifest["demo"]["xc8_pattern"] = "include-wrapper"
        self.build_minimal_catalog(manifest)
        write(self.root / "XC8" / "core" / "demo.c", "void demo_xc8(void) {}\n")

        errors, _ = checker.validate_manifest()
        self.assertTrue(any("route mismatch: demo XC8" in error for error in errors))

    def test_main_fails_when_route_mismatch(self) -> None:
        manifest = self.demo_manifest()
        manifest["demo"]["xc8_pattern"] = "include-wrapper"
        self.build_minimal_catalog(manifest)
        write(self.root / "XC8" / "core" / "demo.c", "void demo_xc8(void) {}\n")

        code, out = run_main()
        self.assertNotEqual(code, 0)
        self.assertIn("route mismatch", out)

    def test_main_fails_when_mapper_card_conflict(self) -> None:
        self.build_minimal_catalog(self.demo_manifest())
        # mapper lists demo -> a different card than the manifest points to
        write(
            self.agents / "core" / "wrong.md",
            "# wrong demo\n\n```text\ncore/demo.h\n```\n",
        )
        write(
            self.agents / "core" / "README.md",
            "# Core\n\n| Need | Module | Detailed card | Source | Status |\n|---|---|---|---|---|\n| demo | `demo` | `.agents/core/wrong.md` | `core/demo.h` | detailed |\n",
        )
        write(self.agents / "core" / "wrong.md", "# wrong demo\n")

        code, out = run_main()
        self.assertNotEqual(code, 0)
        self.assertIn("manifest card mismatch", out)

    def test_mapper_mentions_module_only_in_prose(self) -> None:
        # The module card is never referenced by any table row in the mapper,
        # even though the module name appears in prose.
        self.build_minimal_catalog(self.demo_manifest())
        write(
            self.agents / "core" / "README.md",
            "# Core\n\nThis doc mentions demo in prose but never lists it in a table.\n",
        )
        self.write_manifest(self.demo_manifest())

        code, out = run_main()
        self.assertNotEqual(code, 0)
        self.assertIn("manifest mapper mismatch: demo", out)

    def test_mapper_source_mismatch_fails(self) -> None:
        # Mapper row points to the right card but lists a delta source file.
        self.build_minimal_catalog(self.demo_manifest())
        write(
            self.agents / "core" / "README.md",
            "# Core\n\n| Need | Module | Detailed card | Source | Status |\n|---|---|---|---|---|\n| demo | `demo` | `.agents/core/demo.md` | `core/other.h` | detailed |\n",
        )

        code, out = run_main()
        self.assertNotEqual(code, 0)
        self.assertIn("manifest source mismatch: demo", out)

    def test_absent_route_fails_when_xc8_source_exists(self) -> None:
        # Manifest says XC8 is absent for demo, but a real XC8 source exists.
        manifest = self.demo_manifest()
        del manifest["demo"]["xc8_source"]
        del manifest["demo"]["c18_source"]
        manifest["demo"]["xc8_pattern"] = "absent"
        self.build_minimal_catalog(manifest)
        write(self.root / "XC8" / "core" / "demo.c", "void demo_xc8(void) {}\n")

        errors, _ = checker.validate_manifest()
        self.assertTrue(any("route mismatch: demo XC8" in error for error in errors))

    def test_absent_route_source_fails_when_c18_source_exists(self) -> None:
        manifest = self.demo_manifest()
        del manifest["demo"]["xc8_source"]
        del manifest["demo"]["c18_source"]
        manifest["demo"]["c18_pattern"] = "absent"
        self.build_minimal_catalog(manifest)
        write(self.root / "C18" / "core" / "demo.c", "void demo_c18(void) {}\n")

        errors, _ = checker.validate_manifest()
        self.assertTrue(any("route mismatch: demo C18" in error for error in errors))

    def test_exempt_header_is_skipped(self) -> None:
        # An exempt public header produces no module and no manifest demand.
        self.build_minimal_catalog(self.demo_manifest())
        write(self.root / "core" / "debug.h", "#pragma once\n")

        code, out = run_main()
        self.assertEqual(code, 0)
        self.assertIn("agents catalog ok", out)

    def test_header_only_reusable_requires_manifest(self) -> None:
        # A reusable core header without any source must still get a manifest entry.
        self.build_minimal_catalog(self.demo_manifest())
        write(self.root / "core" / "types.h", "#pragma once\n")

        code, out = run_main()
        self.assertNotEqual(code, 0)
        self.assertIn("missing manifest entry: types", out)

    def test_report_distinguishes_headers_from_modules(self) -> None:
        # Two headers in the same module dir count once at the module level.
        self.build_minimal_catalog(self.demo_manifest())
        write(self.root / "drivers" / "wheel" / "wheel.h", "#pragma once\n")
        write(self.root / "drivers" / "wheel" / "inner.h", "#pragma once\n")

        modules, module_errors = checker.discover_modules()
        self.assertEqual(module_errors, [])
        self.assertEqual(len(checker.discover_headers()), 3)
        self.assertEqual(len([m for m in modules if m == "wheel"]), 1)

    def test_main_calls_orphan_validation_once(self) -> None:
        self.build_minimal_catalog(self.demo_manifest())
        calls = []

        original = checker.orphan_cards
        checker.orphan_cards = lambda: (calls.append(1) or [])
        try:
            code, out = run_main()
        finally:
            checker.orphan_cards = original

        self.assertEqual(code, 0)
        self.assertEqual(calls, [1])


if __name__ == "__main__":
    unittest.main()