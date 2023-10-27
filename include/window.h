#pragma once
#include "widget.h"
#include "string.h"
#include "button.h"

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

    void render(RenderTarget& target) override;

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
        event_mgr.register_object(this);
    }

    ~ModalWindow() override {
        event_mgr.unregister_object(this);
        event_mgr.reset_priority();
    }
};

class ParametersModalWindow: public ModalWindow {
private:
    callback_f on_delete;
    CallbackArgs* on_delete_args;
    dynarray<string> parameters;

public:
    ParametersModalWindow(const Point& pos, const Vector& size, const string& title, EventManager& event_mgr,
        callback_f on_delete, CallbackArgs* on_delete_args, dynarray<string> parameters):
        ModalWindow(pos, size, title, event_mgr),
        on_delete(on_delete),
        on_delete_args(on_delete_args),
        parameters(parameters)
        {
            for (uint i = 0; i < parameters.size(); ++i) {
                register_object_exact_pos(new TextBox(_pos + Vector(100, 10 + HEADER_HEIGHT + i*20), Vector(90, 18)));
            }
        }

    void render(RenderTarget& rt) final;

    ~ParametersModalWindow() override {
        on_delete(on_delete_args);
        delete on_delete_args;
    }
};