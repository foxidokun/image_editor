#pragma once
#include "rendertarget.h"
#include "list.h"
#include "point.h"
#include "vector.h"
#include <chrono>
#include <cassert>
#include "regions.h"

class Renderable {
    virtual void render(RenderTarget& target) const = 0;
};

typedef char keyboard_event_t;

using time_point = std::chrono::time_point<std::chrono::system_clock>;

class Widget;
using transform_f = Widget*(*)(Widget *, void *);
using checker_f   = bool(*)(Widget *, void *); 

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
    linked_list<Widget *> _childs;
    Point _pos;
    Vector _size;
    Region _reg;
    Rectangle _active_area;

public:
    Widget(const Point& pos, const Vector& size):
        _pos(pos),
        _size(size)
        {
            Rectangle whole = {_pos.x, _pos.y, _pos.x+_size.x, _pos.y+_size.y};
            _reg.add_rectangle(whole);
            _active_area = whole;
        }

    virtual EVENT_RES on_keyboard_press  (const keyboard_event_t& key);
    virtual EVENT_RES on_keyboard_release(const keyboard_event_t& key);
    virtual EVENT_RES on_mouse_press     (const mouse_event_t& key);
    virtual EVENT_RES on_mouse_release   (const mouse_event_t& key);
    virtual EVENT_RES on_mouse_move      (const mouse_event_t& key);
    virtual EVENT_RES on_timer           (const time_point& time);

    virtual void render(RenderTarget& target) const override {
        for (const auto& child: _childs) {
            assert (child != this);
            child->render(target);
        }
    }

    void register_object(Widget *child);

    void unregister_object(Widget *rem_child) {
        for (auto child = _childs.begin(); child != _childs.end(); ++child) {
            if (*child == rem_child) {
                delete (*child);
                child = _childs.erase(child);
            }
        }
    }

    const Point&     pos()         const { return _pos;         }
    const Vector&    size()        const { return _size;        }
    const Rectangle& active_area() const { return _active_area; }
    Widget*          parent()      const { return _parent;      }
    
    Region& reg() { return _reg; }

    virtual ~Widget() {
        for (const auto &x: _childs) {
            delete x;
        }
    }

    friend void recursive_update(Widget **widget, transform_f func, void* args, 
                     checker_f check, void* check_args);
    friend Widget* update_coords(Widget *widget, void *args);
};

class WindowManager: public Widget {
public:
    WindowManager(double width, double height):
        Widget(Point(0,0), Vector(width, height))
        {}
};

void recursive_update(Widget **widget, transform_f func, void* args, 
                     checker_f check = nullptr, void* check_args = nullptr);
Widget* update_coords(Widget *widget, void *args);
Widget* return_region(Widget* const widget, void* args_);
Widget* cut_region(Widget* const widget, void* args_);
