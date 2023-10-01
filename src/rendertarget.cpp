#include "rendertarget.h"

void RenderTarget::drawRect(const Point& point, const Vector& size, const Color& color) {
    sf::RectangleShape rect;
    rect.setFillColor(sf::Color(color.r, color.g, color.b, color.a));
    rect.setPosition(point.x, point.y);
    rect.setSize(sf::Vector2f(size.x, size.y));
    
    _data.draw(rect);
}

void RenderTarget::display(sf::RenderWindow& window) const {
    window.draw(sf::Sprite(_data.getTexture()));
}