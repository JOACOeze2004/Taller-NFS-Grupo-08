#!/usr/bin/env bash
set -euo pipefail

GAME_NAME="need_for_speed"
BIN_CLIENT_NAME="${GAME_NAME}-client"
BIN_SERVER_NAME="${GAME_NAME}-server"

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")"/.. && pwd)"
BUILD_DIR="$ROOT_DIR/build"
CFG_DIR="/etc/$GAME_NAME"
VAR_DIR="/var/$GAME_NAME"
BIN_DIR="/usr/bin"
DESKTOP_DIR="${HOME}/Desktop"

GREEN='\033[0;32m'
BLUE='\033[94m'
YELLOW='\033[1;33m'
NC='\033[0m'

log()  { echo -e "${GREEN}[install]${NC} $*"; }
info() { echo -e "${BLUE}[info]${NC}  $*"; }
warn() { echo -e "${YELLOW}[warn]${NC} $*"; }

log "Starting installer for '${GAME_NAME}'"
info "Root dir: $ROOT_DIR"

log "Updating APT indexes"
sudo apt-get update -y

log "Installing base tools"
sudo DEBIAN_FRONTEND=noninteractive apt-get install -y \
  ca-certificates gnupg software-properties-common lsb-release curl \
  build-essential git pkg-config ninja-build unzip zip

if ! command -v cmake >/dev/null 2>&1 || ! cmake --version | awk 'NR==1{exit !($3 >= 3.24)}'; then
  log "Installing modern CMake from Kitware APT repo"
  sudo rm -f /usr/share/keyrings/kitware-archive-keyring.gpg || true
  sudo apt-get purge --auto-remove -y cmake || true

  curl -fsSL https://apt.kitware.com/keys/kitware-archive-latest.asc | \
    gpg --dearmor | sudo tee /usr/share/keyrings/kitware-archive-keyring.gpg >/dev/null

  echo "deb [signed-by=/usr/share/keyrings/kitware-archive-keyring.gpg] https://apt.kitware.com/ubuntu/ $(lsb_release -cs) main" | \
    sudo tee /etc/apt/sources.list.d/kitware.list >/dev/null

  sudo apt-get update -y
  sudo DEBIAN_FRONTEND=noninteractive apt-get install -y cmake
else
  log "CMake is recent enough"
fi

log "Installing dependencies: Qt5, YAML-CPP, SDL2 and codec libs"
sudo DEBIAN_FRONTEND=noninteractive apt-get install -y \
  qt5-qmake qtbase5-dev qtchooser qtbase5-dev-tools \
  libyaml-cpp-dev \
  libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev libsdl2-mixer-dev \
  libpng-dev libjpeg-turbo8-dev libtiff5-dev libwebp-dev \
  libfreetype6-dev libogg-dev libvorbis-dev libmpg123-dev \
  libxmp-dev libopus-dev libopusfile-dev libfluidsynth-dev fluidsynth \
  libwavpack1 libwavpack-dev \
  libx11-xcb1 libxkbcommon-x11-0 libxcb-icccm4 libxcb-image0 libxcb-keysyms1 \
  libxcb-randr0 libxcb-render-util0 libxcb-xinerama0 libxcb-xinput0 libxcb-xfixes0 \
  libxcb-shape0 || true

log "Configuring and building project (Release)"
mkdir -p "$BUILD_DIR"
if [[ ! -f "$BUILD_DIR/Makefile" && ! -f "$BUILD_DIR/build.ninja" ]]; then
  cmake -S "$ROOT_DIR" -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE=Release
fi

cmake --build "$BUILD_DIR" -j"$(nproc)"

log "Installing executables into ${BIN_DIR}"
SRC_CLIENT_BIN="$BUILD_DIR/taller_client"
SRC_SERVER_BIN="$BUILD_DIR/taller_server"

if [[ ! -x "$SRC_CLIENT_BIN" ]]; then
  warn "Client binary not found or not executable at: $SRC_CLIENT_BIN"
fi

if [[ ! -x "$SRC_SERVER_BIN" ]]; then
  warn "Server binary not found or not executable at: $SRC_SERVER_BIN"
fi

if [[ -f "$SRC_CLIENT_BIN" ]]; then
  sudo install -m 755 "$SRC_CLIENT_BIN" "$BIN_DIR/$BIN_CLIENT_NAME"
  log "Installed client -> $BIN_DIR/$BIN_CLIENT_NAME"
fi

if [[ -f "$SRC_SERVER_BIN" ]]; then
  sudo install -m 755 "$SRC_SERVER_BIN" "$BIN_DIR/$BIN_SERVER_NAME"
  log "Installed server -> $BIN_DIR/$BIN_SERVER_NAME"
fi

log "Preparing configuration and data directories"
sudo mkdir -p "$CFG_DIR" "$VAR_DIR"
sudo chown -R "$(id -u):$(id -g)" "$CFG_DIR" "$VAR_DIR" || true

if [[ -d "$ROOT_DIR/config" ]]; then
  info "Copying config files from repo/config -> $CFG_DIR"
  sudo cp -r "$ROOT_DIR/config/"* "$CFG_DIR/" || true
else
  info "No config/ folder found. Creating placeholder"
  echo "# Placeholder client config" > "$CFG_DIR/client_config.yaml"
  echo "# Placeholder server config" > "$CFG_DIR/server_config.yaml"
fi

if [[ -d "$ROOT_DIR/assets" ]]; then
  info "Copying assets from repo/assets -> $VAR_DIR"
  sudo cp -r "$ROOT_DIR/assets/"* "$VAR_DIR/" || true
else
  info "No assets/ folder found. Creating placeholder"
  echo "placeholder asset" > "$VAR_DIR/README.txt"
fi

log "Creating launchers on Desktop"
mkdir -p "$DESKTOP_DIR"

cat > "$DESKTOP_DIR/run_client.sh" <<EOF
#!/usr/bin/env bash
cd "$ROOT_DIR"
export NEED_FOR_SPEED_CLIENT_CONFIG_FILE="$CFG_DIR/client_config.yaml"
exec "$BIN_DIR/$BIN_CLIENT_NAME" "\$@"
EOF

chmod +x "$DESKTOP_DIR/run_client.sh"
log "Client launcher created -> $DESKTOP_DIR/run_client.sh"

cat > "$DESKTOP_DIR/run_server.sh" <<EOF
#!/usr/bin/env bash
cd "$ROOT_DIR"
export NEED_FOR_SPEED_SERVER_CONFIG_FILE="$CFG_DIR/server_config.yaml"
exec "$BIN_DIR/$BIN_SERVER_NAME" "\$@"
EOF

chmod +x "$DESKTOP_DIR/run_server.sh"
log "Server launcher created -> $DESKTOP_DIR/run_server.sh"

log "Installation finished successfully."
echo
info "Summary:"
echo " - Client executable: ${BIN_DIR}/${BIN_CLIENT_NAME}"
echo " - Server executable: ${BIN_DIR}/${BIN_SERVER_NAME}"
echo " - Config directory: ${CFG_DIR}"
echo " - Data directory: ${VAR_DIR}"
echo " - Desktop launchers: ${DESKTOP_DIR}/run_client.sh, ${DESKTOP_DIR}/run_server.sh"