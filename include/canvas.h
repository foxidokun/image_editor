#pragma once

#include "rendertarget.h"
#include "widget.h"
#include "tools.h"
#include "button.h"
#include "filter.h"

class Canvas: public Widget {
private:
    RenderTarget _permanent;
    RenderTarget _tmp;
    ToolManager* tool_manager;
    FilterManager &filter_manager;
    bool is_drawing;

    Vector real_pos_;
    Vector real_size_;

    template<Orientation orientation>
    friend class ScrollController;

public:
    Canvas(const Point& pos, const Vector& size, ToolManager *tool_manager, FilterManager &filter_manager,
                std::optional<Point>  real_pos  = std::optional<Point>(),
                std::optional<Vector> real_size = std::optional<Vector>()):
        Widget(pos, size),
        tool_manager(tool_manager),
        filter_manager(filter_manager),
        is_drawing(false),
        _permanent(RenderTarget(real_size.has_value() ? real_size.value() : size)),
        _tmp(RenderTarget(real_size.has_value() ? real_size.value() : size)),
        real_pos_(real_pos.has_value() ? real_pos.value() : pos),
        real_size_(real_size.has_value() ? real_size.value() : size)
        {
            _permanent.clear(sf::Color::White);
            _tmp.clear(sf::Color::Transparent);
            filter_manager.setRenderTarget(&_permanent);
        }

    void render(RenderTarget& target) final;
    bool onMousePress(mouse_event_t key) final;
    bool onMouseRelease(mouse_event_t key) final;
    bool onMouseMove(mouse_event_t key) final;

    void load_from_file(const char* filename) {
        _permanent.loadFromFile(filename);
        _tmp.clear(sf::Color::Transparent);
    };

    void move(const Vector& shift) override;

    void save_to_file(const char* filename) {
        _permanent.saveToFile(filename);
    };
};

struct SaveLoadCanvasArgs: public CallbackArgs {
    Canvas* self;

    SaveLoadCanvasArgs(Canvas *self): self(self) {}
};

void load_canvas_callback(CallbackArgs *args);
void save_canvas_callback(CallbackArgs *args);