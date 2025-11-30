#ifndef CLIENT_CONFIG_H
#define CLIENT_CONFIG_H

#include <SDL2/SDL.h>

// DISPLAY
constexpr int DEFAULT_SCREEN_WIDTH = 1200;
constexpr int DEFAULT_SCREEN_HEIGHT = 900;
constexpr float ZOOM_FACTOR = 2.0f;

// FONTS
constexpr const char* DEFAULT_FONT_PATH = "../assets/fonts/DejaVuSans.ttf";
constexpr int DEFAULT_FONT_SIZE = 24;
constexpr int UPGRADE_FONT_SIZE = 24;

// TEXTURE PATHS
// Gameplay
constexpr const char* CHECKPOINT_TEXTURE_PATH = "../assets/need-for-speed/sprits/checkpoints.png";
constexpr const char* HINT_TEXTURE_PATH = "../assets/need-for-speed/sprits/Hints.png";
constexpr const char* SPEEDOMETER_TEXTURE_PATH = "../assets/need-for-speed/sprits/Speedmeter.png";

// Workshop
constexpr const char* WORKSHOP_ICONS_PATH = "../assets/need-for-speed/sprits/Workshop_icons_1.png";
constexpr const char* WORKSHOP_ARROWS_PATH = "../assets/need-for-speed/sprits/workshop_arrow_spritesheet.png";

// Cars
constexpr const char* CAR_TEXTURE_PATH = "../assets/need-for-speed/cars/Cars_without_bg.png";
constexpr const char* NITRO_TEXTURE_PATH = "../assets/need-for-speed/sprits/nitro-fire.png";
constexpr const char* YAML_CONFIG_PATH = "../src/client/car_sprites.yaml";

// Maps
constexpr const char* LIBERTY_CITY_MAP_PATH = "../assets/need-for-speed/cities/Game Boy _ GBC - Grand Theft Auto - Backgrounds - Liberty City.png";
constexpr const char* SAN_ANDREAS_MAP_PATH = "../assets/need-for-speed/cities/Game Boy _ GBC - Grand Theft Auto - Backgrounds - San Andreas.png";
constexpr const char* VICE_CITY_MAP_PATH = "../assets/need-for-speed/cities/Game Boy _ GBC - Grand Theft Auto - Backgrounds - Vice City.png";

// MINIMAP
constexpr float MINIMAP_SCALE = 0.04f;
constexpr float RADAR_RADIUS = 400.0f;
constexpr int MINIMAP_MARGIN = 10;
constexpr float MINIMAP_CAR_SIZE = 6.0f;
constexpr float MINIMAP_CHECKPOINT_SIZE_INSIDE = 10.0f;
constexpr float MINIMAP_CHECKPOINT_SIZE_OUTSIDE = 8.0f;
constexpr float MINIMAP_HINT_ARROW_W_INSIDE = 16.0f;
constexpr float MINIMAP_HINT_ARROW_H_INSIDE = 24.0f;
constexpr float MINIMAP_HINT_ARROW_W_OUTSIDE = 12.0f;
constexpr float MINIMAP_HINT_ARROW_H_OUTSIDE = 18.0f;
constexpr float MINIMAP_EDGE_FACTOR = 0.45f;

// UI COLORS
// Medals
constexpr SDL_Color COLOR_GOLD = {255, 215, 0, 255};
constexpr SDL_Color COLOR_SILVER = {192, 192, 192, 255};
constexpr SDL_Color COLOR_BRONZE = {205, 127, 50, 255};

// General
constexpr SDL_Color COLOR_WHITE = {255, 255, 255, 255};
constexpr SDL_Color COLOR_BLACK = {0, 0, 0, 255};
constexpr SDL_Color COLOR_RED = {255, 0, 0, 255};
constexpr SDL_Color COLOR_GREEN = {0, 255, 0, 255};

// Minimap
constexpr SDL_Color COLOR_PLAYER_CAR = {0, 255, 0, 255};
constexpr SDL_Color COLOR_ENEMY_CAR = {255, 0, 0, 255};
constexpr SDL_Color COLOR_CHECKPOINT_GREEN = {0, 255, 0, 255};
constexpr SDL_Color COLOR_CHECKPOINT_YELLOW = {255, 255, 0, 255};
constexpr SDL_Color COLOR_CHECKPOINT_ORANGE = {255, 100, 0, 255};

// State Overlays
constexpr SDL_Color COLOR_OVERLAY_FINISHED = {0, 255, 0, 76};
constexpr SDL_Color COLOR_OVERLAY_DEAD = {255, 0, 0, 76};
constexpr SDL_Color COLOR_OVERLAY_TIMEOUT = {255, 0, 0, 76};

// Ready Button
constexpr SDL_Color COLOR_READY_SENT = {0, 180, 0, 200};
constexpr SDL_Color COLOR_READY_HOVER = {50, 164, 255, 230};
constexpr SDL_Color COLOR_READY_NORMAL = {30, 144, 255, 200};
constexpr SDL_Color COLOR_READY_BORDER = {255, 255, 255, 200};

// Results Panel
constexpr SDL_Color COLOR_PANEL_BG = {20, 20, 40, 230};
constexpr SDL_Color COLOR_PANEL_BORDER = {255, 215, 0, 255};
constexpr SDL_Color COLOR_HEADER_TEXT = {200, 200, 200, 255};

// Upgrade Phase
constexpr SDL_Color COLOR_UPGRADE_BG = {20, 20, 40, 80};
constexpr SDL_Color COLOR_UPGRADE_BUTTON_BG = {60, 90, 150, 220};
constexpr SDL_Color COLOR_UPGRADE_BUTTON_BORDER = {140, 170, 210, 255};
constexpr SDL_Color COLOR_UPGRADE_TITLE = {255, 215, 0, 255};
constexpr SDL_Color COLOR_UPGRADE_INFO = {200, 200, 200, 255};
constexpr SDL_Color COLOR_UPGRADE_DESC = {200, 200, 200, 255};

// UI DIMENSIONS
// Ready Button
constexpr int READY_BUTTON_WIDTH = 250;
constexpr int READY_BUTTON_HEIGHT = 50;
constexpr int READY_BUTTON_MARGIN_BOTTOM = 120;

// Results Panel
constexpr int RESULTS_PANEL_WIDTH = 800;
constexpr int RESULTS_PANEL_HEIGHT = 600;
constexpr int RESULTS_TITLE_Y_OFFSET = 30;
constexpr int RESULTS_HEADER_Y_OFFSET = 80;
constexpr int RESULTS_FIRST_ROW_Y = 120;
constexpr int RESULTS_ROW_HEIGHT = 40;
constexpr size_t RESULTS_MAX_ROWS = 15;
constexpr int RESULTS_COL_POS_X = 30;
constexpr int RESULTS_COL_NAME_X = 120;
constexpr int RESULTS_COL_TIME_X_OFFSET = 180;

// Upgrade Phase
constexpr int UPGRADE_BUTTON_WIDTH = 400;
constexpr int UPGRADE_BUTTON_HEIGHT = 180;
constexpr int UPGRADE_BUTTON_SPACING = 25;
constexpr int UPGRADE_COLUMN_SPACING = 70;
constexpr int UPGRADE_START_Y = 100;
constexpr int UPGRADE_ICON_SIZE = 95;
constexpr int UPGRADE_ARROW_SIZE = 45;
constexpr int UPGRADE_ARROW_GAP = 50;
constexpr int UPGRADE_ICON_PADDING = 12;
constexpr int UPGRADE_TITLE_Y_OFFSET = 10;
constexpr int UPGRADE_DESC_Y_OFFSET = 60;
constexpr int UPGRADE_TEXT_LEFT_MARGIN = 20;

// Speedometer
constexpr int SPEEDOMETER_SRC_X = 40;
constexpr int SPEEDOMETER_SRC_Y = 70;
constexpr int SPEEDOMETER_SRC_W = 530;
constexpr int SPEEDOMETER_SRC_H = 240;
constexpr int SPEEDOMETER_DST_X = 75;
constexpr int SPEEDOMETER_DST_Y_OFFSET = 175;
constexpr int SPEEDOMETER_DST_W = 265;
constexpr int SPEEDOMETER_DST_H = 140;
constexpr int SPEEDOMETER_NEEDLE_SRC_X = 270;
constexpr int SPEEDOMETER_NEEDLE_SRC_Y = 235;
constexpr int SPEEDOMETER_NEEDLE_SRC_W = 61;
constexpr int SPEEDOMETER_NEEDLE_SRC_H = 200;
constexpr float SPEEDOMETER_NEEDLE_CENTER_X = 30.5f;
constexpr float SPEEDOMETER_NEEDLE_CENTER_Y = 180.0f;
constexpr int SPEEDOMETER_NEEDLE_DST_X = 160;
constexpr int SPEEDOMETER_NEEDLE_DST_Y_OFFSET = 225;
constexpr float SPEEDOMETER_NEEDLE_DST_W = 60.0f;
constexpr float SPEEDOMETER_NEEDLE_DST_H = 200.0f;
constexpr float SPEEDOMETER_MAX_SPEED = 400.0f;
constexpr float SPEEDOMETER_ANGLE_RANGE = 180.0f;
constexpr float SPEEDOMETER_ANGLE_OFFSET = -90.0f;

// Checkpoint
constexpr float CHECKPOINT_DST_W = 60.0f;
constexpr float CHECKPOINT_DST_H = 90.0f;

// Hint Arrow
constexpr float HINT_DST_W = 40.0f;
constexpr float HINT_DST_H = 60.0f;

#endif // CLIENT_CONFIG_H
