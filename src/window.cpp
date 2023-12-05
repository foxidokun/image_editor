#include "window.h"
#include "button.h"
#include <iostream>
#include "config.h"
#include "common.h"

struct WindowButtonArgs: public CallbackArgs {
    Window *window;

    WindowButtonArgs(Window *window_): window(window_) {};
};

static void close_window_callback(CallbackArgs *_args) {
    WindowButtonArgs *args = static_cast<WindowButtonArgs *>(_args);

    args->window->kill();
}

static void apply_window_callback(CallbackArgs *_args) {
    WindowButtonArgs *args = static_cast<WindowButtonArgs *>(_args);
    ParametersModalWindow *window = static_cast<ParametersModalWindow *>(args->window);
    window->apply();
    window->kill();
}

void Window::render(RenderTarget& target) {
    target.drawRect(_reg, _pos, _size, WINDOW_BACKGROUND_COLOR);

    // header sep
    target.drawLine(_reg, _pos + Point(0, HEADER_HEIGHT - LINE_THICKNESS), {_size.x, 0});

    // borders
    target.drawLine(_reg, _pos, {_size.x, 0});
    target.drawLine(_reg, _pos+Vector(0, _size.y-LINE_THICKNESS), {_size.x, 0});
    target.drawLine(_reg, _pos+Vector(LINE_THICKNESS,0), {0, _size.y});
    target.drawLine(_reg, _pos+Vector(_size.x, 0), {0, _size.y});
    
    target.drawText(_reg, _pos + Point(LINE_THICKNESS, LINE_THICKNESS), _title.c_str(), TITLE_SIZE);

    Widget::render(target);

    target.drawRegions(_reg);
}

void Window::initialize() {
    CallbackArgs *args = new WindowButtonArgs(this);
    Point pos = {_size.x - BUTTON_SIZE - LINE_THICKNESS, LINE_THICKNESS};
    Vector size = {BUTTON_SIZE, BUTTON_SIZE};
    TextureButton* button = new TextureButton(pos, size, close_window_callback, args, global_resources::close_button);

    register_object(button);

    _active_area.low_y  += HEADER_HEIGHT;
    _active_area.low_x  += LINE_THICKNESS;
    _active_area.high_x -= LINE_THICKNESS;
    _active_area.high_y -= LINE_THICKNESS;
}

bool Window::onMousePress(mouse_event_t key) {
    bool res = Widget::onMousePress(key);
    if (res == EVENT_RES::CONT) {
        bool hit_y = key.position.y > _pos.y && key.position.y < _pos.y + HEADER_HEIGHT;
        bool hit_x = key.position.x > _pos.x && key.position.x < _pos.x + _size.x;
        if (hit_x && hit_y) {
            is_moving = true;
            last_pos = Point(key.position.x, key.position.y);
            return EVENT_RES::STOP;
        }
    }

    return res;
}

bool Window::onMouseRelease(mouse_event_t key) {
    is_moving = false;
    return Widget::onMouseRelease(key);
}

bool check_self(Widget* widget, void* args) {
    if (widget == args)
        return false;
    return true;
}

bool Window::onMouseMove(mouse_event_t key) {
    if (is_moving) {
        Point new_pos = {key.position.x, key.position.y};
        const Rectangle& area = _parent->active_area();
        new_pos.x = std::max(area.low_x, new_pos.x);
        new_pos.x = std::min(new_pos.x, area.high_x - _size.x);
        new_pos.y = std::max(area.low_y, new_pos.y);
        new_pos.y = std::min(new_pos.y, area.high_y - _size.y);

        Vector delta = new_pos - last_pos;

        recursive_update(this, update_coords, &delta);

        _root->recalc_regions();

        last_pos = new_pos;
    }

    return Widget::onMouseMove(key);
}

void ParametersModalWindow::initialize() {
    for (uint i = 0; i < parameters.size(); ++i) {
        auto box = new TextBox(_pos + Vector(_size.x - 100, 10 + HEADER_HEIGHT + i*20), Vector(90, 18));
        boxes.push_back(box);
        register_object_exact_pos(box);
    }

    register_object_exact_pos(new TextButton(_pos + _size - Vector(70, 50), Vector(50, 30), apply_window_callback,
        new WindowButtonArgs(this), "Apply"));
}

void ParametersModalWindow::render(RenderTarget& rt) {
    Window::render(rt);

    for (uint i = 0; i < parameters.size(); ++i) {
        rt.drawText(_reg, _pos + Vector(10, 10 + HEADER_HEIGHT + i*20), parameters[i], TITLE_SIZE);
    }
}
