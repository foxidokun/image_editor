#pragma once
#include "widget.h"
#include "string.h"

class Window: public Widget {
private:
    string _title;

    void initialise();

public:
    Window(Widget *parent, const Point& pos, const Vector& size, const string& title):
        Widget(parent, pos, size),
        _title(title) {
            initialise();
        }

    Window(Widget *parent, const Point& pos, const Vector& size, string&& title):
        Widget(parent, pos, size),
        _title(title) {
            initialise();
        }

    void render(RenderTarget& target, const Point& start_pos) const final;
};