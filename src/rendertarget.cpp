#include "rendertarget.h"

void RenderTarget::drawRect(const Region& reg, const Point& point, const Vector& size, const Color& fillcolor) {
    return drawRect(point, size, fillcolor);
    Region wanted;
    wanted.add_rectangle({point.x, point.y, point.x+size.x, point.y+size.y});
    wanted *= reg;

    sf::RectangleShape rect;
    rect.setFillColor(fillcolor);

    for (const auto &disp_rect: wanted.rects()) {
        double width  = disp_rect.high_x - disp_rect.low_x;
        double height = disp_rect.high_y - disp_rect.low_y;
        rect.setPosition(disp_rect.low_x, disp_rect.low_y);
        rect.setSize(sf::Vector2f(width, height));
    
        _data.draw(rect);
    }
}

void RenderTarget::drawRect(const Point& point, const Vector& size, const Color& fillcolor) {
    sf::RectangleShape rect;
    rect.setFillColor(fillcolor);
    rect.setPosition(point.x, point.y);
    rect.setSize(sf::Vector2f(size.x, size.y));
    _data.draw(rect);
}

void RenderTarget::drawLine(const Region& reg, const Point& point, const Vector& size, const Color& fillcolor) {
    return drawLine(point, size, fillcolor);
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

        drawLine(start_point, new_size, fillcolor);
    }
}

void RenderTarget::drawLine(const Point& start_point, const Vector& size, const Color& fillcolor) {
    Point end_point = start_point + size;
    Vector ortho = Vector(size.y, -size.x).norm() * LINE_THICKNESS;

    sf::VertexArray line(sf::TriangleStrip, 4);
    line[0].position = (sf::Vector2f) (start_point - ortho);
    line[1].position = (sf::Vector2f) (start_point + ortho);
    line[2].position = (sf::Vector2f) (  end_point - ortho);
    line[3].position = (sf::Vector2f) (  end_point + ortho);

    for (int i = 0; i < 4; ++i) {
        line[i].color = fillcolor;
    }

    _data.draw(line);
}

void RenderTarget::display(sf::RenderWindow& window) {
    _data.display();
    window.draw(sf::Sprite(_data.getTexture()));
}

void RenderTarget::drawText(const Region& reg, const Point& point, const char* text, uint size, const Color& backcolor)
{
    // sf::RenderTexture ghost;
    // ghost.create(_data.getSize().x, _data.getSize().y);
    // ghost.clear(backcolor);

    sf::Text textobj;
    textobj.setFont(global_resources::font);
    textobj.setString(text);
    textobj.setPosition(point.x, point.y);
    textobj.setCharacterSize(size);
    textobj.setFillColor(TEXT_COLOR);

    _data.draw(textobj);
    // ghost.display();

    // for (const auto &disp_rect: reg.rects()) {
    //     float width  = disp_rect.high_x - disp_rect.low_x;
    //     float height = disp_rect.high_y - disp_rect.low_y;

    //     sf::Sprite part_sprite(ghost.getTexture(), sf::Rect<int>(disp_rect.low_x, disp_rect.low_y, width, height));
    //     part_sprite.setPosition(disp_rect.low_x, disp_rect.low_y);

    //     _data.draw(part_sprite);
    // }
}

void RenderTarget::drawTexture(const Region& reg, const Point& point, const Vector& size, const Texture& texture,
    bool invert)
{
    return drawTexture(point, size, texture, invert);

    sf::RenderTexture ghost;
    ghost.create(_data.getSize().x, _data.getSize().y);
    ghost.clear(sf::Color::Transparent);

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

        if (invert) {
            _data.draw(part_sprite, &invert_shader);
        } else {
            _data.draw(part_sprite);
        }

    }
}

void RenderTarget::drawTexture(const Point& point, const Vector& size, const Texture& texture,
    bool invert)
{
    sf::Sprite sprite(texture);
    sprite.setPosition(point.x, point.y);

    double x_scale = size.x / texture.getSize().x;
    double y_scale = size.y / texture.getSize().y;

    sprite.setScale(x_scale, y_scale);

    if (invert) {
        _data.draw(sprite, &invert_shader);
    } else {
        _data.draw(sprite);
    }
}


const int color_len = 6;
const sf::Color COLORS[color_len] = {
    sf::Color::Red,     sf::Color::Blue,
    sf::Color::Cyan,    sf::Color::Black,
    sf::Color::Magenta, sf::Color::Yellow
};

#if DRAW_REGIONS
void RenderTarget::drawRegions(const Region& reg) {
    int color_i = 0;

    for (uint i = 0; i < reg.rects().size(); ++i) {
        color_i = (color_i+1) % color_len;
        sf::RectangleShape rect;
        rect.setPosition(reg.rects()[i].low_x, reg.rects()[i].low_y);
        rect.setSize(sf::Vector2f(reg.rects()[i].high_x - reg.rects()[i].low_x, reg.rects()[i].high_y - reg.rects()[i].low_y));
        rect.setOutlineThickness(1);
        auto black =  sf::Color::Black;
        black.a = 50;
        rect.setOutlineColor(black);
        sf::Color fillcolor = COLORS[color_i];
        fillcolor.a = 50;
        rect.setFillColor(fillcolor);
        _data.draw(rect);
    }
}
#endif

void RenderTarget::drawCircle(const Region& reg, const Point& point, double radius, const Color& fillcolor) {
    return drawCircle(point, radius, fillcolor);

    sf::RenderTexture ghost;
    ghost.create(_data.getSize().x, _data.getSize().y);
    ghost.clear(sf::Color::Transparent);

    sf::CircleShape circle;
    circle.setRadius(radius);
    circle.setFillColor(fillcolor);
    circle.setPosition(point.x, point.y);

    ghost.draw(circle);
    ghost.display();

    for (const auto &disp_rect: reg.rects()) {
        float width  = disp_rect.high_x - disp_rect.low_x;
        float height = disp_rect.high_y - disp_rect.low_y;

        sf::Sprite part_sprite(ghost.getTexture(), sf::Rect<int>(disp_rect.low_x, disp_rect.low_y, width, height));
        part_sprite.setPosition(disp_rect.low_x, disp_rect.low_y);

        _data.draw(part_sprite);
    }
}

void RenderTarget::drawCircle(const Point& point, double radius, const Color& fillcolor) {
    sf::CircleShape circle;
    circle.setRadius(radius);
    circle.setFillColor(fillcolor);
    circle.setPosition(point.x, point.y);

    _data.draw(circle);
}

void RenderTarget::drawEllipse(const Point& point, const Vector& size, const Color& fillcolor) {
    sf::CircleShape circle;

    double max_size = std::max(fabs(size.x), fabs(size.y));

    circle.setPosition((sf::Vector2f)point);
    circle.setFillColor(fillcolor);
    circle.setRadius(max_size / 2);
    circle.setScale(size.x / max_size, size.y / max_size);

    _data.draw(circle);
}

void RenderTarget::drawRenderTarget(const Region& reg, const Point& point, const RenderTarget& rt) {
    // sf::RenderTexture ghost;
    // ghost.create(_data.getSize().x, _data.getSize().y);
    // ghost.clear(sf::Color::Transparent);

    sf::Sprite fullsprite(rt._data.getTexture());
    fullsprite.setPosition(point.x, point.y);
    _data.draw(fullsprite);
    // ghost.display();

    // for (const auto &disp_rect: reg.rects()) {
    //     float width  = disp_rect.high_x - disp_rect.low_x;
    //     float height = disp_rect.high_y - disp_rect.low_y;

    //     sf::Sprite part_sprite(ghost.getTexture(), sf::Rect<int>(disp_rect.low_x, disp_rect.low_y, width, height));
    //     part_sprite.setPosition(disp_rect.low_x, disp_rect.low_y);

    //     _data.draw(part_sprite);
    // }
}

void RenderTarget::loadFromFile(const char *filename) {
    sf::Texture texture;
    texture.loadFromFile(filename);

    drawTexture(Point(0,0), (Vector)_data.getSize(), texture);
}

void RenderTarget::saveToFile(const char *filename) {
    sf::Image image = _data.getTexture().copyToImage();
    image.saveToFile(filename);
}

RawImage RenderTarget::get_image() const {
    sf::Image img = _data.getTexture().copyToImage();
    const uint8_t *pix_ptr = img.getPixelsPtr();
    return RawImage(_data.getSize().x, _data.getSize().y, pix_ptr);
}

void RenderTarget::set_image(const RawImage& img) {
    sf::Texture texture;
    texture.create(img.width(), img.height());
    texture.update(img.get_bytes());
    _data.draw(sf::Sprite(texture));
}

void RenderTarget::setPixel(Vec2 pos, Color color) {
    sf::Vertex vertex;
    vertex.position = pos;
    vertex.color = color;
    _data.draw(&vertex, 1, sf::Points);
}

void RenderTarget::drawTexture(Vec2 pos, Vec2 size, const plugin::Texture *img) {
    drawTexture(pos, size, img, false);
}

void RenderTarget::drawTexture(Vec2 pos, Vec2 size, const plugin::Texture *img, bool invert) {
    sf::Texture texture;
    texture.create(img->width(), img->height());
    texture.update(img->get_bytes());
    
    sf::Sprite sprite(texture);
    sprite.setPosition(pos.x, pos.y);

    double x_scale = size.x / texture.getSize().x;
    double y_scale = size.y / texture.getSize().y;

    sprite.setScale(x_scale, y_scale);
     if (invert) {
        _data.draw(sprite, &invert_shader);
    } else {
        _data.draw(sprite);
    }
}

void RenderTarget::drawText(Vec2 pos, const char *content, uint16_t char_size, Color color) {
    sf::Text textobj;
    textobj.setFont(global_resources::font);
    textobj.setString(content);
    textobj.setPosition(pos.x, pos.y);
    textobj.setCharacterSize(char_size);
    textobj.setFillColor(TEXT_COLOR);

    _data.draw(textobj);
}