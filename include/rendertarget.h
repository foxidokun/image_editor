#pragma once

#include <SFML/Graphics.hpp>
#include "vector.h"
#include "point.h"

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
public:
    RenderTarget(const Vector& size) {
            _data.create(size.x, size.y);
            clear(sf::Color(255,255,255,255));
        }

    void clear(const sf::Color& color) { _data.clear(color); };

    void display(sf::RenderWindow& window);

    void drawRect(const Point& point, const Vector& size, const Color& fillcolor = {0,0,0,0},
                    const Color& bordercolor={0,0,0,255});

    void drawLine(const Point& point, const Vector& size, const Color& fillcolor = {0,0,0,255});

    void drawText(const Point& point, const char* text, uint size);

    void drawTexture(const Point& point, const Vector& size, const Texture& texture);
};