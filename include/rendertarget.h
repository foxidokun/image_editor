#pragma once

#include <SFML/Graphics.hpp>
#include "vector.h"
#include "point.h"

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
        }

    void display(sf::RenderWindow& window) const;

    void drawRect(const Point& point, const Vector& size, const Color& color);
};