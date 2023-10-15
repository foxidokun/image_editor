#include "canvas.h"

void Canvas::render(RenderTarget& target) {
    _permanent.display();
    _tmp.display();
    target.drawRenderTarget(_reg, _pos, _permanent);
    target.drawRenderTarget(_reg, _pos, _tmp);
}

EVENT_RES Canvas::on_mouse_press(const mouse_event_t& key) {
    bool hit_y = key.y >= _pos.y && key.y <= _pos.y + _size.y;
    bool hit_x = key.x >= _pos.x && key.x <= _pos.x + _size.x;
    if (hit_x && hit_y) {
        std::cout << "Canvas hit\n";
        is_drawing = true;
        mouse_event_t key_copy = key;
        key_copy.x -= _pos.x;
        key_copy.y -= _pos.y;
        tool_manager->paint_on_press(_permanent, _tmp, key_copy);
        return EVENT_RES::STOP;
    }
    return EVENT_RES::CONT;
}

EVENT_RES Canvas::on_mouse_release(const mouse_event_t& key) {
    mouse_event_t key_copy = key;
    key_copy.x -= _pos.x;
    key_copy.y -= _pos.y;
    tool_manager->paint_on_release(_permanent, _tmp, key_copy);
    is_drawing = false;
    return EVENT_RES::CONT;
}

EVENT_RES Canvas::on_mouse_move(const mouse_event_t& key) {
    bool hit_y = key.y >= _pos.y && key.y <= _pos.y + _size.y;
    bool hit_x = key.x >= _pos.x && key.x <= _pos.x + _size.x;
    if (hit_x && hit_y) {
        if (is_drawing) {
            mouse_event_t key_copy = key;
            key_copy.x -= _pos.x;
            key_copy.y -= _pos.y;
            tool_manager->paint_on_move(_permanent, _tmp, key_copy);
        }
        return EVENT_RES::STOP;
    } 

    static uint16_t i = 0;
    std::cout << "MOUSE OUT " << ++i << "\n";

    mouse_event_t key_copy = key;
    key_copy.button = mouse_event_t::button_type::UNKNOWN;
    key_copy.x -= _pos.x;
    key_copy.y -= _pos.y;
    key_copy.x = std::max(key_copy.x, _pos.x);
    key_copy.y = std::max(key_copy.y, _pos.y);
    key_copy.x = std::min(key_copy.x, _pos.x + _size.x);
    key_copy.y = std::min(key_copy.y, _pos.y + _size.y);
    tool_manager->paint_on_release(_permanent, _tmp, key_copy);

    return EVENT_RES::CONT;
}
