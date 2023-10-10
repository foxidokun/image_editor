#include "canvas.h"

void Canvas::render(RenderTarget& target) const {
    target.drawTexture(_reg, _pos, _size, _data.getTexture());
}

EVENT_RES Canvas::on_mouse_press(const mouse_event_t& key) {
    bool hit_y = key.y >= _pos.y && key.y <= _pos.y + _size.y;
    bool hit_x = key.x >= _pos.x && key.x <= _pos.x + _size.x;
    if (hit_x && hit_y) {
        is_drawing = true;
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
        tool_manager->paint(_data, Point(key.x - _pos.x, key.y - _pos.y));
        _data.display();
    }
    return EVENT_RES::STOP;
}
