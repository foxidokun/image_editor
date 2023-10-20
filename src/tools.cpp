#include "tools.h"

static inline Point extract_point(const mouse_event_t& key) {
    return Point(key.x, key.y);
}

void Brush::paint(RenderTarget& permanent, const Point& point_pos, const Color& color) const {
    permanent.drawCircle(point_pos, BRUSH_RADIUS, color);
}

void Brush::paint_on_press(RenderTarget& permanent, RenderTarget& tmp, const mouse_event_t& key, const Color& color) {
    tmp.clear(sf::Color::Transparent);
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

void AlienBrush::paint_on_press(RenderTarget& permanent, RenderTarget& tmp, const mouse_event_t& key, const Color& color) {
    tmp.clear(sf::Color::Transparent);
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
    tmp.clear(sf::Color::Transparent);
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
    tmp.clear(sf::Color::Transparent);
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
    tmp.clear(sf::Color::Transparent);
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

void ColorPicker::paint_on_press(RenderTarget& permanent, RenderTarget& tmp, const mouse_event_t& point_pos, const Color& color) {
    tmp.clear(sf::Color::Transparent);
    is_drawing = true;
    set_color(permanent, point_pos);
}

void ColorPicker::paint_on_move(RenderTarget& permanent, RenderTarget& tmp, const mouse_event_t& point_pos, const Color& color) {
    if (is_drawing) {
        set_color(permanent, point_pos);
    }
}

void ColorPicker::paint_on_release(RenderTarget& permanent, RenderTarget& tmp, const mouse_event_t& point_pos, const Color& color) {
    is_drawing = false;
}

void ColorPicker::set_color(RenderTarget& permanent, const mouse_event_t& point_pos) {
    Point pos = extract_point(point_pos);
    Color color = permanent.get_image().get_pixel(pos.x, pos.y);
    tm->set_color(color);
}

void FillTool::paint_on_press(RenderTarget& permanent, RenderTarget& tmp, const mouse_event_t& point_pos, const Color& color) {
    tmp.clear(sf::Color::Transparent);

    Point pos = extract_point(point_pos);
    RawImage img = permanent.get_image();
    Color ref_color = img.get_pixel(pos.x, pos.y);
    
    linked_list<Point> queue;
    queue.push_back(pos);
    while (queue.size() > 0) {
        Point cur_pos = queue.front();
        queue.pop_front();

        if (cur_pos.x < 0 || cur_pos.x + 1 > img.width()) {
            continue;
        }

        if (cur_pos.y < 0 || cur_pos.y + 1> img.height()) {
            continue;
        }

        if (img.get_pixel(cur_pos.x, cur_pos.y) == ref_color) {
            img.set_pixel(cur_pos.x, cur_pos.y, color);
            Vector diff = cur_pos - pos;
            double diff_length = diff.length_square();
            for (int i = -1; i <= 1; ++i) {
                for (int j = -1; j <= 1; ++j) {
                    if ((diff+Vector(i, j)).length_square() > diff_length) {
                        queue.push_back(cur_pos + Vector(i, j));
                    }
                }
            }
        }
    }

    permanent.set_image(img);
}


void SplineTool::paint_on_press(RenderTarget& permanent, RenderTarget& tmp, const mouse_event_t& point_pos, const Color& color) {
    if (cnt == 0) { tmp.clear(sf::Color::Transparent); }

    cnt++;
}

void SplineTool::paint_on_move(RenderTarget& permanent, RenderTarget& tmp, const mouse_event_t& point_pos, const Color& color) {
    // BE ME, DO NOTHING
}

void SplineTool::paint_on_release(RenderTarget& permanent, RenderTarget& tmp, const mouse_event_t& point_pos, const Color& color) {
    if (cnt <= 0) { return; }
    if (point_pos.button == mouse_event_t::button_type::UNKNOWN) { return; }

    points[cnt - 1] = extract_point(point_pos);
    if (cnt < 4) { return; }

    for (int i = 0; i < 3; ++i) {
        permanent.drawLine(points[i], points[i+1] - points[i], color);
    }

    cnt = 0;
}
