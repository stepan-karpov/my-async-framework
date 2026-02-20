#!/usr/bin/env bash
set -e

# Directory where this installer (yamo repo) lives
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
INSTALL_DIR="${HOME}/.yamo"
LAUNCHER="/usr/local/bin/yamo"

echo "Installing yamo to ${INSTALL_DIR}"

# 1. Copy the whole folder to ~/.yamo
mkdir -p "$INSTALL_DIR"
cp -R "${SCRIPT_DIR}/." "${INSTALL_DIR}/"

# 2. Add current framework path to ~/.yamo
echo "FRAMEWORK_PATH=\"$(dirname "${SCRIPT_DIR}")\"" > ~/.yamo/framework_path.py

# 3. Virtual environment in ~/.yamo
echo "Creating virtual environment..."
python3 -m venv "${INSTALL_DIR}/venv"
"${INSTALL_DIR}/venv/bin/pip" install -q --upgrade pip
"${INSTALL_DIR}/venv/bin/pip" install -q fire

# 4–5. Launcher in /usr/local/bin (available as yamo)
echo "Installing launcher to ${LAUNCHER} (sudo required)"
sudo cp "${SCRIPT_DIR}/yamo-bin.sh" "$LAUNCHER"
sudo chmod +x "$LAUNCHER"

echo "Done. Run: yamo hi"