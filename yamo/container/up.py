import random
import subprocess
from pathlib import Path

from framework_path import FRAMEWORK_PATH

from container import enter

def up(service_name: str, e: bool = False):
  service_dir = Path(FRAMEWORK_PATH) / "services" / service_name
  if not service_dir.is_dir():
    print(f"Service directory not found: {service_dir}")
    return

  # 1. docker build from repo root so COPY my-async-framework works
  subprocess.run(
    ["sudo", "docker", "build", "-f", f"services/{service_name}/Dockerfile", "-t", service_name, "."],
    cwd=FRAMEWORK_PATH,
    check=True,
  )

  # 2. docker create with a unique container name
  container_name = f"{service_name}_{random.randint(10000, 99999)}"
  subprocess.run(
      ["sudo", "docker", "run", "-d", "--name", container_name,
      "-p", "8082:8082",
      service_name],   # без "sleep", "infinity"
      check=True,
  )

  if e:
    enter.enter(service_name)
