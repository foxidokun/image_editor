#pragma once

#include <SFML/Graphics.hpp>
#include "point.h"
#include "rendertarget.h"

class Tool {
public:
    virtual void paint(sf::RenderTexture& canvas, const Point& pos) = 0;
    virtual ~Tool() = default;
};

class ToolManager: public Tool {
private:
    Tool *active_tool; // owned

public:
    ToolManager(): active_tool(nullptr) {}
    ToolManager(Tool *tool): active_tool(tool) {}

    void paint(sf::RenderTexture& canvas, const Point& pos) final {
        active_tool->paint(canvas, pos);
    }

    void set_tool(Tool *new_tool) {
        delete active_tool;
        active_tool = new_tool;
    }

    ~ToolManager() override {
        delete active_tool;
    }
};

class Brush: public Tool {
private:
    double _radius;
    Color _color;
public:
    Brush(double radius, const Color& color): _radius(radius), _color(color) {}

    void paint(sf::RenderTexture& canvas, const Point& pos) final;
};