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

void Window::render(RenderTarget& target) const {
    target.drawRect(_reg, _pos, _size, WINDOW_COLOR);

    // header sep
    target.drawLine(_reg, _pos + Point(0, HEADER_HEIGHT), {_size.x, 0});

    // borders
    target.drawLine(_reg, _pos,                    {_size.x, 0});
    target.drawLine(_reg, _pos+Vector(0, _size.y), {_size.x, 0});
    target.drawLine(_reg, _pos,                    {0, _size.y});
    target.drawLine(_reg, _pos+Vector(_size.x, 0), {0, _size.y});
    
    target.drawText(_reg, _pos +  Point(LINE_THICKNESS, LINE_THICKNESS), _title.c_str(), TITLE_SIZE);

    Widget::render(target);
}

void Window::initialise() {
    CallbackArgs *args = new WindowButtonArgs(this);
    Point pos = {_size.x - BUTTON_SIZE - LINE_THICKNESS, LINE_THICKNESS};
    Vector size = {BUTTON_SIZE, BUTTON_SIZE};
    TextureButton* button = new TextureButton(pos, size, close_window_callback, args, close_window_button);

    register_object(button);
}

EVENT_RES Window::on_mouse_press(const mouse_event_t& key) {
    EVENT_RES res = Widget::on_mouse_press(key);
    if (res == EVENT_RES::CONT) {
        bool hit_y = key.y > _pos.y && key.y < _pos.y + HEADER_HEIGHT;
        bool hit_x = key.x > _pos.x && key.x < _pos.x + _size.x;
        if (hit_x && hit_y) {
            is_moving = true;
            return EVENT_RES::STOP;
        }
    }

    return res;
}

EVENT_RES Window::on_mouse_release(const mouse_event_t& key) {
    is_moving = false;
    return Widget::on_mouse_release(key);
}

EVENT_RES Window::on_mouse_move(const mouse_event_t& key) {
    if (is_moving) {
        Point new_pos = {key.x, key.y};
        bool valid_x = new_pos.x > _parent->pos().x && (new_pos.x + _size.x) < (_parent->pos().x + _parent->size().x);
        bool valid_y = new_pos.y > _parent->pos().y && (new_pos.y + _size.y) < (_parent->pos().y + _parent->size().y);
        if (valid_x && valid_y) {
            Vector delta = new_pos - _pos;
            Widget* tmp_ptr= this;
            recursive_update(&tmp_ptr, update_coords, &delta);
            assert(tmp_ptr == this);
        } else {
            is_moving = false;
        }
    }

    return Widget::on_mouse_move(key);
}
