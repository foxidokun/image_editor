#pragma once
#include "widget.h"
#include "string.h"


// Abstract class for button callback arguments
class CallbackArgs {
public:
    virtual ~CallbackArgs() = default;
};

using on_click_f = void(*)(CallbackArgs*);

class Button: public Widget {
private:
    on_click_f _callback;
    CallbackArgs *_allocated_args;
public:
    Button(const Point& pos, const Vector& size, on_click_f callback, CallbackArgs *allocated_args):
        Widget(pos, size),
        _callback(callback),
        _allocated_args(allocated_args) {}

    EVENT_RES on_mouse_press(const mouse_event_t& key) final;

    ~Button() {
        delete _allocated_args;
    }
};

class TextureButton: public Button {
private:
    Texture _texture;
public:
    TextureButton(const Point& pos, const Vector& size, on_click_f callback, CallbackArgs *allocated_args,
        const sf::Texture& texture):
        Button(pos, size, callback, allocated_args),
        _texture(texture) {}

    void render(RenderTarget& target, const Point& start_pos) const final;
};