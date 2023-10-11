#include "common.h"
#include <cassert>
#include <iostream>
#include "config.h"

static sf::Font load_font();
static sf::Texture load_texture(const char* path);

const sf::Font global_font = load_font();
const sf::Texture close_window_button = load_texture(CLOSE_WINDOW_BUTTON_TEXTURE_PATH);
const sf::Texture brush_tool          = load_texture(BRUSH_TOOL_TEXTURE_PATH);
const sf::Texture alien_brush_tool    = load_texture(ALIEN_BRUSH_TOOL_TEXTURE_PATH);

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