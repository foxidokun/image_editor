#pragma once

#include "rendertarget.h"
#include "widget.h"
#include "tools.h"

class Canvas: public Widget {
private:
    RenderTarget _permanent;
    RenderTarget _tmp;
    const ToolManager* tool_manager;
    bool is_drawing;

public:
    Canvas(const Point& pos, const Vector& size, const ToolManager *tool_manager):
        Widget(pos, size),
        tool_manager(tool_manager),
        is_drawing(false),
        _permanent(RenderTarget(size)),
        _tmp(RenderTarget(size))
        {
            _permanent.clear(sf::Color::Yellow);
            _tmp.clear(sf::Color::Transparent);
        }

    void render(RenderTarget& target) final;
    EVENT_RES on_mouse_press(const mouse_event_t& key) final;
    EVENT_RES on_mouse_release(const mouse_event_t& key) final;
    EVENT_RES on_mouse_move(const mouse_event_t& key) final;
};