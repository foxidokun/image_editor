#pragma once
#include "widget.h"
#include "string.h"

class Window: public Widget {
public: // TODO: revert to private
    string _title;

    void initialise();

public:
    Window(const Point& pos, const Vector& size, const string& title):
        Widget(pos, size),
        _title(title) {
            initialise();
        }

    Window(const Point& pos, const Vector& size, string&& title):
        Widget(pos, size),
        _title(title) {
            initialise();
        }

    void render(RenderTarget& target, const Point& start_pos) const final;
};