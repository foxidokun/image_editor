#include "widget.h"
#include <type_traits>
#include <cassert>

// TODO: implement priority logic

static inline bool no_hit(const Point& pos, const Vector& size, const mouse_event_t& event);
Widget* update_coords(Widget *widget, void *args);

template<typename T>
using handler_func_t = EVENT_RES (Widget::*)(const T& event);

const Vector SAFETY_AROUND = {20, 20};

template<typename T>
static EVENT_RES default_event_handler(const list<Widget *>& childs, handler_func_t<T> handler_func, const T& event) {
    for (auto& child: childs) {
        if constexpr (std::is_same_v<T, mouse_event_t>) {
            if (no_hit(child->pos() - SAFETY_AROUND, child->size() + 2*SAFETY_AROUND, event)) {
                continue;
            }
        }

        if ((child->*handler_func)(event) == EVENT_RES::STOP) {
            return EVENT_RES::STOP;
        }
    }

    return EVENT_RES::CONT;
}



EVENT_RES Widget::on_keyboard_press(const keyboard_event_t& key) {
    return default_event_handler<keyboard_event_t>(_childs, &Widget::on_keyboard_press, key);
}


EVENT_RES Widget::on_keyboard_release(const keyboard_event_t& key) {
    return default_event_handler<keyboard_event_t>(_childs, &Widget::on_keyboard_release, key);
}


EVENT_RES Widget::on_mouse_press(const mouse_event_t& _key) {
    mouse_event_t key = _key;

    if (no_hit(_pos, _size, key)) { return EVENT_RES::CONT; }

    return default_event_handler<mouse_event_t>(_childs, &Widget::on_mouse_press, key);
}


EVENT_RES Widget::on_mouse_release(const mouse_event_t& _key) {
    mouse_event_t key = _key;
    
    if (no_hit(_pos - SAFETY_AROUND, _size + 2*SAFETY_AROUND, key)) { return EVENT_RES::CONT; }

    return default_event_handler<mouse_event_t>(_childs, &Widget::on_mouse_release, key);
}

EVENT_RES Widget::on_mouse_move(const mouse_event_t& _key) {
    mouse_event_t key = _key;
    
    if (no_hit(_pos - SAFETY_AROUND, _size + 2*SAFETY_AROUND, key)) { return EVENT_RES::CONT; }

    return default_event_handler<mouse_event_t>(_childs, &Widget::on_mouse_move, key);
}


EVENT_RES Widget::on_timer(const time_point& time) {
    return default_event_handler<time_point>(_childs, &Widget::on_timer, time);
}

void Widget::register_object(Widget *child) {
    recursive_update(&child, update_coords, &_pos);
    assert(child != this);
    
    child->_parent = this;
    _childs.push_back(child);
}

void recursive_update(Widget **widget_ptr, transform_f func, void* args) {
    Widget *widget = *widget_ptr;
    for (auto child = widget->_childs.begin(); child != widget->_childs.end(); ++child) {
        *child = func(*child, args);
    }

    widget = func(widget, args);
    *widget_ptr = widget;
}

static inline bool no_hit(const Point& pos, const Vector& size, const mouse_event_t& event) {
    double rel_x = event.x - pos.x;
    double rel_y = event.y - pos.y;

    return !((0 < rel_x && rel_x < size.x) && (0 < rel_y && rel_y < size.y));
}

Widget* update_coords(Widget *const widget, void *args) {
    Point base_point = *static_cast<Point *>(args);
    widget->_pos += base_point;
    widget->_reg.shift(Vector(base_point.x, base_point.y));
    return widget;
}