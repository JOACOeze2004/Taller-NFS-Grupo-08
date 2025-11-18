#!/usr/bin/env bash
GAME_NAME="need_for_speed"
BIN_SERVER_NAME="${GAME_NAME}-server"

cd "$(dirname "${BASH_SOURCE[0]}")/.."
exec /usr/bin/$BIN_SERVER_NAME "$@"