#!/usr/bin/env bash
GAME_NAME="need_for_speed"
BIN_CLIENT_NAME="${GAME_NAME}-client"

cd "$(dirname "${BASH_SOURCE[0]}")/.."
exec /usr/bin/$BIN_CLIENT_NAME "$@"
