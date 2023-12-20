#pragma once

#include "rendertarget.h"
#include "widget.h"
#include "tools.h"
#include "button.h"
#include "filter.h"
#include "window.h"

const char *extract_filename(const char *path);
void window_menu_callback(CallbackArgs*);

class Canvas; // forward declaration

struct CanvasCallbackArgs: public CallbackArgs {
    Canvas* self;

    CanvasCallbackArgs(Canvas *self): self(self) {}
};

class Canvas: public Widget {
private:
    RenderTarget _permanent;
    RenderTarget _tmp;
    ToolManager* tool_manager;
    FilterManager &filter_manager;
    bool is_drawing;
    Menu& window_menu_;
    TextButton *menu_button_;

    Vector real_pos_;
    Vector real_size_;

    template<Orientation orientation>
    friend class ScrollController;

public:
    Canvas(const Point& pos, const Vector& size, ToolManager *tool_manager, FilterManager &filter_manager, Menu& window_menu,
                std::optional<Point>  real_pos  = std::optional<Point>(),
                std::optional<Vector> real_size = std::optional<Vector>()):
        Widget(pos, size),
        tool_manager(tool_manager),
        filter_manager(filter_manager),
        is_drawing(false),
        window_menu_(window_menu),
        _permanent(RenderTarget(real_size.has_value() ? real_size.value() : size)),
        _tmp(RenderTarget(real_size.has_value() ? real_size.value() : size)),
        real_pos_(real_pos.has_value() ? real_pos.value() : pos),
        real_size_(real_size.has_value() ? real_size.value() : size)
        {
            _permanent.clear(sf::Color::White);
            _tmp.clear(sf::Color::Transparent);
            filter_manager.setRenderTarget(&_permanent);

            menu_button_ = new TextButton(Vector(), Vector(), window_menu_callback, new CanvasCallbackArgs(this), "Unnamed");
            window_menu_.register_object(menu_button_);
        }

    void render(RenderTarget& target) final;
    bool onMousePress(mouse_event_t key) final;
    bool onMouseRelease(mouse_event_t key) final;
    bool onMouseMove(mouse_event_t key) final;

    void notify_register() override { set_parent_title("Unnamed"); }

    void set_parent_title(const char *title) {
        menu_button_->set_text(title);
        char buf[256];
        sprintf(buf, "Canvas [%s]", title);
        auto parent_window = static_cast<Window *>(_parent);
        parent_window->set_title(buf);
    }

    void load_from_file(const char* filepath) {
        set_parent_title(extract_filename(filepath));
        _permanent.loadFromFile(filepath);
        _tmp.clear(sf::Color::Transparent);
    };

    void move(const Vector& shift) override;

    void save_to_file(const char* filepath) {
        set_parent_title(extract_filename(filepath));
        _permanent.saveToFile(filepath);
    };
};

void load_canvas_callback(CallbackArgs *args);
void save_canvas_callback(CallbackArgs *args);