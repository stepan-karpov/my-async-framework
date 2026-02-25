import sys
from pathlib import Path

import fire  # noqa: E402

from gen import main as gen_cmd
from hi import main as hi_cmd

from container import up, rm, enter
from framework import image, build, test

def main():
    """Главная функция для CLI команд."""
    fire.Fire(
        {
            "hi": hi_cmd.hi,
            "gen": gen_cmd.gen,
            "container": {
                "up": up.up,
                "rm": rm.rm,
                "enter": enter.enter
            },
            "framework": {
                "image": image.image,
                "build": build.build,
                "test": test.test,
            }
        }
    )


if __name__ == "__main__":
    main()