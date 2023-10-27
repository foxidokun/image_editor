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

    void render(RenderTarget& target) final;

    EVENT_RES on_mouse_press   (const mouse_event_t& key) final;
    EVENT_RES on_mouse_release (const mouse_event_t& key) final;
    EVENT_RES on_mouse_move    (const mouse_event_t& key) final;

    void print(std::ostream& stream) const final {
        stream << "Window " << _title;
    }
};

class ModalWindow: public Window {
private:
    EventManager &event_mgr;
public:
    ModalWindow(const Point& pos, const Vector& size, const string& title, EventManager& event_mgr):
        Window(pos, size, title),
        event_mgr(event_mgr)
    {
        _priority = EVENT_PRIORITIES::MODAL_WINDOW;
        event_mgr.set_priority({MOUSE_EVENT, KEYBOARD_EVENT}, (uint)EVENT_PRIORITIES::MODAL_WINDOW);
    }

    ~ModalWindow() override {
        event_mgr.reset_priority();
    }
};