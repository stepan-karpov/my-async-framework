import subprocess
from pathlib import Path

from framework_path import FRAMEWORK_PATH

def build():
    framework_path = Path(FRAMEWORK_PATH) / "my-async-framework"
    if not framework_path.is_dir():
        print(f"Framework directory not found: {framework_path}")
        return

    # 1. Local build (cmake + build)
    subprocess.run(
        ["cmake", "-S", ".", "-B", "build", "-DCMAKE_EXPORT_COMPILE_COMMANDS=ON", "-DLOG_LEVEL=0"],
        cwd=framework_path,
        check=True,
    )
    subprocess.run(
        ["cmake", "--build", "build"],
        cwd=framework_path,
        check=True,
    )
    print("Framework built locally.")
