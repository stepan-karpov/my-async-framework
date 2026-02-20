import subprocess

from container.utils import get_container_names

def rm(service_name: str):
  prefix = f"{service_name}_"
  names = get_container_names.get_container_names()
  names = list(filter(lambda n: n.startswith(prefix), names))

  for name in names:
    subprocess.run(["sudo", "docker", "rm", "-f", name], check=True)
    print("Removed", name)

  if not names:
    print(f"No containers matching {prefix}*")