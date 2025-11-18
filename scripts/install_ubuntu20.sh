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
info() { echo -e "${BLUE}[info]${NC}    $*"; }
warn() { echo -e "${YELLOW}[warn]${NC}    $*"; }

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
  info "No config/ folder found. Creating placeholder configs"
  echo "# Placeholder client config" | sudo tee "$CFG_DIR/client_config.yaml" >/dev/null
  echo "# Placeholder server config" | sudo tee "$CFG_DIR/server_config.yaml" >/dev/null
fi

log "Copying assets to $VAR_DIR"
if [[ -d "$ROOT_DIR/assets" ]]; then
  sudo cp -r "$ROOT_DIR/assets" "$VAR_DIR/" || true
  info "Copied assets/ directory"
else
  for subdir in fonts images "need-for-speed"; do
    if [[ -d "$ROOT_DIR/assets/$subdir" ]]; then
      sudo mkdir -p "$VAR_DIR/assets/$subdir"
      sudo cp -r "$ROOT_DIR/assets/$subdir/"* "$VAR_DIR/assets/$subdir/" || true
      info "Copied assets/$subdir"
    fi
  done
fi

log "Copying complete src/ structure to $VAR_DIR"
if [[ -d "$ROOT_DIR/src" ]]; then
  sudo cp -r "$ROOT_DIR/src" "$VAR_DIR/" || true
  info "Copied src/ directory with all YAMLs"
fi

log "Creating symlink structure for hardcoded paths"

sudo ln -sf "$VAR_DIR/assets" /usr/assets
if [[ -L /usr/assets ]]; then
  info "Symlink created: /usr/assets -> $VAR_DIR/assets"
else
  warn "Failed to create symlink /usr/assets"
fi

sudo mkdir -p /usr/src
sudo ln -sf "$VAR_DIR/src/server" /usr/src/server
if [[ -L /usr/src/server ]]; then
  info "Symlink created: /usr/src/server -> $VAR_DIR/src/server"
  info "This makes hardcoded paths like '../src/server/libertycity.yaml' work from /usr/bin"
else
  warn "Failed to create symlink /usr/src/server"
fi

sudo ln -sf "$VAR_DIR/src/client" /usr/src/client
if [[ -L /usr/src/client ]]; then
  info "Symlink created: /usr/src/client -> $VAR_DIR/src/client"
  info "This makes '../src/client/car_sprites.yaml' work from /usr/bin"
else
  warn "Failed to create symlink /usr/src/client"
fi

log "Creating launcher scripts on Desktop"
mkdir -p "$DESKTOP_DIR"

cat > "$DESKTOP_DIR/run_client.sh" <<'EOFCLIENT'
#!/usr/bin/env bash
cd /usr/bin

# Export config file location
export NEED_FOR_SPEED_CLIENT_CONFIG_FILE="/etc/need_for_speed/client_config.yaml"

# Default values
HOST="${1:-localhost}"
PORT="${2:-8080}"

# Launch client
exec ./need_for_speed-client "$HOST" "$PORT"
EOFCLIENT

chmod +x "$DESKTOP_DIR/run_client.sh"
log "Client launcher created -> $DESKTOP_DIR/run_client.sh"

cat > "$DESKTOP_DIR/run_server.sh" <<'EOFSERVER'
#!/usr/bin/env bash
cd /usr/bin

# Export config file location
export NEED_FOR_SPEED_SERVER_CONFIG_FILE="/etc/need_for_speed/server_config.yaml"

# Default port
PORT="${1:-8080}"

# Launch server
exec ./need_for_speed-server "$PORT"
EOFSERVER

chmod +x "$DESKTOP_DIR/run_server.sh"
log "Server launcher created -> $DESKTOP_DIR/run_server.sh"

log "Installation finished successfully!"
echo
info "Summary:"
echo " - Client executable: ${BIN_DIR}/${BIN_CLIENT_NAME}"
echo " - Server executable: ${BIN_DIR}/${BIN_SERVER_NAME}"
echo " - Config directory: ${CFG_DIR}"
echo " - Data directory: ${VAR_DIR}"
echo " - Assets: ${VAR_DIR}/assets"
echo " - Source YAMLs: ${VAR_DIR}/src/server"
echo " - Symlinks for hardcoded paths:"
echo "   * /usr/assets -> ${VAR_DIR}/assets"
echo "   * /usr/src/server -> ${VAR_DIR}/src/server"
echo "   * /usr/src/client -> ${VAR_DIR}/src/client"
echo " - Desktop launchers:"
echo "   * ${DESKTOP_DIR}/run_client.sh"
echo "   * ${DESKTOP_DIR}/run_server.sh"
echo
info "To run the game:"
echo " 1. Server: ${DESKTOP_DIR}/run_server.sh [PORT]"
echo "    Example: ${DESKTOP_DIR}/run_server.sh 8080"
echo "    (defaults to port 8080 if not specified)"
echo
echo " 2. Client: ${DESKTOP_DIR}/run_client.sh [HOST] [PORT]"
echo "    Example: ${DESKTOP_DIR}/run_client.sh localhost 8080"
echo "    (defaults to localhost:8080 if not specified)"
echo
echo " 3. You can also just double-click the .sh files for default values"