#include "canvas.h"
#include <nfd.h>

void Canvas::render(RenderTarget& target) {
    _permanent.display();
    _tmp.display();
    target.drawRenderTarget(_reg, real_pos_, _permanent);
    target.drawRenderTarget(_reg, real_pos_, _tmp);
}

bool Canvas::onMousePress(mouse_event_t key) {
    bool hit_y = key.position.y >= _pos.y && key.position.y <= _pos.y + _size.y;
    bool hit_x = key.position.x >= _pos.x && key.position.x <= _pos.x + _size.x;
    if (hit_x && hit_y) {
        is_drawing = true;
        filter_manager.set_rt(&_permanent);
        mouse_event_t key_copy = key;
        key_copy.position.x -= real_pos_.x;
        key_copy.position.y -= real_pos_.y;
        tool_manager->paint_on_press(_permanent, _tmp, key_copy);
        return EVENT_RES::STOP;
    }
    return EVENT_RES::CONT;
}

bool Canvas::onMouseRelease(mouse_event_t key) {
    mouse_event_t key_copy = key;
    key_copy.position.x -= real_pos_.x;
    key_copy.position.y -= real_pos_.y;
    tool_manager->paint_on_release(_permanent, _tmp, key_copy);
    is_drawing = false;
    return EVENT_RES::CONT;
}

bool Canvas::onMouseMove(mouse_event_t key) {
    bool hit_y = key.position.y >= _pos.y && key.position.y <= _pos.y + _size.y;
    bool hit_x = key.position.x >= _pos.x && key.position.x <= _pos.x + _size.x;
    if (hit_x && hit_y) {
        if (is_drawing) {
            mouse_event_t key_copy = key;
            key_copy.position.x -= real_pos_.x;
            key_copy.position.y -= real_pos_.y;
            tool_manager->paint_on_move(_permanent, _tmp, key_copy);
        }
        return EVENT_RES::STOP;
    } 

    mouse_event_t key_copy = key;
    key_copy.position.x -= real_pos_.x;
    key_copy.position.y -= real_pos_.y;
    key_copy.position.x = std::max(key_copy.position.x, _pos.x);
    key_copy.position.y = std::max(key_copy.position.y, _pos.y);
    key_copy.position.x = std::min(key_copy.position.x, _pos.x + _size.x);
    key_copy.position.y = std::min(key_copy.position.y, _pos.y + _size.y);
    tool_manager->paint_on_release(_permanent, _tmp, key_copy);

    return EVENT_RES::CONT;
}

void Canvas::move(const Vector& shift) {
    real_pos_ += shift;
    Widget::move(shift);
}

void load_canvas_callback(CallbackArgs *args_) {
    SaveLoadCanvasArgs *args = static_cast<SaveLoadCanvasArgs *>(args_);

    nfdchar_t *path = NULL;
    nfdresult_t result = NFD_OpenDialog("png;jpg;bmp", NULL, &path);
    
    if (result == NFD_OKAY) {
        args->self->load_from_file(path);
        free(path);
    } else if (result == NFD_CANCEL) {
        return;
    } else {
        std::cerr << "Error: " << NFD_GetError() << '\n';
    }
}

void save_canvas_callback(CallbackArgs *args_) {
    SaveLoadCanvasArgs *args = static_cast<SaveLoadCanvasArgs *>(args_);

    nfdchar_t *path = NULL;
    nfdresult_t result = NFD_SaveDialog("png;jpg;bmp", NULL, &path);
    
    if (result == NFD_OKAY) {
        args->self->save_to_file(path);
        free(path);
    } else if (result == NFD_CANCEL) {
        return;
    } else {
        std::cerr << "Error: " << NFD_GetError() << '\n';
    }
}
