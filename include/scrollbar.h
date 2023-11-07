#pragma once
#include "button.h"
#include "canvas.h"

// ---------------------------------------------------------------------------------------------------------------------

template<Orientation orientation>
class ScrollController;

template<class T>
concept IsVec2 = requires(T val) {
    val.x;
    val.y;
    requires std::integral<typeof(val.x)> || std::floating_point<typeof(val.x)>;
    requires std::integral<typeof(val.y)> || std::floating_point<typeof(val.y)>;
    requires std::same_as<typeof(val.x), typeof(val.y)>;
};

template<Orientation orientation, IsVec2 T>
auto _coord(T &vec) -> std::conditional_t<std::is_const_v<T>, const decltype(vec.x)&, decltype(vec.x)&> {
    if constexpr (orientation == Orientation::Horizontal) {
        return vec.x;
    } else if constexpr (orientation == Orientation::Vertical) {
        return vec.y;
    } else {
        static_assert(false, "unexpected enum member");
    }
}

// ---------------------------------------------------------------------------------------------------------------------

template<Orientation orientation>
class Scrollbar: public Widget {
private:
    friend void on_scrollbar_click(CallbackArgs* args);
    double _percentage = 0.5;
    double _vis_percentage = 1.0;
    bool _pressed = false;
    ScrollController<orientation>* _controller = nullptr;

    void set_percentage(const mouse_event_t& key);

    template<IsVec2 T>
    auto& coord(T& vec) { return _coord<orientation, T>(vec); }

public:
    Scrollbar(const Point& pos, const Vector& size): Widget(pos, size) {}

    virtual EVENT_RES on_mouse_press(const mouse_event_t& key) override;
    virtual EVENT_RES on_mouse_move(const mouse_event_t& key) override;
    virtual EVENT_RES on_mouse_release(const mouse_event_t& key) override;
    virtual void render(RenderTarget& target) override;

    double percentage() const noexcept { return _percentage; }
    void percentage(double val) noexcept { _percentage = val; }

    double vis_percentage() const noexcept { return _vis_percentage; }
    void vis_percentage(double val) noexcept { _vis_percentage = val; }

    void controller(ScrollController<orientation>* val) { _controller = val; }
};

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

template<Orientation orientation>
class ScrollController {
private:
    Scrollbar<orientation>& scrollbar_;
    Canvas& canvas_;

    template<IsVec2 T>
    static auto& coord(T& vec) { return _coord<orientation, T>(vec); }

public:
    ScrollController(Scrollbar<orientation>& bar, Canvas& canvas): scrollbar_(bar), canvas_(canvas) {
        bar.controller(this);
        bar.vis_percentage(coord(canvas._size) / coord(canvas.real_size_));
        notify();
    }

    void notify();
};

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

template<Orientation orientation>
void Scrollbar<orientation>::set_percentage(const mouse_event_t& key) {
    _percentage = (coord(key) - coord(_pos)) / coord(_size);
    _percentage = std::max(_percentage, _vis_percentage / 2);
    _percentage = std::min(_percentage, 1 - _vis_percentage / 2);

    if (_controller) {
        _controller->notify();
    }
}

// ---------------------------------------------------------------------------------------------------------------------

template<Orientation orientation>
inline EVENT_RES Scrollbar<orientation>::on_mouse_press(const mouse_event_t& key) {
    bool hit_x = key.x > _pos.x && key.x < _pos.x + _size.x;
    bool hit_y = key.y > _pos.y && key.y < _pos.y + _size.y;

    if (hit_x && hit_y) {
        set_percentage(key);
        _pressed = true;
        return EVENT_RES::STOP;
    } else {
        _pressed = false;
        return EVENT_RES::CONT;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

template<Orientation orientation>
inline EVENT_RES Scrollbar<orientation>::on_mouse_release(const mouse_event_t& key) {
    bool hit_x = key.x > _pos.x && key.x < _pos.x + _size.x;
    bool hit_y = key.y > _pos.y && key.y < _pos.y + _size.y;

    _pressed = false;

    if (hit_x && hit_y) {
        set_percentage(key);

        return EVENT_RES::STOP;
    } else {
        return EVENT_RES::CONT;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

template<Orientation orientation>
inline EVENT_RES Scrollbar<orientation>::on_mouse_move(const mouse_event_t& key) {
    bool hit_x = key.x > _pos.x && key.x < _pos.x + _size.x;
    bool hit_y = key.y > _pos.y && key.y < _pos.y + _size.y;

    if (hit_x && hit_y) {
        if (_pressed) {
            set_percentage(key);
        }
        return EVENT_RES::STOP;
    } else {
        return EVENT_RES::CONT;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

template<Orientation orientation>
inline void Scrollbar<orientation>::render(RenderTarget& target) {
    target.drawRect(_reg, _pos, _size, SCROLLBAR_BACKGROUND_COLOR);

    Vector offset(0, 0);
    coord(offset) = coord(_size) * (_percentage - _vis_percentage / 2);

    Vector ruler_size = _size;
    coord(ruler_size) = coord(_size) * _vis_percentage;

    target.drawRect(_reg, _pos + offset, ruler_size, SCROLLBAR_RULER_COLOR);
}

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

template<Orientation orientation>
void ScrollController<orientation>::notify() {
    double val = scrollbar_.percentage();

    coord(canvas_.real_pos_) = coord(canvas_._pos) + coord(canvas_._size) / 2 - val * coord(canvas_.real_size_);
}