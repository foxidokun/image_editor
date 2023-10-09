#include "rendertarget.h"

static sf::Color convert_color(const Color& color) {
    return sf::Color(color.r, color.g, color.b, color.a);
}

void RenderTarget::drawRect(const Region& reg, const Point& point, const Vector& size, const Color& fillcolor, const Color& bordercolor) {
    Region wanted;
    wanted.add_rectangle({point.x, point.y, point.x+size.x, point.y+size.y});
    wanted *= reg;

    sf::RectangleShape rect;
    rect.setFillColor(convert_color(fillcolor));
    rect.setOutlineColor(convert_color(bordercolor));
    rect.setOutlineThickness(LINE_THICKNESS);

    for (const auto &disp_rect: wanted.rects()) {
        float width  = disp_rect.high_x - disp_rect.low_x;
        float height = disp_rect.high_y - disp_rect.low_y;
        rect.setPosition(disp_rect.low_x, disp_rect.low_y);
        rect.setSize(sf::Vector2f(width, height));
    
        _data.draw(rect);
    }
}

void RenderTarget::drawLine(const Region& reg, const Point& point, const Vector& size, const Color& fillcolor) {
    
    sf::RectangleShape rect;
    rect.setFillColor(convert_color(fillcolor));
    rect.setRotation(radians_to_degrees(atan(size.y / size.x)));

    for (const auto &disp_rect: reg.rects()) {
        Point  new_pos  = Point(std::max(point.x, disp_rect.low_x), std::max(point.y, disp_rect.low_y));
        Vector new_size = Vector(std::min(size.x, disp_rect.high_x - new_pos.x),
                                std::min(size.y, disp_rect.high_y - new_pos.y));

        rect.setSize(sf::Vector2f(new_size.length(), LINE_THICKNESS));
        rect.setPosition(new_pos.x, new_pos.y);


        _data.draw(rect);
    }
}

void RenderTarget::display(sf::RenderWindow& window) {
    _data.display();
    window.draw(sf::Sprite(_data.getTexture()));
}

void RenderTarget::drawText(const Region& reg, const Point& point, const char* text, uint size) {
    sf::RenderTexture ghost;
    ghost.create(_data.getSize().x, _data.getSize().y);

    sf::Text textobj;
    textobj.setFont(global_font);
    textobj.setString(text);
    textobj.setPosition(point.x, point.y);
    textobj.setCharacterSize(size);
    textobj.setFillColor(sf::Color(0,0,0,255));

    ghost.draw(textobj);
    ghost.display();

    for (const auto &disp_rect: reg.rects()) {
        float width  = disp_rect.high_x - disp_rect.low_x;
        float height = disp_rect.high_y - disp_rect.low_y;
        ghost.setView(sf::View(sf::Rect((float)disp_rect.low_x, (float)disp_rect.high_y, width, height)));
        _data.draw(sf::Sprite(ghost.getTexture()));
    }
}

void RenderTarget::drawTexture(const Region& reg, const Point& point, const Vector& size, const Texture& texture) {
    sf::RenderTexture ghost;
    ghost.create(_data.getSize().x, _data.getSize().y);

    sf::Sprite sprite(texture);
    sprite.setPosition(point.x, point.y);

    double x_scale = size.x / texture.getSize().x;
    double y_scale = size.y / texture.getSize().y;

    sprite.setScale(x_scale, y_scale);

    ghost.draw(sprite);
    ghost.display();

    for (const auto &disp_rect: reg.rects()) {
        float width  = disp_rect.high_x - disp_rect.low_x;
        float height = disp_rect.high_y - disp_rect.low_y;
        ghost.setView(sf::View(sf::Rect((float)disp_rect.low_x, (float)disp_rect.high_y, width, height)));
        _data.draw(sf::Sprite(ghost.getTexture()));
    }
}
