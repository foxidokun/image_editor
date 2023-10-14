#include "tools.h"

static inline Point extract_point(const mouse_event_t& key) {
    return Point(key.x, key.y);
}

void Brush::paint(RenderTarget& permanent, const Point& point_pos, const Color& color) const {
    permanent.drawCircle(point_pos, _radius, color);
}

void Brush::paint_on_press(RenderTarget& permanent, RenderTarget&, const mouse_event_t& key, const Color& color) {
    last_pos = extract_point(key);
    paint(permanent, last_pos, color);
}

void Brush::paint_on_move(RenderTarget& permanent, RenderTarget& tmp, const mouse_event_t& key, const Color& color) {
    Point point_pos = extract_point(key);

    Vector step = (point_pos - last_pos);
    double step_length = step.length();
    double num_of_steps = step_length / _radius * 5;
    num_of_steps = std::max(num_of_steps, 1.0);
    step = step / num_of_steps;

    for (uint i = 0; i < (uint)num_of_steps; ++i) {
        Point draw_point = last_pos + step * i;
        paint(permanent, draw_point, color);
    }

    last_pos = point_pos;
}

void AlienBrush::paint(RenderTarget& permanent, const Point& point_pos, const Color& color) const {
    permanent.drawCircle(point_pos, _radius, color);
}

void AlienBrush::paint_on_press(RenderTarget& permanent, RenderTarget&, const mouse_event_t& key, const Color& color) {
    start_pos = extract_point(key);
    paint(permanent, start_pos, color);
}

void AlienBrush::paint_on_move(RenderTarget& permanent, RenderTarget& tmp, const mouse_event_t& key, const Color& color) {
    Point point_pos = extract_point(key);

    Vector dist = (point_pos - start_pos);
    double dist_length = dist.length();

    uint num_of_steps = (uint) floor(dist_length /(_radius * 3)) + 1;

    if (num_of_steps == 1) {
        return;
    }

    Vector step = dist.norm() * (3 * _radius);

    for (uint i = 0; i < num_of_steps; ++i) {
        Point draw_point = start_pos + step * i;
        paint(permanent, draw_point, color);
    }
}