from __future__ import annotations

import unittest
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
SKILL_PATH = ROOT / ".skills" / "pic-platform" / "SKILL.md"
AGENTS_PATH = ROOT / "AGENTS.md"


class PicPlatformSkillTests(unittest.TestCase):
    def setUp(self) -> None:
        self.skill = SKILL_PATH.read_text(encoding="utf-8")
        self.agents = AGENTS_PATH.read_text(encoding="utf-8")

    # -- existence and encoding --

    def test_skill_file_exists(self) -> None:
        self.assertTrue(SKILL_PATH.exists(), f"Missing {SKILL_PATH}")

    def test_skill_is_utf8(self) -> None:
        raw = SKILL_PATH.read_bytes()
        try:
            raw.decode("utf-8")
        except UnicodeDecodeError:
            self.fail("SKILL.md is not valid UTF-8")

    def test_skill_ends_with_newline(self) -> None:
        raw = SKILL_PATH.read_bytes()
        self.assertTrue(raw.endswith(b"\n"), "SKILL.md must end with a newline")

    # -- size --

    def test_skill_reasonable_size(self) -> None:
        line_count = len(self.skill.splitlines())
        self.assertGreaterEqual(line_count, 80, "Skill too short to be useful")
        self.assertLessEqual(line_count, 350, "Skill exceeds 350-line target")

    # -- AGENTS.md references skill --

    def test_agents_references_skill(self) -> None:
        self.assertIn("SKILL.md", self.agents, "AGENTS.md must reference the skill")

    # -- skill references key files --

    def test_skill_references_agents_md(self) -> None:
        self.assertIn("AGENTS.md", self.skill)

    def test_skill_references_agents_routing(self) -> None:
        self.assertIn(".agents/", self.skill)

    # -- workflow content --

    def test_skill_contains_existing_first_workflow(self) -> None:
        lower = self.skill.lower()
        self.assertIn("reuse existing", lower)
        self.assertIn("extend existing", lower)
        self.assertIn("consolidate", lower)

    def test_skill_contains_platform_vs_application(self) -> None:
        lower = self.skill.lower()
        self.assertIn("platform code", lower)
        self.assertIn("application code", lower)

    def test_skill_contains_tests_examples_docs_roles(self) -> None:
        lower = self.skill.lower()
        self.assertIn("test", lower)
        self.assertIn("example", lower)
        self.assertIn("doc", lower)

    def test_skill_contains_configuration_rules(self) -> None:
        self.assertIn("CONFIGURATION-RULES.md", self.skill)
        self.assertIn("project_config", self.skill)

    def test_skill_contains_build_rules(self) -> None:
        lower = self.skill.lower()
        self.assertIn("xc8", lower)
        self.assertIn("build", lower)

    def test_skill_contains_git_rules(self) -> None:
        self.assertIn("git add .", self.skill)
        self.assertIn("git status", self.skill)

    # -- must NOT enumerate all modules --

    def test_skill_does_not_list_all_modules(self) -> None:
        module_names = [
            "seven_segment", "button", "encoder", "adc_buttons",
            "ir_receiver", "keypad_matrix", "lcd_hd44780", "ili9341",
            "sh1106", "ssd1306", "st7735", "position_drive",
            "cooldown_output", "software_pwm", "ac_phase_control",
            "zero_cross", "tick", "rtc_framework", "software_rtc",
            "timezone", "ds3231", "ds1307", "ds1302", "pcf8563",
            "ds12c887", "cron_scheduler", "ntp_sync", "uart_debug",
            "debug", "buzzer", "relay", "servo", "digital_input",
            "segment_keys", "shared_segment_buttons", "bitmap",
            "graphics", "display_framework", "seven_segment_mux",
            "tachometer",
        ]
        listed = [m for m in module_names if m in self.skill]
        self.assertLessEqual(
            len(listed), 3,
            f"Skill enumerates too many modules: {listed}. "
            "It should route to .agents/ instead.",
        )

    # -- referenced paths exist --

    def test_referenced_agents_paths_exist(self) -> None:
        paths = [
            ROOT / ".agents" / "10-rules" / "CONFIGURATION-RULES.md",
            ROOT / "docs" / "configuration.md",
            ROOT / "docs" / "configuration.ua.md",
            ROOT / "scripts" / "config_contract.py",
        ]
        for p in paths:
            self.assertTrue(p.exists(), f"Referenced path missing: {p}")


if __name__ == "__main__":
    unittest.main()
