import subprocess

def get_container_names():
  result = subprocess.run(
    ["sudo", "docker", "ps", "-a", "--format", "{{.Names}}"],
    capture_output=True,
    text=True,
  )
  if result.returncode != 0:
    raise RuntimeError(
      f"docker ps failed: {result.stderr or result.stdout or 'unknown error'}"
    )
  names = [
    n.strip()
    for n in (result.stdout or "").strip().splitlines()
    if n.strip()
  ]
  return names