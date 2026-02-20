import sys
from pathlib import Path

import fire  # noqa: E402

from hi import main as hi_cmd
from gen import main as gen_cmd

def main():
    """Главная функция для CLI команд."""
    fire.Fire(
        {
            "hi": hi_cmd.hi,
            "gen": gen_cmd.gen,
        }
    )


if __name__ == "__main__":
    main()