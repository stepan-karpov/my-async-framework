"""
Script to update C++ execution command in settings.json
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
elif [[ "$dir" == *"my-async-framework/tests"* ]]; then
  cd "$workspaceRoot/my-async-framework" &&
  cmake -S . -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DBUILD_TESTING=ON -DLOG_LEVEL=2 -DSANITIZER=address,undefined &&
  cmake --build build &&
  cd build &&
  ctest;
else
  cd "$workspaceRoot/my-async-framework" &&
  cmake -S . -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DBUILD_TESTING=ON -DLOG_LEVEL=0 &&
  cmake --build build &&
  cmake --install build --prefix ~/local/my-async-framework &&

  cd "$workspaceRoot/services/simple-tcp-server" &&
  cmake -S . -B build -DCMAKE_PREFIX_PATH=~/local/my-async-framework -DCMAKE_EXPORT_COMPILE_COMMANDS=ON &&
  cmake --build build &&
  ./build/SimpleTCPServer;
fi
"""


if __name__ == "__main__":
  with open(SETTINGS_PATH, 'r', encoding='utf-8') as f:
    settings = json.load(f)
    settings["code-runner.executorMap"]["cpp"] = CPP_COMMAND.replace("\n", "")
  with open(SETTINGS_PATH, 'w', encoding='utf-8') as f:
    json.dump(settings, f, indent=2, ensure_ascii=False)

