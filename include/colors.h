#pragma once

#include <SFML/Graphics.hpp>
typedef unsigned int uint;

struct Color {
    uint8_t r = 0;
    uint8_t g = 0;
    uint8_t b = 0;
    uint8_t a = 0;

    Color(const sf::Color &sfc): r(sfc.r), g(sfc.g), b(sfc.b), a(sfc.a) {}
    Color(uint r, uint g, uint b, uint a=255): r(r), g(g), b(b), a(a) {}
    Color() = default;

    operator sf::Color() const {
        return sf::Color(r, g, b, a);
    }

    bool operator==(const Color& other) const {
        return ((sf::Color)(*this) == (sf::Color)(other));
    } 
};

#define BREEZE_DARK 1

#define COLOR_SCHEME BREEZE_DARK


#if COLOR_SCHEME == BREEZE_DARK
const Color WINDOW_BACKGROUND_COLOR(49,54,59);
const Color WM_BACKGROUND_COLOR(35,38,41);
const Color TEXT_COLOR(255,255,255);
const Color BUTTON_INACTIVE_COLOR(49,54,59);
const Color BUTTON_ACTIVE_COLOR(61,174,233);
const Color TEXTBOX_BACKGROUND_COLOR(77,77,77);

#else
#error "Unknown color scheme"
#endif