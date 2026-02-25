import subprocess

from framework_path import FRAMEWORK_PATH

FRAMEWORK_IMAGE_NAME = "my-async-framework"

def test():
    subprocess.run(
        [
            "sudo",
            "docker",
            "build",
            "-f",
            "my-async-framework/Dockerfile",
            "-t",
            FRAMEWORK_IMAGE_NAME,
            ".",
        ],
        cwd=FRAMEWORK_PATH,
        check=True,
    )
    print(f"Image {FRAMEWORK_IMAGE_NAME} built successfully.")
