#include "rendertarget.h"

static sf::Color convert_color(const Color& color) {
    return sf::Color(color.r, color.g, color.b, color.a);
}

void RenderTarget::drawRect(const Point& point, const Vector& size, const Color& fillcolor, const Color& bordercolor) {
    sf::RectangleShape rect;
    rect.setFillColor(convert_color(fillcolor));
    rect.setOutlineColor(convert_color(bordercolor));
    rect.setOutlineThickness(LINE_THICKNESS);
    rect.setPosition(point.x, point.y);
    rect.setSize(sf::Vector2f(size.x, size.y));
    
    _data.draw(rect);
}

void RenderTarget::drawLine(const Point& point, const Vector& size, const Color& fillcolor) {
    sf::RectangleShape rect;
    rect.setSize(sf::Vector2f(size.length(), LINE_THICKNESS));
    rect.setRotation(radians_to_degrees(atan(size.y / size.x)));
    rect.setFillColor(convert_color(fillcolor));
    rect.setPosition(point.x, point.y);

    _data.draw(rect);
}

void RenderTarget::display(sf::RenderWindow& window) {
    _data.display();
    window.draw(sf::Sprite(_data.getTexture()));
}

void RenderTarget::drawText(const Point& point, const char* text, uint size) {
    sf::Text textobj;
    textobj.setFont(global_font);
    textobj.setString(text);
    textobj.setPosition(point.x, point.y);
    textobj.setCharacterSize(size);
    textobj.setFillColor(sf::Color(0,0,0,255));

    _data.draw(textobj);
}

void RenderTarget::drawTexture(const Point& point, const Vector& size, const Texture& texture) {
    sf::Sprite sprite(texture);
    sprite.setPosition(point.x, point.y);

    double x_scale = size.x / texture.getSize().x;
    double y_scale = size.y / texture.getSize().y;

    sprite.setScale(x_scale, y_scale);

    _data.draw(sprite);
}
