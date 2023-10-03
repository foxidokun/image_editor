#pragma once
#include "rendertarget.h"
#include "list.h"
#include "point.h"
#include "vector.h"
#include <chrono>

class Renderable {
    virtual void render(RenderTarget& target, const Point& start_pos) const = 0;
};

typedef char keyboard_event_t;

using time_point = std::chrono::time_point<std::chrono::system_clock>;

struct mouse_event_t {
    double x;
    double y;

    enum class button_type {
        NONE,
        LEFT,
        RIGHT,
        UNKNOWN
    } button;
};


enum class EVENT_RES {
    CONT,
    STOP
};

class Widget: public Renderable {
protected:
    Widget* _parent;
    list<Widget *> _childs;
    Point _pos;
    Vector _size;

public:
    Widget(Widget* parent, const Point& pos, const Vector& size):
        _parent(parent),
        _pos(pos),
        _size(size)
        {}

    virtual EVENT_RES on_keyboard_press  (const keyboard_event_t& key);
    virtual EVENT_RES on_keyboard_release(const keyboard_event_t& key);
    virtual EVENT_RES on_mouse_press     (const mouse_event_t& key);
    virtual EVENT_RES on_mouse_release   (const mouse_event_t& key);
    virtual EVENT_RES on_mouse_move      (const mouse_event_t& key);
    virtual EVENT_RES on_timer           (const time_point& time);

    virtual void render(RenderTarget& target, const Point& start_pos) const override {
        for (const auto& child: _childs) {
            child->render(target, start_pos + _pos);
        }
    }

    void register_object(Widget *child) {
        _childs.push_back(child);
    }

    void unregister_object(Widget *rem_child) {
        for (auto child = _childs.begin(); child != _childs.end(); ++child) {
            if (*child == rem_child) {
                delete (*child);
                child = _childs.erase(child);
            }
        }
    }

    const Point& pos()   const { return _pos;    }
    const Vector& size() const { return _size;   }
    Widget* parent()     const { return _parent; }

    virtual ~Widget() {
        for (const auto &x: _childs) {
            delete x;
        }
    }
};

class WindowManager: public Widget {
public:
    WindowManager(double width, double height):
        Widget(nullptr, Point(0,0), Vector(width, height))
        {}
};