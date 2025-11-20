#!/usr/bin/env bash
GAME_NAME="need_for_speed"
BIN_SERVER_NAME="${GAME_NAME}-server"

export NEED_FOR_SPEED_SERVER_CONFIG_FILE="/etc/need_for_speed/server_config.yaml"

cd "$(dirname "${BASH_SOURCE[0]}")/.."

PORT="${1:-8080}"

exec /usr/bin/$BIN_SERVER_NAME "$PORT"