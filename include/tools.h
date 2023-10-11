#pragma once

#include <SFML/Graphics.hpp>
#include "point.h"
#include "rendertarget.h"

struct click_info_t {
    Point pos;
    Point last_pos;
    Point start_pos;
};

class Tool {
public:
    virtual void paint(sf::RenderTexture& canvas, const click_info_t& click_info, const Color& color) = 0;
    virtual ~Tool() = default;
};

class ToolManager {
private:
    Tool *active_tool; // owned
    Color color;

public:
    ToolManager(): active_tool(nullptr), color(Color{0,0,0,255}) {}
    ToolManager(Tool *tool): active_tool(tool), color(Color{0,0,0,255}) {}
    ToolManager(Tool *tool, const Color& color): active_tool(tool), color(color) {}

    void paint(sf::RenderTexture& canvas, const click_info_t& click_info) const {
        if (active_tool) {
            active_tool->paint(canvas, click_info, color);
        }
    }

    void set_tool(Tool *new_tool) {
        delete active_tool;
        active_tool = new_tool;
    }

    void set_color(const Color& new_color) {
        color = new_color;
    }

    ~ToolManager() {
        delete active_tool;
    }
};

class Brush: public Tool {
private:
    double _radius;
public:
    Brush(double radius): _radius(radius) {}

    void paint(sf::RenderTexture& canvas, const click_info_t& click_info, const Color& color) final;
};

class AlienBrush: public Tool {
private:
    double _radius;
public:
    AlienBrush(double radius): _radius(radius) {}

    void paint(sf::RenderTexture& canvas, const click_info_t& click_info, const Color& color) final;
};