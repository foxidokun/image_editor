#pragma once

#include <SFML/Graphics.hpp>
#include "plugin.h"
typedef unsigned int uint;

using plugin::Color;

#define BREEZE_DARK 1

#define COLOR_SCHEME BREEZE_DARK


#if COLOR_SCHEME == BREEZE_DARK
const Color WINDOW_BACKGROUND_COLOR(49,54,59);
const Color WM_BACKGROUND_COLOR(35,38,41);
const Color TEXT_COLOR(255,255,255);
const Color BUTTON_INACTIVE_COLOR(49,54,59);
const Color BUTTON_ACTIVE_COLOR(61,174,233);
const Color TEXTBOX_BACKGROUND_COLOR(77,77,77);
const Color SCROLLBAR_RULER_COLOR(77,77,77);
const Color SCROLLBAR_BACKGROUND_COLOR(35,38,41);

#else
#error "Unknown color scheme"
#endif