#pragma once

#include <cstring>
#include <SFML/Graphics.hpp>
#include "vector.h"
#include "point.h"
#include "regions.h"
#include "colors.h"
#include "plugin.h"

typedef sf::Texture Texture;

#define DRAW_REGIONS 0

using plugin::Vec2;
using RawImage = plugin::Texture;

class RenderTarget: public plugin::RenderTargetI {
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

    void loadFromFile(const char *filename);
    void saveToFile(const char *filename);
    
    void display() { _data.display(); }
    void display(sf::RenderWindow& window);

    void drawRect(const Region& reg, const Point& point, const Vector& size, const Color& fillcolor = {0,0,0,0});

    void drawLine(const Region& reg, const Point& point, const Vector& size, const Color& fillcolor = {0,0,0,255});
    
    void drawCircle(const Region& reg, const Point& point, double radius, const Color& fillcolor = {0,0,0,255});

    void drawRect(const Point& point, const Vector& size, const Color& fillcolor = {0,0,0,0});

    void drawLine(const Point& point, const Vector& size, const Color& fillcolor = {0,0,0,255});
    
    void drawCircle(const Point& point, double radius, const Color& fillcolor = {0,0,0,255});
    
    void drawEllipse(const Point& point, const Vector& size, const Color& fillcolor = {0,0,0,0});

    void drawText(const Region& reg, const Point& point, const char* text, uint size, const Color& backcolor = sf::Color::Transparent);

    void drawTexture(const Region& reg, const Point& point, const Vector& size, const Texture& texture,
        bool invert = false);
    void drawTexture(const Point& point, const Vector& size, const Texture& texture,
        bool invert = false);

    void drawRenderTarget(const Region& reg, const Point& point, const RenderTarget& rt);

    RawImage get_image() const;
    void set_image(const RawImage& img);

    // RenderTargetI Interface

    void setPixel(Vec2 pos, Color color) final;
    void drawLine(Vec2 point1, Vec2 point2, Color color) final { drawLine((Point)point1, point2 - point1, color); };
    void drawRect(Vec2 pos, Vec2 size, Color color) final { drawRect((Point)pos, (Vector)size, color); };
    void drawEllipse(Vec2 pos, Vec2 size, Color color) final { drawEllipse((Point)pos, (Vector)size, color); };
    void drawTexture(Vec2 pos, Vec2 size, const plugin::Texture *texture) final;
    void drawTexture(Vec2 pos, Vec2 size, const plugin::Texture *texture, bool invert);
    void drawText(Vec2 pos, const char *content, uint16_t char_size, Color color) final;

    plugin::Texture *getTexture() const final {
        auto res = new plugin::Texture(get_image());
        return res;
    }

    void setTexture(plugin::Texture *) final {assert(0);};

    void clear() final {clear(sf::Color::Transparent); };
    void clear(plugin::Color color) final { _data.clear(color); };

    #if DRAW_REGIONS
        void drawRegions(const Region& reg);
    #else
        void drawRegions(const Region& reg) {}
    #endif
};