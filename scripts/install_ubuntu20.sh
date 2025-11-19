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
RED='\033[0;31m'
NC='\033[0m'

log()  { echo -e "${GREEN}[install]${NC} $*"; }
info() { echo -e "${BLUE}[info]${NC}    $*"; }
warn() { echo -e "${YELLOW}[warn]${NC}    $*"; }
error() { echo -e "${RED}[error]${NC}   $*"; }

log "Starting installer for '${GAME_NAME}'"
info "Root dir: $ROOT_DIR"

# Clean up problematic Kitware repo if it exists
if [[ -f /etc/apt/sources.list.d/kitware.list ]]; then
  warn "Found problematic Kitware repository, removing it..."
  sudo rm -f /etc/apt/sources.list.d/kitware.list
  sudo rm -f /usr/share/keyrings/kitware-archive-keyring.gpg
fi

log "Updating APT indexes"
sudo apt-get update -y

log "Installing base tools"
sudo DEBIAN_FRONTEND=noninteractive apt-get install -y \
  ca-certificates gnupg software-properties-common lsb-release curl wget \
  build-essential git pkg-config ninja-build unzip zip libssl-dev

# Function to check if CMake version is sufficient
check_cmake_version() {
  if ! command -v cmake >/dev/null 2>&1; then
    return 1
  fi
  
  local version=$(cmake --version | head -n1 | grep -oP '\d+\.\d+' | head -n1)
  local major=$(echo "$version" | cut -d. -f1)
  local minor=$(echo "$version" | cut -d. -f2)
  
  if [[ $major -gt 3 ]] || [[ $major -eq 3 && $minor -ge 24 ]]; then
    echo "$version"
    return 0
  fi
  return 1
}

# Check current CMake
CMAKE_OK=false
if CMAKE_VERSION=$(check_cmake_version); then
  CMAKE_OK=true
  log "CMake version $CMAKE_VERSION is sufficient (>= 3.24)"
else
  if command -v cmake >/dev/null 2>&1; then
    CURRENT_VERSION=$(cmake --version | head -n1 | grep -oP '\d+\.\d+' | head -n1)
    warn "CMake version $CURRENT_VERSION is too old (need >= 3.24)"
  else
    warn "CMake not found"
  fi
fi

# Install CMake if needed
if [[ "$CMAKE_OK" == "false" ]]; then
  log "Installing modern CMake automatically from official source"
  
  # Determine architecture
  ARCH=$(uname -m)
  if [[ "$ARCH" == "x86_64" ]]; then
    CMAKE_ARCH="x86_64"
  elif [[ "$ARCH" == "aarch64" ]]; then
    CMAKE_ARCH="aarch64"
  else
    error "Unsupported architecture: $ARCH"
    exit 1
  fi
  
  # CMake version to install
  CMAKE_VERSION_TO_INSTALL="3.28.3"
  CMAKE_URL="https://github.com/Kitware/CMake/releases/download/v${CMAKE_VERSION_TO_INSTALL}/cmake-${CMAKE_VERSION_TO_INSTALL}-linux-${CMAKE_ARCH}.tar.gz"
  CMAKE_TAR="cmake-${CMAKE_VERSION_TO_INSTALL}-linux-${CMAKE_ARCH}.tar.gz"
  CMAKE_DIR="cmake-${CMAKE_VERSION_TO_INSTALL}-linux-${CMAKE_ARCH}"
  
  log "Downloading CMake ${CMAKE_VERSION_TO_INSTALL} for ${CMAKE_ARCH}"
  cd /tmp
  
  # Remove old CMake installation if exists
  sudo rm -rf /opt/cmake
  
  # Download CMake
  if [[ -f "$CMAKE_TAR" ]]; then
    rm -f "$CMAKE_TAR"
  fi
  
  wget --no-check-certificate -q --show-progress "$CMAKE_URL" || {
    error "Failed to download CMake from $CMAKE_URL"
    exit 1
  }
  
  log "Extracting CMake"
  tar -xzf "$CMAKE_TAR"
  
  log "Installing CMake to /opt/cmake"
  sudo mv "$CMAKE_DIR" /opt/cmake
  
  # Create symlinks
  sudo ln -sf /opt/cmake/bin/cmake /usr/local/bin/cmake
  sudo ln -sf /opt/cmake/bin/ctest /usr/local/bin/ctest
  sudo ln -sf /opt/cmake/bin/cpack /usr/local/bin/cpack
  
  # Clean up
  rm -f "$CMAKE_TAR"
  
  # Update PATH for current session
  export PATH="/usr/local/bin:$PATH"
  
  # Verify installation
  if CMAKE_VERSION=$(check_cmake_version); then
    log "Successfully installed CMake $CMAKE_VERSION"
  else
    error "CMake installation failed"
    exit 1
  fi
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

# Clean build if CMake was just installed
if [[ "$CMAKE_OK" == "false" ]]; then
  log "Cleaning previous build configuration due to CMake update"
  rm -rf "$BUILD_DIR"/*
fi

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
  warn "Assets directory not found at $ROOT_DIR/assets"
fi

log "Copying complete src/ structure to $VAR_DIR"
if [[ -d "$ROOT_DIR/src" ]]; then
  sudo cp -r "$ROOT_DIR/src" "$VAR_DIR/" || true
  info "Copied src/ directory with all YAMLs"
else
  warn "Source directory not found at $ROOT_DIR/src"
fi

log "Creating optimized directory structure for asset loading"

# Create main game directory structure
sudo mkdir -p "$VAR_DIR/game"

# Copy or move everything to the game directory
if [[ -d "$VAR_DIR/assets" ]]; then
  sudo cp -r "$VAR_DIR/assets" "$VAR_DIR/game/" || true
fi

if [[ -d "$VAR_DIR/src" ]]; then
  sudo cp -r "$VAR_DIR/src" "$VAR_DIR/game/" || true
fi

log "Creating symlink structure for multiple path resolution strategies"

# Strategy 1: Symlinks in /usr for ../assets and ../src paths
sudo rm -f /usr/assets /usr/src 2>/dev/null || true
sudo ln -sf "$VAR_DIR/game/assets" /usr/assets
sudo ln -sf "$VAR_DIR/game/src" /usr/src

if [[ -L /usr/assets ]]; then
  info "Created: /usr/assets -> $VAR_DIR/game/assets"
else
  warn "Failed to create symlink /usr/assets"
fi

if [[ -L /usr/src ]]; then
  info "Created: /usr/src -> $VAR_DIR/game/src"
else
  warn "Failed to create symlink /usr/src"
fi

# Strategy 2: Symlinks in /var/need_for_speed for working directory approach
sudo ln -sf "$VAR_DIR/game/assets" "$VAR_DIR/assets" 2>/dev/null || true
sudo ln -sf "$VAR_DIR/game/src" "$VAR_DIR/src" 2>/dev/null || true

# Strategy 3: Create assets and src in the parent directory of /usr/bin (root)
sudo ln -sf "$VAR_DIR/game/assets" /assets 2>/dev/null || true
sudo ln -sf "$VAR_DIR/game/src" /src 2>/dev/null || true

log "Creating enhanced launcher scripts on Desktop"
mkdir -p "$DESKTOP_DIR"

# Client launcher with multiple path strategies
cat > "$DESKTOP_DIR/run_client.sh" <<EOFCLIENT
#!/usr/bin/env bash

# Set working directory to /var/need_for_speed/game where assets/src are located
cd "$VAR_DIR/game" || cd /var/need_for_speed/game || {
  echo "Error: Cannot change to game directory"
  exit 1
}

# Export config file location
export NEED_FOR_SPEED_CLIENT_CONFIG_FILE="$CFG_DIR/client_config.yaml"

# Export asset paths for the game to use
export NEED_FOR_SPEED_ASSETS_DIR="$VAR_DIR/game/assets"
export NEED_FOR_SPEED_SRC_DIR="$VAR_DIR/game/src"

# Default connection values
HOST="\${1:-localhost}"
PORT="\${2:-8080}"

echo "================================================"
echo "  Need For Speed - Client Launcher"
echo "================================================"
echo "Working directory: \$(pwd)"
echo "Assets directory: \$NEED_FOR_SPEED_ASSETS_DIR"
echo "Source directory: \$NEED_FOR_SPEED_SRC_DIR"
echo "Config file: \$NEED_FOR_SPEED_CLIENT_CONFIG_FILE"
echo "Connecting to: \$HOST:\$PORT"
echo "================================================"
echo

# Launch client from the game directory (so relative paths work)
exec "$BIN_DIR/$BIN_CLIENT_NAME" "\$HOST" "\$PORT"
EOFCLIENT

chmod +x "$DESKTOP_DIR/run_client.sh"
log "Client launcher created -> $DESKTOP_DIR/run_client.sh"

# Server launcher with multiple path strategies
cat > "$DESKTOP_DIR/run_server.sh" <<EOFSERVER
#!/usr/bin/env bash

# Set working directory to /var/need_for_speed/game where assets/src are located
cd "$VAR_DIR/game" || cd /var/need_for_speed/game || {
  echo "Error: Cannot change to game directory"
  exit 1
}

# Export config file location
export NEED_FOR_SPEED_SERVER_CONFIG_FILE="$CFG_DIR/server_config.yaml"

# Export asset paths for the game to use
export NEED_FOR_SPEED_ASSETS_DIR="$VAR_DIR/game/assets"
export NEED_FOR_SPEED_SRC_DIR="$VAR_DIR/game/src"

# Default port
PORT="\${1:-8080}"

echo "================================================"
echo "  Need For Speed - Server Launcher"
echo "================================================"
echo "Working directory: \$(pwd)"
echo "Assets directory: \$NEED_FOR_SPEED_ASSETS_DIR"
echo "Source directory: \$NEED_FOR_SPEED_SRC_DIR"
echo "Config file: \$NEED_FOR_SPEED_SERVER_CONFIG_FILE"
echo "Starting server on port: \$PORT"
echo "================================================"
echo

# Launch server from the game directory (so relative paths work)
exec "$BIN_DIR/$BIN_SERVER_NAME" "\$PORT"
EOFSERVER

chmod +x "$DESKTOP_DIR/run_server.sh"
log "Server launcher created -> $DESKTOP_DIR/run_server.sh"

# Set proper permissions
sudo chown -R "$(id -u):$(id -g)" "$VAR_DIR" || true
sudo chmod -R 755 "$VAR_DIR/game" || true

log "Installation finished successfully!"
echo
info "=========================================="
info "         INSTALLATION SUMMARY"
info "=========================================="
echo
echo "CMake Version: $(cmake --version | head -n1)"
echo
echo "Executables:"
echo "  • Client: ${BIN_DIR}/${BIN_CLIENT_NAME}"
echo "  • Server: ${BIN_DIR}/${BIN_SERVER_NAME}"
echo
echo "Configuration:"
echo "  • Config directory: ${CFG_DIR}"
echo "  • Game data: ${VAR_DIR}/game"
echo "  • Assets: ${VAR_DIR}/game/assets"
echo "  • Source YAMLs: ${VAR_DIR}/game/src"
echo
echo "Symlinks for path resolution:"
echo "  • /usr/assets -> ${VAR_DIR}/game/assets"
echo "  • /usr/src -> ${VAR_DIR}/game/src"
echo "  • /assets -> ${VAR_DIR}/game/assets"
echo "  • /src -> ${VAR_DIR}/game/src"
echo
echo "Desktop Launchers:"
echo "  • ${DESKTOP_DIR}/run_client.sh"
echo "  • ${DESKTOP_DIR}/run_server.sh"
echo
info "=========================================="
info "         HOW TO RUN THE GAME"
info "=========================================="
echo
echo "1. START THE SERVER:"
echo "   ${DESKTOP_DIR}/run_server.sh [PORT]"
echo "   Example: ${DESKTOP_DIR}/run_server.sh 8080"
echo "   (Default port: 8080)"
echo
echo "2. START THE CLIENT:"
echo "   ${DESKTOP_DIR}/run_client.sh [HOST] [PORT]"
echo "   Example: ${DESKTOP_DIR}/run_client.sh localhost 8080"
echo "   (Defaults: localhost:8080)"
echo
echo "3. Or simply double-click the .sh files on your Desktop!"
echo
info "=========================================="
echo
info "The launchers now:"
echo "  ✓ Change to the correct working directory"
echo "  ✓ Set environment variables for asset paths"
echo "  ✓ Display diagnostic information"
echo "  ✓ Should resolve all asset/YAML loading issues"
echo
