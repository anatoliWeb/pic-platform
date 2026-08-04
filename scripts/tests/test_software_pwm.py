from __future__ import annotations

import unittest
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
PROJECT = ROOT / "examples-projects" / "xc8" / "output" / "software_pwm.X"


class SoftwarePwmSmokeTests(unittest.TestCase):
    def test_project_exists(self) -> None:
        self.assertTrue(PROJECT.is_dir())

    def test_main_c_does_not_depend_on_zero_cross(self) -> None:
        text = (PROJECT / "main.c").read_text(encoding="utf-8-sig")
        self.assertNotIn("zero_cross", text)

    def test_readme_exists(self) -> None:
        self.assertTrue((PROJECT / "README.md").is_file())
        self.assertTrue((PROJECT / "README.ua.md").is_file())


if __name__ == "__main__":
    unittest.main(verbosity=2)
