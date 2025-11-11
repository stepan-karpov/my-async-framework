"""
Скрипт для обновления команды запуска C++ в settings.json
"""

import json
import os
from pathlib import Path

SETTINGS_PATH = Path(__file__).parent / "settings.json"

# f8 execution command
CPP_COMMAND = """
if [[ "$dir" == *".sandbox"* ]]; then
  cd "$dir" &&
  clang++ -O2 -fsanitize=undefined 
              -fsanitize=address
              -std=c++20 
              "$fileName" -o "$fileNameWithoutExt.trash" &&
  ./"$fileNameWithoutExt.trash";
else
  cd "$workspaceRoot/build" &&
  cmake .. &&
  cmake --build . &&
  ./my-server/my-server-bin;
fi
"""


if __name__ == "__main__":
  with open(SETTINGS_PATH, 'r', encoding='utf-8') as f:
    settings = json.load(f)
    settings["code-runner.executorMap"]["cpp"] = CPP_COMMAND.replace("\n", "")
  with open(SETTINGS_PATH, 'w', encoding='utf-8') as f:
    json.dump(settings, f, indent=2, ensure_ascii=False)

