#pragma once

#include "rendertarget.h"
#include "widget.h"
#include "tools.h"

class Canvas: public Widget {
private:
    sf::RenderTexture _data;
    Tool* drawing_tool;
    bool is_drawing;

public:
    Canvas(const Point& pos, const Vector& size, Tool *drawing_tool):
        Widget(pos, size),
        drawing_tool(drawing_tool),
        is_drawing(false) {
            _data.create(_size.x, _size.y);
            _data.clear(sf::Color::Yellow); // debug purpose only
        }

    void render(RenderTarget& target) const final;
    EVENT_RES on_mouse_press(const mouse_event_t& key) final;
    EVENT_RES on_mouse_release(const mouse_event_t& key) final;
    EVENT_RES on_mouse_move(const mouse_event_t& key) final;
};