#!/usr/bin/env bash
GAME_NAME="need_for_speed"
BIN_EDITOR_NAME="${GAME_NAME}-editor"
VAR_DIR="/var/$GAME_NAME"
CFG_DIR="/etc/$GAME_NAME"

cd "$VAR_DIR/game" || cd /var/need_for_speed/game || {
  echo "Error: Cannot change to game directory"
  exit 1
}

export NEED_FOR_SPEED_EDITOR_CONFIG_FILE="$CFG_DIR/editor_config.yaml"

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

exec /usr/bin/$BIN_EDITOR_NAME "$@"