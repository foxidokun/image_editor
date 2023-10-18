#include "common.h"
#include <cassert>
#include <iostream>
#include "config.h"

static sf::Font load_font();
static sf::Texture load_texture(const char* path);

namespace global_resources {
    const sf::Font    font  = load_font();

    const sf::Texture close_button = load_texture(CLOSE_WINDOW_BUTTON_TEXTURE_PATH);
    const sf::Texture brush        = load_texture(BRUSH_TOOL_TEXTURE_PATH);
    const sf::Texture alien        = load_texture(ALIEN_BRUSH_TOOL_TEXTURE_PATH);
    const sf::Texture polyline     = load_texture(POLYLINE_TOOL_TEXTURE_PATH);
    const sf::Texture rectangle    = load_texture(RECTANGLE_TOOL_TEXTURE_PATH);
    const sf::Texture ellipse      = load_texture(ELLIPSE_TOOL_TEXTURE_PATH);
    const sf::Texture pick         = load_texture(PICK_TOOL_TEXTURE_PATH);
    const sf::Texture fill         = load_texture(FILL_TOOL_TEXTURE_PATH);
    const sf::Texture palette      = load_texture(PALETTE_TEXTURE_PATH);
}

sf::Font load_font() {
    sf::Font font;
    bool res = font.loadFromFile(FONT_FILENAME);
    assert(res);

    return font;
}

static sf::Texture load_texture(const char* path) {
    sf::Texture texture;
    bool res = texture.loadFromFile(path);
    assert(res);

    return texture;
}