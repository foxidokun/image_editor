#include "button.h"

EVENT_RES Button::on_mouse_press(const mouse_event_t& key) {
    bool hit_x = key.x > _pos.x && key.x < _pos.x + _size.x;
    bool hit_y = key.y > _pos.y && key.y < _pos.y + _size.y;
    
    if (hit_x && hit_y) { 
        _is_clicked = true;
        _callback(_allocated_args);
        return EVENT_RES::STOP;
    } else {
        return EVENT_RES::CONT;
    }
}

EVENT_RES Button::on_mouse_release(const mouse_event_t& key) {
    _is_clicked = false;
    return EVENT_RES::CONT;
}

EVENT_RES Button::on_mouse_move(const mouse_event_t& key) {
    bool hit_x = key.x > _pos.x && key.x < _pos.x + _size.x;
    bool hit_y = key.y > _pos.y && key.y < _pos.y + _size.y;
    if (!(hit_x && hit_y)) {
        _is_clicked = false;
        return EVENT_RES::CONT;
    } else {
        return EVENT_RES::STOP;
    }
}

void TextureButton::render(RenderTarget& target) const {
    target.drawTexture(_reg, _pos, _size, _texture, _is_clicked);
}
