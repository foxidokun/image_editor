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
        winmgr.set_last_canvas(this);
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

void window_menu_callback(CallbackArgs* args_) {
    CanvasCallbackArgs *args = static_cast<CanvasCallbackArgs *>(args_);
    args->self->prioritize_itself();
}

void load_canvas_callback(CallbackArgs *args_) {
    CanvasOpenCallbackArgs *args = static_cast<CanvasOpenCallbackArgs *>(args_);

    auto result = QFileDialog::getOpenFileName();
    auto path = result.toStdString();

    if (!result.isEmpty() && !result.isNull()) {
        auto win      = new Window(Point(110,0), Vector(800, 600), "Canvas");
        double width  = win->active_area().high_x - win->active_area().low_x;
        double height = win->active_area().high_y - win->active_area().low_y;

        auto canvas = new Canvas(Point(0,0), Vector(width, height),  &args->toolmgr, args->filtmgr, args->winmgr, args->menu);
        win->register_object(canvas);
        args->winmgr.register_object(win);

        canvas->load_from_file(path.c_str());
    }
}

void save_canvas_callback(CallbackArgs *args_) {
    CanvasOpenCallbackArgs *args = static_cast<CanvasOpenCallbackArgs *>(args_);

    auto result = QFileDialog::getSaveFileName();
    auto path = result.toStdString();

    if (!result.isEmpty() && !result.isNull()) {
        args->winmgr.get_last_canvas()->save_to_file(path.c_str());
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