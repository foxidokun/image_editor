#include "tools.h"

void Brush::paint(sf::RenderTexture& canvas, const Point& pos) {
    sf::CircleShape circle;
    circle.setRadius(_radius);
    circle.setPosition(pos.x, pos.y);
    circle.setFillColor(convert_color(_color));
    canvas.draw(circle);
}