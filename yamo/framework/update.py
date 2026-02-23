import subprocess

from framework_path import FRAMEWORK_PATH

FRAMEWORK_IMAGE_NAME = "my-async-framework"
FRAMEWORK_DOCKERFILE = "my-async-framework/Dockerfile"


def update():
    subprocess.run(
        [
            "sudo",
            "docker",
            "build",
            "-f",
            FRAMEWORK_DOCKERFILE,
            "-t",
            FRAMEWORK_IMAGE_NAME,
            ".",
        ],
        cwd=FRAMEWORK_PATH,
        check=True,
    )
    print(f"Image {FRAMEWORK_IMAGE_NAME} built successfully.")
