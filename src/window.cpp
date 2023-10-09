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

    target.drawLine(_reg, _pos + Point(0, HEADER_HEIGHT), {_size.x, 0});
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