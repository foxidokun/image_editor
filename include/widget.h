#pragma once
#include "rendertarget.h"
#include "list.h"
#include "point.h"
#include "vector.h"

class Renderable {
    virtual void render(RenderTarget& target, const Point& start_pos) const = 0;
};

typedef char keyboard_event_t;

struct mouse_event_t {
    double x;
    double y;

    enum class button {
        NONE,
        LEFT,
        RIGHT
    } button;
};


enum class EVENT_RES {
    CONT,
    STOP
};

class Widget: public Renderable {
protected:
    list<Widget *> _childs;
    Point _pos;
    Vector _size;

public:
    Widget(const Point& pos, const Vector& size):
        _pos(pos),
        _size(size)
        {}

    virtual EVENT_RES on_keyboard_press  (const keyboard_event_t& key) { return EVENT_RES::CONT; };
    virtual EVENT_RES on_keyboard_release(const keyboard_event_t& key) { return EVENT_RES::CONT; };
    virtual EVENT_RES on_mouse_press     (const mouse_event_t& key)    { return EVENT_RES::CONT; };
    virtual EVENT_RES on_mouse_release   (const mouse_event_t& key)    { return EVENT_RES::CONT; };
    virtual EVENT_RES on_mouse_move      (const mouse_event_t& key)    { return EVENT_RES::CONT; };
    virtual EVENT_RES on_timer           ()                            { return EVENT_RES::CONT; };

    void register_object(Widget *child) {
        _childs.push_back(child);
    }
};