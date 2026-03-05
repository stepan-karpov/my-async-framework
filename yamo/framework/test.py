
import pexpect
import random
import subprocess

from framework_path import FRAMEWORK_PATH

FRAMEWORK_IMAGE_NAME = "my-async-framework-test"

def test():
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
