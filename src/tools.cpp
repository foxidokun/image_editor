#include "tools.h"

static inline Point extract_point(const mouse_event_t& key) {
    return Point(key.x, key.y);
}

void Brush::paint(RenderTarget& permanent, const Point& point_pos, const Color& color) const {
    permanent.drawCircle(point_pos, BRUSH_RADIUS, color);
}

void Brush::paint_on_press(RenderTarget& permanent, RenderTarget&, const mouse_event_t& key, const Color& color) {
    last_pos = extract_point(key);
    paint(permanent, last_pos, color);
}

void Brush::paint_on_move(RenderTarget& permanent, RenderTarget& tmp, const mouse_event_t& key, const Color& color) {
    Point point_pos = extract_point(key);

    Vector step = (point_pos - last_pos);
    double step_length = step.length();
    double num_of_steps = step_length / BRUSH_RADIUS * 5;
    num_of_steps = std::max(num_of_steps, 1.0);
    step = step / num_of_steps;

    for (uint i = 0; i < (uint)num_of_steps; ++i) {
        Point draw_point = last_pos + step * i;
        paint(permanent, draw_point, color);
    }

    last_pos = point_pos;
}

void AlienBrush::paint(RenderTarget& permanent, const Point& point_pos, const Color& color) const {
    permanent.drawCircle(point_pos, BRUSH_RADIUS, color);
}

void AlienBrush::paint_on_press(RenderTarget& permanent, RenderTarget&, const mouse_event_t& key, const Color& color) {
    start_pos = extract_point(key);
    paint(permanent, start_pos, color);
}

void AlienBrush::paint_on_move(RenderTarget& permanent, RenderTarget& tmp, const mouse_event_t& key, const Color& color) {
    Point point_pos = extract_point(key);

    Vector dist = (point_pos - start_pos);
    double dist_length = dist.length();

    uint num_of_steps = (uint) floor(dist_length / (BRUSH_RADIUS * 3)) + 1;

    if (num_of_steps == 1) {
        return;
    }

    Vector step = dist.norm() * (3 * BRUSH_RADIUS);

    for (uint i = 0; i < num_of_steps; ++i) {
        Point draw_point = start_pos + step * i;
        paint(permanent, draw_point, color);
    }
}

void Polyline::paint_on_press(RenderTarget& permanent, RenderTarget& tmp, const mouse_event_t& point_pos, const Color& color) {
    if (!has_drawn) {
        last_point = extract_point(point_pos);
        has_drawn = true;
    }
}

void Polyline::paint_on_move(RenderTarget& permanent, RenderTarget& tmp, const mouse_event_t& point_pos, const Color& color) {
    if (has_drawn) {
        tmp.clear(sf::Color::Transparent);
        Point new_point = extract_point(point_pos);
        tmp.drawLine(last_point, new_point - last_point, color);
        unclear_tmp = true;
    }
}

void Polyline::paint_on_release(RenderTarget& permanent, RenderTarget& tmp, const mouse_event_t& point_pos, const Color& color) {
    if (unclear_tmp) {
        tmp.clear(sf::Color::Transparent);
    }

    if (point_pos.button == mouse_event_t::button_type::UNKNOWN) {
        has_drawn = false;
        return;
    }

    if (has_drawn) {
        Point new_point = extract_point(point_pos);
        permanent.drawLine(last_point, new_point - last_point, color);
        last_point = new_point;

        if (point_pos.button == mouse_event_t::button_type::RIGHT)
        {
            has_drawn = false;
        }
    }
}

void RectTool::paint_on_press(RenderTarget& permanent, RenderTarget& tmp, const mouse_event_t& point_pos, const Color& color) {
    start_point = extract_point(point_pos);
    is_drawing = true;
}

void RectTool::paint_on_move(RenderTarget& permanent, RenderTarget& tmp, const mouse_event_t& point_pos, const Color& color) {
    if (is_drawing) {
        tmp.clear(sf::Color::Transparent);

        Point end_point = extract_point(point_pos);
        tmp.drawRect(start_point, end_point-start_point, color);
    }
}

void RectTool::paint_on_release(RenderTarget& permanent, RenderTarget& tmp, const mouse_event_t& point_pos, const Color& color) {
    if (is_drawing) {
        tmp.clear(sf::Color::Transparent);

        Point end_point = extract_point(point_pos);
        permanent.drawRect(start_point, end_point-start_point, color);
        is_drawing = false;
    }
}


void EllipseTool::paint_on_press(RenderTarget& permanent, RenderTarget& tmp, const mouse_event_t& point_pos, const Color& color) {
    start_point = extract_point(point_pos);
    is_drawing = true;
}

void EllipseTool::paint_on_move(RenderTarget& permanent, RenderTarget& tmp, const mouse_event_t& point_pos, const Color& color) {
    if (is_drawing) {
        tmp.clear(sf::Color::Transparent);

        Point end_point = extract_point(point_pos);
        tmp.drawEllipse(start_point, end_point-start_point, color);
    }
}

void EllipseTool::paint_on_release(RenderTarget& permanent, RenderTarget& tmp, const mouse_event_t& point_pos, const Color& color) {
    if (is_drawing) {
        tmp.clear(sf::Color::Transparent);

        Point end_point = extract_point(point_pos);
        permanent.drawEllipse(start_point, end_point-start_point, color);
        is_drawing = false;
    }
}
