#pragma once

#include "rendertarget.h"
#include "widget.h"
#include "tools.h"

class Canvas: public Widget {
private:
    sf::RenderTexture _data;
    const ToolManager* tool_manager;
    bool is_drawing;
    Point last_pos;

public:
    Canvas(const Point& pos, const Vector& size, const ToolManager *tool_manager):
        Widget(pos, size),
        tool_manager(tool_manager),
        is_drawing(false) {
            _data.create(_size.x, _size.y);
            _data.clear(sf::Color::Yellow); // debug purpose only
        }

    void render(RenderTarget& target) const final;
    EVENT_RES on_mouse_press(const mouse_event_t& key) final;
    EVENT_RES on_mouse_release(const mouse_event_t& key) final;
    EVENT_RES on_mouse_move(const mouse_event_t& key) final;
};