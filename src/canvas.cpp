#include "canvas.h"

void Canvas::render(RenderTarget& target) const {
    target.drawTexture(_reg, _pos, _size, _data.getTexture());
}

EVENT_RES Canvas::on_mouse_press(const mouse_event_t& key) {
    bool hit_y = key.y >= _pos.y && key.y <= _pos.y + _size.y;
    bool hit_x = key.x >= _pos.x && key.x <= _pos.x + _size.x;
    if (hit_x && hit_y) {
        is_drawing = true;
        last_pos = Point(key.x - _pos.x, key.y - _pos.y);
        return EVENT_RES::STOP;
    }
    return EVENT_RES::CONT;
}

EVENT_RES Canvas::on_mouse_release(const mouse_event_t& key) {
    is_drawing = false;
    return EVENT_RES::CONT;
}

EVENT_RES Canvas::on_mouse_move(const mouse_event_t& key) {
    if (is_drawing) {
        Point target = Point(key.x - _pos.x, key.y - _pos.y);
        Vector step = (target - last_pos);
        double step_length = step.length();
        double num_of_steps = step_length / BRUSH_RADIUS * 5;
        num_of_steps = std::max(num_of_steps, 1.0);
        step = step / num_of_steps;
        std::cout << num_of_steps << "\n";
        for (uint i = 0; i < (uint)num_of_steps; ++i) {
            tool_manager->paint(_data, last_pos + step*i);
        }
        last_pos = target;
        _data.display();
    }
    return EVENT_RES::STOP;
}
