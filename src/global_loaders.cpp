#include "common.h"
#include <cassert>
#include <iostream>

static sf::Font load_font();

sf::Font global_font = load_font();

sf::Font load_font() {
    sf::Font font;
    bool res = font.loadFromFile(FONT_FILENAME);
    assert(res);

    return font;
}
