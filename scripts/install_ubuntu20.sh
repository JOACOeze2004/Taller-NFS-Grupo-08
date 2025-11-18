#!/usr/bin/env bash
set -euo pipefail


SUDO=""
if [ "$(id -u)" -ne 0 ]; then
  SUDO="sudo"
fi

log() { echo -e "\e[1;32m[install]\e[0m $*"; }
warn() { echo -e "\e[1;33m[warn]\e[0m $*"; }

log "Updating APT indexes"
$SUDO apt-get update -y

log "Installing base tools"
$SUDO DEBIAN_FRONTEND=noninteractive apt-get install -y \
  ca-certificates gnupg software-properties-common lsb-release curl \
  build-essential git pkg-config ninja-build unzip zip

if ! cmake --version >/dev/null 2>&1 || ! cmake --version | awk 'NR==1{exit !($3 >= 3.24)}'; then
  log "Installing modern CMake from Kitware APT repo"
  $SUDO rm -f /usr/share/keyrings/kitware-archive-keyring.gpg || true
  $SUDO apt-get purge --auto-remove -y cmake || true

  curl -fsSL https://apt.kitware.com/keys/kitware-archive-latest.asc | \
    gpg --dearmor | $SUDO tee /usr/share/keyrings/kitware-archive-keyring.gpg >/dev/null

  echo "deb [signed-by=/usr/share/keyrings/kitware-archive-keyring.gpg] https://apt.kitware.com/ubuntu/ $(lsb_release -cs) main" | \
    $SUDO tee /etc/apt/sources.list.d/kitware.list >/dev/null

  $SUDO apt-get update -y
  $SUDO DEBIAN_FRONTEND=noninteractive apt-get install -y cmake
else
  log "CMake is recent enough"
fi

log "Installing Qt5 (Widgets), YAML-CPP, and SDL-related libs"

$SUDO DEBIAN_FRONTEND=noninteractive apt-get install -y \
  qt5-qmake qtbase5-dev qtchooser qtbase5-dev-tools \
  libyaml-cpp-dev \
  libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev libsdl2-mixer-dev

log "Installing codec libraries used by SDL_image/SDL_mixer/SDL_ttf"
$SUDO DEBIAN_FRONTEND=noninteractive apt-get install -y \
  libpng-dev libjpeg-turbo8-dev libtiff5-dev libwebp-dev \
  libfreetype6-dev \
  libogg-dev libvorbis-dev libmpg123-dev \
  libxmp-dev libopus-dev libopusfile-dev \
  libfluidsynth-dev fluidsynth \
  libwavpack1 libwavpack-dev

$SUDO DEBIAN_FRONTEND=noninteractive apt-get install -y \
  libx11-xcb1 libxkbcommon-x11-0 libxcb-icccm4 libxcb-image0 libxcb-keysyms1 \
  libxcb-randr0 libxcb-render-util0 libxcb-xinerama0 libxcb-xinput0 libxcb-xfixes0 \
  libxcb-shape0 || true

log "All dependencies installed. You can now build and run the project."
log "Next steps:"
cat <<'EOS'
  # Build and run client
  scripts/run_client.sh

  # Build and run server
  scripts/run_server.sh
EOS