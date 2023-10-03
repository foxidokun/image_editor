#pragma once
#include "widget.h"
#include "string.h"

class Window: public Widget {
private:
    string _title;

public:
    Window(const Point& pos, const Vector& size, const string& title):
        Widget(pos, size),
        _title(title) {}

    Window(const Point& pos, const Vector& size, string&& title):
        Widget(pos, size),
        _title(title) {}

    void render(RenderTarget& target, const Point& start_pos) const final;
};