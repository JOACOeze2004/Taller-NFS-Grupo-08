#!/usr/bin/env bash
GAME_NAME="need_for_speed"
BIN_TEST_NAME="${GAME_NAME}-tests"

cd "$(dirname "${BASH_SOURCE[0]}")/.."

exec /usr/bin/$BIN_TEST_NAME