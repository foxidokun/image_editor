#pragma once
#include "widget.h"
#include "string.h"

class Window: public Widget {
private:
    string _title;
    bool is_moving = false;
    Point last_pos;

    void initialise();

public:
    Window(const Point& pos, const Vector& size, const string& title):
        Widget(pos, size),
        _title(title) {
            initialise();
        }

    Window(const Point& pos, const Vector& size, string&& title):
        Widget(pos, size),
        _title(title) {
            initialise();
        }

    void render(RenderTarget& target) const final;

    EVENT_RES on_mouse_press   (const mouse_event_t& key) final;
    EVENT_RES on_mouse_release (const mouse_event_t& key) final;
    EVENT_RES on_mouse_move    (const mouse_event_t& key) final;
};