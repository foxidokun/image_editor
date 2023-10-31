#pragma once
#include "widget.h"
#include "string.h"
#include "button.h"

class Window: public Widget {
private:
    string _title;
    bool is_moving = false;
    Point last_pos;

    void initialize();

public:
    Window(const Point& pos, const Vector& size, const string& title):
        Widget(pos, size),
        _title(title) {
            initialize();
        }

    Window(const Point& pos, const Vector& size, string&& title):
        Widget(pos, size),
        _title(title) {
            initialize();
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
    callback_f on_apply;
    CallbackArgs* on_apply_args;
    dynarray<const char *> parameters;
    dynarray<TextBox *> boxes;

    void initialize();

public:
    ParametersModalWindow(const Point& pos, const Vector& size, const string& title, EventManager& event_mgr,
        callback_f on_apply, CallbackArgs* on_apply_args, dynarray<const char *> parameters):
        ModalWindow(pos, size, title, event_mgr),
        on_apply(on_apply),
        on_apply_args(on_apply_args),
        parameters(parameters)
        {
            initialize();
        }

    void render(RenderTarget& rt) final;

    void apply() const {
        if (on_apply) {
            on_apply(on_apply_args);
        }
    }

    dynarray<string> get_params() {
        dynarray<string> ans;
        for (const auto box: boxes) {
            ans.push_back(box->get_content());
        }
        return ans;
    }

    ~ParametersModalWindow() override {
        delete on_apply_args;
    }
};