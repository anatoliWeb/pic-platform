from __future__ import annotations

import unittest
import xml.etree.ElementTree as ET
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
XC8 = ROOT / "examples-projects" / "xc8"

FAMILIES: dict[str, list[str]] = {
    "debug": ["default_uart", "disabled", "display_i2c", "display_parallel", "multi_backend", "pins_gpio"],
    "communication/uart": ["basic", "debug"],
    "communication/i2c": ["basic", "scan"],
    "communication/spi": ["basic", "loopback"],
    "communication/rs485": ["basic", "echo"],
    "communication/onewire": ["basic", "bus_test"],
    "sensors/ds18b20": ["basic", "read", "multi_read", "search_rom"],
    "display/lcd_hd44780": ["i2c_pcf8574"],
    "input": ["button", "encoder", "adc_buttons"],
    "output": ["software_pwm", "ac_phase_control"],
    "timers": ["tick", "timer0", "timer1", "timer2", "timer3"],
    "system": ["wdt"],
}

SEVEN_SEGMENT = [
    "basic_manual",
    "multiplex_manual",
    "multiplex_timer",
    "keys_single_line",
    "keys_diode_coded",
]

STALE_FLAT_PROJECTS = [
    "debug_default_uart.X",
    "debug_disabled.X",
    "debug_display_i2c.X",
    "debug_display_parallel.X",
    "debug_multi_backend.X",
    "debug_pins_gpio.X",
    "ds18b20.X",
    "ds18b20_multi_read.X",
    "ds18b20_read.X",
    "ds18b20_search_rom.X",
    "onewire.X",
    "onewire_bus_test.X",
    "button.X",
    "encoder.X",
    "adc_buttons.X",
    "uart.X",
    "uart_debug.X",
    "spi.X",
    "spi_loopback.X",
    "i2c.X",
    "i2c_scan.X",
    "rs485_basic.X",
    "rs485_echo.X",
    "tick.X",
    "timer0.X",
    "timer1.X",
    "timer2.X",
    "timer3.X",
    "software_pwm.X",
    "ac_phase_control.X",
    "wdt.X",
]

IGNORED_DOC_DIRS = {".git", "build", "dist", "__pycache__", "nbproject"}


def config_sources(xml_path: Path) -> list[str]:
    root = ET.fromstring(xml_path.read_text(encoding="utf-8-sig"))
    source_folder = root.find(".//logicalFolder[@name='SourceFiles']")
    if source_folder is None:
        raise AssertionError(f"SourceFiles folder not found in {xml_path}")
    return [item.text for item in source_folder.iter("itemPath") if item.text]


class FamilyStructureTests(unittest.TestCase):
    def test_families_and_projects_exist(self) -> None:
        for family, projects in FAMILIES.items():
            family_dir = XC8 / family
            with self.subTest(family=family):
                self.assertTrue(family_dir.is_dir(), f"missing family dir {family}")
                for project in projects:
                    self.assertTrue(
                        (family_dir / f"{project}.X").is_dir(),
                        f"missing {family}/{project}.X",
                    )

    def test_debug_family_has_six_examples(self) -> None:
        debug_dir = XC8 / "debug"
        projects = [p.name for p in debug_dir.iterdir() if p.is_dir() and p.name.endswith(".X")]
        self.assertEqual(len(projects), 6)
        self.assertEqual(set(projects), {f"{n}.X" for n in FAMILIES["debug"]})

    def test_ds18b20_final_structure(self) -> None:
        ds18b20_dir = XC8 / "sensors" / "ds18b20"
        projects = {p.name for p in ds18b20_dir.iterdir() if p.is_dir() and p.name.endswith(".X")}
        self.assertEqual(projects, {f"{n}.X" for n in FAMILIES["sensors/ds18b20"]})

    def test_onewire_final_structure(self) -> None:
        onewire_dir = XC8 / "communication" / "onewire"
        projects = {p.name for p in onewire_dir.iterdir() if p.is_dir() and p.name.endswith(".X")}
        self.assertEqual(projects, {f"{n}.X" for n in FAMILIES["communication/onewire"]})

    def test_seven_segment_family_not_broken(self) -> None:
        seven_dir = XC8 / "seven_segment"
        self.assertTrue(seven_dir.is_dir())
        projects = {p.name for p in seven_dir.iterdir() if p.is_dir() and p.name.endswith(".X")}
        self.assertEqual(projects, {f"{n}.X" for n in SEVEN_SEGMENT})

    def test_stale_flat_project_dirs_absent(self) -> None:
        root_projects = {p.name for p in XC8.iterdir() if p.is_dir() and p.name.endswith(".X")}
        for stale in STALE_FLAT_PROJECTS:
            with self.subTest(stale=stale):
                self.assertNotIn(stale, root_projects, f"stale flat project {stale} still at xc8 root")

    def test_project_identities_unique(self) -> None:
        seen: set[str] = set()
        for family, projects in FAMILIES.items():
            for project in projects:
                name = f"{family}/{project}.X"
                self.assertNotIn(name, seen, f"duplicate project identity {name}")
                seen.add(name)

    def test_relative_source_paths_exist(self) -> None:
        in_project_files = {"main.c", "config_bits.c", "project_config.h", "Makefile"}
        for family, projects in FAMILIES.items():
            for project in projects:
                xml_path = XC8 / family / f"{project}.X" / "nbproject" / "configurations.xml"
                with self.subTest(project=f"{family}/{project}.X"):
                    self.assertTrue(xml_path.is_file(), f"missing {xml_path}")
                    project_dir = xml_path.parents[1]
                    for source in config_sources(xml_path):
                        if source in in_project_files:
                            continue
                        self.assertTrue(
                            (project_dir / source).is_file(),
                            f"missing relative source {source} in {family}/{project}.X",
                        )

    def test_no_build_dist_private_output_under_families(self) -> None:
        for family in FAMILIES:
            family_dir = XC8 / family
            for output in ("build", "dist", "nbproject/private"):
                self.assertFalse(
                    (family_dir / output).exists(),
                    f"untracked output {output} present in {family}",
                )


class StalePathInDocsTests(unittest.TestCase):
    def test_docs_do_not_reference_stale_flat_project_paths(self) -> None:
        stale_patterns = [f"xc8/{name}" for name in STALE_FLAT_PROJECTS]
        stale_patterns.append("xc8/debug_*.X")

        for path in ROOT.rglob("*.md"):
            rel = path.relative_to(ROOT).as_posix()
            if any(part in IGNORED_DOC_DIRS for part in path.parts):
                continue
            text = path.read_text(encoding="utf-8-sig", errors="ignore")
            for pattern in stale_patterns:
                with self.subTest(path=rel, pattern=pattern):
                    self.assertNotIn(pattern, text, f"stale path {pattern} in {rel}")


if __name__ == "__main__":
    unittest.main()
