#include "rendertarget.h"

void RenderTarget::drawRect(const Region& reg, const Point& point, const Vector& size, const Color& fillcolor) {
    Region wanted;
    wanted.add_rectangle({point.x, point.y, point.x+size.x, point.y+size.y});
    wanted *= reg;

    sf::RectangleShape rect;
    rect.setFillColor(convert_color(fillcolor));

    for (const auto &disp_rect: wanted.rects()) {
        double width  = disp_rect.high_x - disp_rect.low_x;
        double height = disp_rect.high_y - disp_rect.low_y;
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
        Point start_point = point;
        Point end_point   = point + size;
        start_point.x = std::max(start_point.x, disp_rect.low_x);
        start_point.y = std::max(start_point.y, disp_rect.low_y);
        end_point.x   = std::min(end_point.x,   disp_rect.high_x);
        end_point.y   = std::min(end_point.y,   disp_rect.high_y);

        Vector new_size = end_point - start_point;

        if (new_size.x < 0 || new_size.y < 0) {
            continue;
        }

        rect.setSize(sf::Vector2f(new_size.length(), LINE_THICKNESS));
        rect.setPosition(start_point.x, start_point.y);

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

        sf::Sprite part_sprite(ghost.getTexture(), sf::Rect<int>(disp_rect.low_x, disp_rect.low_y, width, height));
        part_sprite.setPosition(disp_rect.low_x, disp_rect.low_y);

        _data.draw(part_sprite);
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

        sf::Sprite part_sprite(ghost.getTexture(), sf::Rect<int>(disp_rect.low_x, disp_rect.low_y, width, height));
        part_sprite.setPosition(disp_rect.low_x, disp_rect.low_y);
        _data.draw(part_sprite);
    }
}
