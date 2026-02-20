import subprocess

from container.utils import get_container_names

def enter(service_name: str):
  prefix = f"{service_name}_"
  names = get_container_names.get_container_names()
  names = list(filter(lambda n: n.startswith(prefix), names))

  if len(names) == 0:
    print("No containers found")
    return
  if len(names) > 1:
    print("More than 1 container found. Please, clarify what container to choose")
    return

  container = names[0]
  subprocess.run(["sudo", "docker", "exec", "-it", container, "bash"])
