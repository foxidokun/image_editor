#pragma once

#include <SFML/Graphics.hpp>
#include "vector.h"
#include "point.h"
#include "regions.h"
#include "colors.h"

typedef sf::Texture Texture;

#define DRAW_REGIONS 0

class RenderTarget {
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

    void clear(const Color& color) { _data.clear(color); };
    
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

    Color get_pixel(const Vector& pos) const;

    #if DRAW_REGIONS
        void drawRegions(const Region& reg);
    #else
        void drawRegions(const Region& reg) {}
    #endif
};

static inline sf::Color convert_color(const Color& color) {
    return sf::Color(color.r, color.g, color.b, color.a);
}