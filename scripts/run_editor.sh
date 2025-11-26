#!/usr/bin/env bash
GAME_NAME="need_for_speed"
BIN_EDITOR_NAME="${GAME_NAME}-editor"
VAR_DIR="/var/$GAME_NAME"
CFG_DIR="/etc/$GAME_NAME"

# Set working directory to /var/need_for_speed/game where assets/src are located
cd "$VAR_DIR/game" || cd /var/need_for_speed/game || {
  echo "Error: Cannot change to game directory"
  exit 1
}

# Export config file location
export NEED_FOR_SPEED_EDITOR_CONFIG_FILE="$CFG_DIR/editor_config.yaml"

# Export asset paths for the game to use
export NEED_FOR_SPEED_ASSETS_DIR="$VAR_DIR/game/assets"
export NEED_FOR_SPEED_SRC_DIR="$VAR_DIR/game/src"

echo "================================================"
echo "  Need For Speed - Editor Launcher"
echo "================================================"
echo "Working directory: $(pwd)"
echo "Assets directory: $NEED_FOR_SPEED_ASSETS_DIR"
echo "Source directory: $NEED_FOR_SPEED_SRC_DIR"
echo "Config file: $NEED_FOR_SPEED_EDITOR_CONFIG_FILE"
echo "================================================"
echo

# Launch editor from the game directory (so relative paths work)
exec /usr/bin/$BIN_EDITOR_NAME "$@"