#include "canvas.h"
#include <QFileDialog>

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
        filter_manager.setRenderTarget(&_permanent);
        mouse_event_t key_copy = key;
        key_copy.position.x -= real_pos_.x;
        key_copy.position.y -= real_pos_.y;
        tool_manager->paintOnPress(&_permanent, &_tmp, key_copy);
        return EVENT_RES::STOP;
    }
    return EVENT_RES::CONT;
}

bool Canvas::onMouseRelease(mouse_event_t key) {
    mouse_event_t key_copy = key;
    key_copy.position.x -= real_pos_.x;
    key_copy.position.y -= real_pos_.y;
    tool_manager->paintOnRelease(&_permanent, &_tmp, key_copy);
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
            tool_manager->paintOnMove(&_permanent, &_tmp, key_copy);
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
    tool_manager->paintOnRelease(&_permanent, &_tmp, key_copy);

    tool_manager->disableTool(&_permanent, &_tmp, key_copy);

    return EVENT_RES::CONT;
}

void Canvas::move(const Vector& shift) {
    real_pos_ += shift;
    Widget::move(shift);
}

void load_canvas_callback(CallbackArgs *args_) {
    SaveLoadCanvasArgs *args = static_cast<SaveLoadCanvasArgs *>(args_);

    auto result = QFileDialog::getOpenFileName();
    auto path = result.toStdString();


    if (!result.isEmpty() && !result.isNull()) {
        args->self->load_from_file(path.c_str());
    }
}

void save_canvas_callback(CallbackArgs *args_) {
    SaveLoadCanvasArgs *args = static_cast<SaveLoadCanvasArgs *>(args_);

    auto result = QFileDialog::getSaveFileName();
    auto path = result.toStdString();

    if (!result.isEmpty() && !result.isNull()) {
        args->self->save_to_file(path.c_str());
    }
}

const char *extract_filename(const char *path) {
    const char *start = path;

    while (*path) {
        if (*path == '/') {
            start = path + 1;
        }

        path++;
    }

    return start;
}