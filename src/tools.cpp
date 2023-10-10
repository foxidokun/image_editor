#include "tools.h"

void Brush::paint(sf::RenderTexture& canvas, const Point& pos, const Color& color) {
    sf::CircleShape circle;
    circle.setRadius(_radius);
    circle.setPosition(pos.x, pos.y);
    circle.setFillColor(convert_color(color));
    canvas.draw(circle);
}