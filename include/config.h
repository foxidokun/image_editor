#pragma once

typedef unsigned int uint;

#define ALWAYS_SAME_RANDOM 0

const double NEAR_ZERO_VEC_1DLEN = 1e-5;

const char WINDOW_TITLE[] = "foxidokun's WM";
const uint WINDOW_WIDTH   = 1080;
const uint WINDOW_HEIGHT  = 720;

const uint LINE_THICKNESS = 2;
const uint HEADER_HEIGHT  = 25;
const uint TITLE_SIZE     = 16;
const uint BUTTON_SIZE    = 20;

const uint BRUSH_RADIUS   = 5;

const double MENU_ITEM_HEIGHT = 30;

const double MENU_HEIGHT = 30;

const char FONT_FILENAME[]                    = "assets/font.ttf";
const char CLOSE_WINDOW_BUTTON_TEXTURE_PATH[] = "assets/close_window_button.png";
const char BRUSH_TOOL_TEXTURE_PATH[]          = "assets/brush.svg.png";
const char ALIEN_BRUSH_TOOL_TEXTURE_PATH[]    = "assets/alien.png";
const char POLYLINE_TOOL_TEXTURE_PATH[]       = "assets/polyline.svg.png";
const char RECTANGLE_TOOL_TEXTURE_PATH[]      = "assets/rectangle.svg.png";
const char RENDER_SHADER_PATH[]               = "assets/inverter.shader";
const char ELLIPSE_TOOL_TEXTURE_PATH[]        = "assets/ellipse.svg.png";
const char PICK_TOOL_TEXTURE_PATH[]           = "assets/colorpick.svg.png";
const char FILL_TOOL_TEXTURE_PATH[]           = "assets/fill.svg.png";
const char PALETTE_TEXTURE_PATH[]             = "assets/palette.svg.png";
const char SPLINE_TOOL_TEXTURE_PATH[]         = "assets/spline.svg.png";