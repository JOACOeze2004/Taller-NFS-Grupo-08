#!/usr/bin/env bash
GAME_NAME="need_for_speed"
BIN_SERVER_NAME="${GAME_NAME}-server"

# Export config file location
export NEED_FOR_SPEED_SERVER_CONFIG_FILE="/etc/need_for_speed/server_config.yaml"

cd "$(dirname "${BASH_SOURCE[0]}")/.."