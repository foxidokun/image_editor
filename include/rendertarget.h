#pragma once

#include <SFML/Graphics.hpp>
#include "vector.h"
#include "point.h"
#include "regions.h"

typedef sf::Texture Texture;

struct Color {
    uint r;
    uint g;
    uint b;
    uint a;
};

class RenderTarget {
private:
    sf::RenderTexture _data;
    sf::Shader invert_shader;

public:
    RenderTarget(const Vector& size) {
            _data.create(size.x, size.y);
            clear(sf::Color(255,255,255,255));
            invert_shader.loadFromFile(RENDER_SHADER_PATH, sf::Shader::Fragment);
            invert_shader.setUniform("texture", sf::Shader::CurrentTexture);
        }

    void clear(const sf::Color& color) { _data.clear(color); };

    void display(sf::RenderWindow& window);

    void drawRect(const Region& reg, const Point& point, const Vector& size, const Color& fillcolor = {0,0,0,0});

    void drawLine(const Region& reg, const Point& point, const Vector& size, const Color& fillcolor = {0,0,0,255});

    void drawText(const Region& reg, const Point& point, const char* text, uint size);

    void drawTexture(const Region& reg, const Point& point, const Vector& size, const Texture& texture,
        bool invert = false);
};

static inline sf::Color convert_color(const Color& color) {
    return sf::Color(color.r, color.g, color.b, color.a);
}