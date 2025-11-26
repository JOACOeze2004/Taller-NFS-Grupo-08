#!/usr/bin/env bash
GAME_NAME="need_for_speed"
BIN_SERVER_NAME="${GAME_NAME}-server"

export NEED_FOR_SPEED_SERVER_CONFIG_FILE="/etc/need_for_speed/server_config.yaml"

cd "$(dirname "${BASH_SOURCE[0]}")/.."

USE_VALGRIND=false
PORT=8080

if [[ "$1" == "--valgrind" ]]; then
    USE_VALGRIND=true
    PORT="${2:-8080}"
else
    PORT="${1:-8080}"
fi

if $USE_VALGRIND; then
    echo "[SERVER] Running under Valgrind..."
    exec valgrind --leak-check=full --track-origins=yes --show-reachable=yes --error-exitcode=2 --show-leak-kinds=all --trace-children=yes /usr/bin/$BIN_SERVER_NAME "$PORT"
else
    exec /usr/bin/$BIN_SERVER_NAME "$PORT"
fi