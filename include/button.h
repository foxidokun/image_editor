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
protected:
    on_click_f _callback;
    CallbackArgs *_allocated_args;
    bool _is_clicked;
public:
    Button(const Point& pos, const Vector& size, on_click_f callback, CallbackArgs *allocated_args):
        Widget(pos, size),
        _callback(callback),
        _allocated_args(allocated_args),
        _is_clicked(false) {}

    EVENT_RES on_mouse_press  (const mouse_event_t& key) override;
    EVENT_RES on_mouse_move   (const mouse_event_t& key) override;
    EVENT_RES on_mouse_release(const mouse_event_t& key) override;

    ~Button() {
        delete _allocated_args;
    }
};

class TextureButton: public Button {
protected:
    Texture _texture;
public:
    TextureButton(const Point& pos, const Vector& size, on_click_f callback,
        CallbackArgs *allocated_args, const sf::Texture& texture):
        Button(pos, size, callback, allocated_args),
        _texture(texture) {}

    void render(RenderTarget& target) override;

    void print(std::ostream& stream) const final {
        stream << "TextureButton";
    }
};

class Menu: public TextureButton {
private:
    bool is_open = false;
    Point last_btn_pos;
    Vector default_size;

    void initialize();

public:
    Menu(const Point& pos, const Vector& size, const sf::Texture& texture): 
        TextureButton(pos, size, nullptr, nullptr, texture),
        last_btn_pos(Vector(0, _size.y)),
        default_size(size)
        {
            std::cout << "size = " << _size << " pos =  " << _pos  << " reg = " << _reg << "\n";
            initialize();
        }

    void register_object(Widget *); // override non-virtual fuction
    void render(RenderTarget& target) final;

    void open();
    void close();

    EVENT_RES on_mouse_press  (const mouse_event_t& key) final;
    EVENT_RES on_mouse_move   (const mouse_event_t& key) final;
    EVENT_RES on_mouse_release(const mouse_event_t& key) final;

    friend void show_menu_callback(CallbackArgs *);
};