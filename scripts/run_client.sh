#!/usr/bin/env bash
GAME_NAME="need_for_speed"
BIN_CLIENT_NAME="${GAME_NAME}-client"

export NEED_FOR_SPEED_CLIENT_CONFIG_FILE="/etc/need_for_speed/client_config.yaml"

cd "$(dirname "${BASH_SOURCE[0]}")/.."

HOST="${1:-localhost}"
PORT="${2:-8080}"

exec /usr/bin/$BIN_CLIENT_NAME "$HOST" "$PORT"
