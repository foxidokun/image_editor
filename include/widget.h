#pragma once
#include "rendertarget.h"
#include "list.h"
#include "point.h"
#include "vector.h"
#include <chrono>
#include <cassert>
#include <iostream>
#include "regions.h"

class Renderable {
    virtual void render(RenderTarget& target) = 0;
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
    Widget* _parent = nullptr;
    Widget* _root   = this;
    linked_list<Widget *> _childs;
    Point _pos;
    Vector _size;
    Region _reg;
    Rectangle _active_area;
    bool _is_alive = true;

protected:
    bool recursive_cleanup();

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

    virtual void render(RenderTarget& target) override {
        for (auto child = _childs.rbegin(); child != _childs.rend(); ++child) {
            assert (*child != this);
            (*child)->render(target);
        }
    }

    void register_object(Widget *child);
    void register_object_exact_pos(Widget *child);

    void kill()     { _is_alive = false; }
    bool is_alive() { return _is_alive; }

    const Point&     pos()         const { return _pos;         }
    const Vector&    size()        const { return _size;        }
    const Rectangle& active_area() const { return _active_area; }
    Widget*          parent()      const { return _parent;      }
    
    void             set_pos   (const Point&   pos) { _pos  = pos;  }
    void             set_size  (const Vector& size) { _size = size; }
    
    Region& reg() { return _reg; }

    virtual ~Widget() {
        for (const auto &x: _childs) {
            delete x;
        }
    }

    friend void recursive_update(Widget **widget, transform_f func, void* args, 
                     checker_f check, void* check_args);
    friend Widget* update_coords(Widget *widget, void *args);
    friend Widget* set_root(Widget *const widget, void *args);

    void recalc_regions();

    virtual void print(std::ostream& stream) const {
        stream << "basic widget";
    }

    Rectangle get_hit_rectangle() {
        return {_pos.x, _pos.y, _pos.x + _size.x, _pos.y + _size.y};
    }
};

class WindowManager: public Widget {
public:
    WindowManager(double width, double height):
        Widget(Point(0,0), Vector(width, height))
        {
            _root = this;
            _active_area.low_y += MENU_HEIGHT;
        }

    void print(std::ostream& stream) const final {
        stream << "Window manager";
    }

    void cleanup() {
        bool cleaned = recursive_cleanup();
        if (cleaned) {
            recalc_regions();
        }
    }

    virtual void render(RenderTarget& target) override {
        cleanup();
        target.drawRect(_reg, _pos, _size, WM_BACKGROUND_COLOR);
        target.drawLine(_reg, _pos + Vector(0, HEADER_HEIGHT), Vector(_size.x, 0), sf::Color::Black);

        Widget::render(target);
    }
};

class ColorIndicator: public Widget {
private:
    const Color *color_ptr;
public:
    ColorIndicator(const Point& pos, const Vector& size, const Color* color_ptr): 
        Widget(pos, size),
        color_ptr(color_ptr)
        {}

    virtual void render(RenderTarget& target) override {
        target.drawRect(_reg, _pos, _size, *color_ptr);
    }
};

void recursive_update(Widget **widget, transform_f func, void* args, 
                     checker_f check = nullptr, void* check_args = nullptr);
Widget* update_coords(Widget *widget, void *args);
Widget* return_region(Widget* const widget, void* args_);
Widget* cut_region(Widget* const widget, void* args_);
