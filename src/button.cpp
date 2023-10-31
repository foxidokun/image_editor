#include "button.h"

EVENT_RES Button::on_mouse_press(const mouse_event_t& key) {
    bool hit_x = key.x > _pos.x && key.x < _pos.x + _size.x;
    bool hit_y = key.y > _pos.y && key.y < _pos.y + _size.y;
    
    if (hit_x && hit_y) { 
        _is_clicked = true;
        _callback(_allocated_args);
        return EVENT_RES::STOP;
    } else {
        return Widget::on_mouse_press(key);
    }
}

EVENT_RES Button::on_mouse_release(const mouse_event_t& key) {
    _is_clicked = false;
    return Widget::on_mouse_release(key);
}

EVENT_RES Button::on_mouse_move(const mouse_event_t& key) {
    bool hit_x = key.x > _pos.x && key.x < _pos.x + _size.x;
    bool hit_y = key.y > _pos.y && key.y < _pos.y + _size.y;
    if (!(hit_x && hit_y)) {
        _is_clicked = false;
        return EVENT_RES::CONT;
    } else {
        return Widget::on_mouse_move(key);
    }
}

void TextureButton::render(RenderTarget& target) {
    target.drawTexture(_reg, _pos, _size, _texture, _is_clicked);
}

void TextButton::render(RenderTarget& target) {
    if (_is_clicked) {
        target.drawText(_reg, _pos, text.c_str(), TITLE_SIZE, BUTTON_ACTIVE_COLOR);
    } else {
        target.drawText(_reg, _pos, text.c_str(), TITLE_SIZE, BUTTON_INACTIVE_COLOR);
    }
}

void Menu::register_object(Widget *widget) {
    widget->set_pos(last_btn_pos);
    last_btn_pos.y += MENU_ITEM_HEIGHT;

    Vector btn_size = widget->size();
    btn_size.x = std::max(_size.x, btn_size.x);
    btn_size.y = MENU_ITEM_HEIGHT;
    widget->set_size(btn_size);
    widget->recalc_regions();

    _active_area = Rectangle(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

    Widget::register_object(widget);
}

void Menu::render(RenderTarget& target) {
    Vector tmp_size = _size;
    _size = default_size;
    TextButton::render(target);
    _size = tmp_size;

    if (is_open) {
        Widget::render(target);
    }
}

Region Menu::get_default_region() const {
    Region new_reg = Widget::get_default_region();

    if (is_open) {
        for (const auto& child: _childs) {
            std::cout << "adding child region = " <<  child->get_default_region() << "  :::   ";
            std::cout << "child size: " << child->size() << '\n';
            new_reg += child->get_default_region();
        }
    }

    return new_reg;
}

void Menu::open() {
    assert(!is_open);

    is_open = true;
    _root->recalc_regions();
}

void Menu::close() {
    is_open = false;
    _root->recalc_regions();
}

EVENT_RES Menu::on_mouse_press(const mouse_event_t& key) {
    bool hit_x = key.x > _pos.x && key.x < _pos.x + default_size.x;
    bool hit_y = key.y > _pos.y && key.y < _pos.y + default_size.y;
    
    if (hit_x && hit_y) { 
        _is_clicked = true;
        _callback(_allocated_args);
        return EVENT_RES::STOP;
    }

    if (is_open) {
        hit_x = key.x > _pos.x && key.x < _pos.x + _size.x;
        hit_y = key.y > _pos.y && key.y < last_btn_pos.y;

        if (hit_x && hit_y) {
            close();
        }

        EVENT_RES res = Widget::on_mouse_press(key);
        if (res == EVENT_RES::STOP) {
            close();
        }

        return res;
    }

    return EVENT_RES::CONT;
}

EVENT_RES Menu::on_mouse_move(const mouse_event_t& key) {
    bool hit_x = key.x > _pos.x && key.x < _pos.x + default_size.x;
    bool hit_y = key.y > _pos.y && key.y < _pos.y + default_size.y;
    if (!(hit_x && hit_y)) {
        _is_clicked = false;
        return EVENT_RES::CONT;
    } else {
        return Widget::on_mouse_move(key);
    }
}

EVENT_RES Menu::on_mouse_release(const mouse_event_t& key) {
    _is_clicked = false;
    return Widget::on_mouse_release(key);
}


struct MenuOpenArgs: CallbackArgs {
    Menu *self;

    MenuOpenArgs(Menu *self): self(self) {};
};

void show_menu_callback(CallbackArgs *_args) {
    MenuOpenArgs *args = static_cast<MenuOpenArgs *>(_args);

    if (args->self->is_open) {
        args->self->close();
    } else {
        args->self->open();
    }
}

void Menu::initialize() {
    _callback       = show_menu_callback; 
    _allocated_args = new MenuOpenArgs(this);
}