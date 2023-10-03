#include "button.h"

EVENT_RES Button::on_mouse_press(const mouse_event_t& key) {
    _callback(_allocated_args);
    return EVENT_RES::STOP;
}

void TextureButton::render(RenderTarget& target, const Point& start_pos) const {
    target.drawTexture(_pos + start_pos, _size, _texture);
}
