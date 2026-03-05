import os
import pexpect
import random
import subprocess
from pathlib import Path

from framework_path import FRAMEWORK_PATH

from framework import build

FRAMEWORK_IMAGE_NAME = "my-async-framework-test"

def run_in_container(r: bool):
    if r:
        subprocess.run(
            [
                "sudo",
                "docker",
                "build",
                "-f",
                "my-async-framework/Dockerfile.test",
                "-t",
                FRAMEWORK_IMAGE_NAME,
                ".",
            ],
            cwd=FRAMEWORK_PATH,
            check=True,
        )
        print(f"Image {FRAMEWORK_IMAGE_NAME} built successfully.")

    container_name = f"{FRAMEWORK_IMAGE_NAME}_test_{random.randint(10000, 99999)}"
    subprocess.run(
        [
            "sudo",
            "docker",
            "run",
            "-d",
            "--name",
            container_name,
            FRAMEWORK_IMAGE_NAME,
            "sleep",
            "infinity",
        ],
        check=True,
    )
    print(f"Started container {container_name} from image {FRAMEWORK_IMAGE_NAME}")

    child = pexpect.spawn(
        "sudo docker exec -it %s bash" % container_name,
        encoding="utf-8",
    )
    child.send("ctest")
    child.interact()

    subprocess.run(["sudo", "docker", "rm", "-f", container_name], check=False)
    print(f"Removed container {container_name}")

def run_in_terminal(r: bool):
    if r:
        build.build(t=True)

    build_dir = Path(FRAMEWORK_PATH) / "my-async-framework" / "build"
    if not build_dir.is_dir():
        print(f"Build directory not found: {build_dir}. Run with r=True to build.")
        return

    cmd = f'cd "{build_dir}" && ctest'
    os.system(f'{os.environ.get("SHELL", "/bin/bash")} -i -c \'{cmd}; exec $SHELL\'')

def test(c: bool = False, r: bool = False):
    """Запускает тесты фреймворка

        Args:
            c: Запустить тесты в контейнере
            r: Собрать фреймворк
    """

    if c:
        run_in_container(r)
    else:
        run_in_terminal(r)
