#include "window.h"
#include "button.h"
#include <iostream>
#include "config.h"

const Color WINDOW_COLOR = {255,255,255,255};

struct WindowButtonArgs: public CallbackArgs {
    Window *window;

    WindowButtonArgs(Window *window_): window(window_) {};
};

static void close_window_callback(CallbackArgs *_args) {
    WindowButtonArgs *args = static_cast<WindowButtonArgs *>(_args);

    Widget *parent = args->window->parent();
    parent->unregister_object(args->window);
}

void Window::render(RenderTarget& target) {
    target.drawRect(_reg, _pos, _size, WINDOW_COLOR);

    // header sep
    target.drawLine(_reg, _pos + Point(0, HEADER_HEIGHT - LINE_THICKNESS), {_size.x, 0});

    // borders
    target.drawLine(_reg, _pos,                          {_size.x, 0});
    target.drawLine(_reg, _pos+Vector(0, _size.y-LINE_THICKNESS),       {_size.x, 0});
    target.drawLine(_reg, _pos+Vector(LINE_THICKNESS,0), {0, _size.y});
    target.drawLine(_reg, _pos+Vector(_size.x, 0), {0, _size.y});
    
    target.drawText(_reg, _pos +  Point(LINE_THICKNESS, LINE_THICKNESS), _title.c_str(), TITLE_SIZE);

    Widget::render(target);

    target.drawRegions(_reg);
}

void Window::initialise() {
    CallbackArgs *args = new WindowButtonArgs(this);
    Point pos = {_size.x - BUTTON_SIZE - LINE_THICKNESS, LINE_THICKNESS};
    Vector size = {BUTTON_SIZE, BUTTON_SIZE};
    TextureButton* button = new TextureButton(pos, size, close_window_callback, args, close_window_button);

    register_object(button);

    _active_area.low_y  += HEADER_HEIGHT;
    _active_area.low_x  += LINE_THICKNESS;
    _active_area.high_x -= LINE_THICKNESS;
    _active_area.high_y -= LINE_THICKNESS;
}

EVENT_RES Window::on_mouse_press(const mouse_event_t& key) {
    EVENT_RES res = Widget::on_mouse_press(key);
    if (res == EVENT_RES::CONT) {
        bool hit_y = key.y > _pos.y && key.y < _pos.y + HEADER_HEIGHT;
        bool hit_x = key.x > _pos.x && key.x < _pos.x + _size.x;
        if (hit_x && hit_y) {
            is_moving = true;
            last_pos = Point( _pos.x, _pos.y);
            return EVENT_RES::STOP;
        }
    }

    return res;
}

EVENT_RES Window::on_mouse_release(const mouse_event_t& key) {
    is_moving = false;
    return Widget::on_mouse_release(key);
}

bool check_self(Widget* widget, void* args) {
    if (widget == args)
        return false;
    return true;
}

EVENT_RES Window::on_mouse_move(const mouse_event_t& key) {
    if (is_moving) {
        Point new_pos = {key.x, key.y};
        const Rectangle& area = _parent->active_area();
        new_pos.x = std::max(area.low_x, new_pos.x);
        new_pos.x = std::min(new_pos.x, area.high_x - _size.x);
        new_pos.y = std::max(area.low_y, new_pos.y);
        new_pos.y = std::min(new_pos.y, area.high_y - _size.y);

        Vector delta = new_pos - last_pos;
        Widget* tmp_ptr = this;

        recursive_update(&tmp_ptr, update_coords, &delta);
        assert(tmp_ptr == this);

        _root->recalc_regions();

        last_pos = new_pos;
    }

    return Widget::on_mouse_move(key);
}
