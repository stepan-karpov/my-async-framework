import subprocess
from pathlib import Path

from framework_path import FRAMEWORK_PATH

def build(t: bool = False):
    """Собрать и установить my-async-framework 

        Args:
            t: Включить тесты в сборку
    """

    framework_path = Path(FRAMEWORK_PATH) / "my-async-framework"
    if not framework_path.is_dir():
        print(f"Framework directory not found: {framework_path}")
        return

    # 1. Local build (cmake + build)
    args = ["cmake", "-S", ".", "-B", "build", "-DCMAKE_EXPORT_COMPILE_COMMANDS=ON", "-DLOG_LEVEL=0"]
    if t:
        args.append("-DBUILD_TESTING=ON")
    else:
        args.append("-DBUILD_TESTING=OFF")

    subprocess.run(
        args,
        cwd=framework_path,
        check=True,
    )
    subprocess.run(
        ["cmake", "--build", "build"],
        cwd=framework_path,
        check=True,
    )
    print("Framework built locally.")
