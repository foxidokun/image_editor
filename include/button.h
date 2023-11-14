#pragma once
#include "widget.h"
#include "string.h"


// Abstract class for button callback arguments
class CallbackArgs {
public:
    virtual ~CallbackArgs() = default;
};

using callback_f = void(*)(CallbackArgs*);

class Button: public Widget {
protected:
    callback_f _callback;
    CallbackArgs *_allocated_args;
    bool _is_clicked;
public:
    Button(const Point& pos, const Vector& size, callback_f callback, CallbackArgs *allocated_args):
        Widget(pos, size),
        _callback(callback),
        _allocated_args(allocated_args),
        _is_clicked(false) {}

    bool onMousePress  (mouse_event_t key) override;
    bool onMouseMove   (mouse_event_t key) override;
    bool onMouseRelease(mouse_event_t key) override;

    ~Button() {
        delete _allocated_args;
    }
};

class TextureButton: public Button {
protected:
    Texture _texture;
public:
    TextureButton(const Point& pos, const Vector& size, callback_f callback,
        CallbackArgs *allocated_args, const sf::Texture& texture):
        Button(pos, size, callback, allocated_args),
        _texture(texture) {}

    void render(RenderTarget& target) override;

    void print(std::ostream& stream) const final {
        stream << "TextureButton";
    }
};

class TextButton: public Button {
private:
    string text;
public:
    TextButton(const Point& pos, const Vector& size, callback_f callback,
        CallbackArgs *allocated_args, const string& text):
        Button(pos, size, callback, allocated_args),
        text(text)
        {}

    void render(RenderTarget& target) override;

    void print(std::ostream& stream) const override {
        stream << "TextButton";
    }
};


class Menu: public TextButton {
private:
    bool is_open = false;
    Point last_btn_pos;
    Vector default_size;
    double max_width;

    void initialize();

public:
    Menu(const Point& pos, const Vector& size, const string& text): 
        TextButton(pos, size, nullptr, nullptr, text),
        last_btn_pos(Vector(_pos.x, _size.y)),
        default_size(size),
        max_width(size.x)
        {
            initialize();
        }

    void register_object(Widget *); // override non-virtual fuction
    void render(RenderTarget& target) final;

    Region get_default_region() const override;

    void open();
    void close();

    bool onMousePress  (mouse_event_t key) final;
    bool onMouseMove   (mouse_event_t key) final;
    bool onMouseRelease(mouse_event_t key) final;

    void print(std::ostream& stream) const override {
        stream << "Menu";
    }

    friend void show_menu_callback(CallbackArgs *);
};