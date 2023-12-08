#include "widget.h"
#include "window.h"
#include <type_traits>
#include <cassert>

// ---------------------------------------------------------------------------------------------------------------------

static inline bool no_hit(const Point& pos, const Vector& size, const mouse_event_t& event);
void set_root(Widget * widget, void *args);

template<typename T>
using handler_func_t = bool (Widget::*)(T event);

// ---------------------------------------------------------------------------------------------------------------------

template<typename T, bool reorder = false>
static bool default_event_handler(Widget * root, linked_list<Widget *>& childs, handler_func_t<T> handler_func,
    const T& event)
{
    for (auto child_iter = childs.begin(); child_iter != childs.end();) {
        if (((*child_iter)->*handler_func)(event) == EVENT_RES::STOP) {
            if constexpr (reorder) {
                childs.push_front(*child_iter);
                childs.erase(child_iter);
                root->recalc_regions();
            }
            return EVENT_RES::STOP;
        }

        ++child_iter;
    }

    return EVENT_RES::CONT;
}

// ---------------------------------------------------------------------------------------------------------------------

bool Widget::onKeyboardPress(keyboard_event_t key) {
    return default_event_handler<keyboard_event_t>(_root, _childs, &Widget::onKeyboardPress, key);
}


bool Widget::onKeyboardRelease(keyboard_event_t key) {
    return default_event_handler<keyboard_event_t>(_root, _childs, &Widget::onKeyboardRelease, key);
}


bool Widget::onMousePress(mouse_event_t key) {
    return default_event_handler<mouse_event_t, true>(_root, _childs, &Widget::onMousePress, key);
}


bool Widget::onMouseRelease(mouse_event_t key) {
    return default_event_handler<mouse_event_t, true>(_root, _childs, &Widget::onMouseRelease, key);
}

bool Widget::onMouseMove(mouse_event_t key) {
    return default_event_handler<mouse_event_t>(_root, _childs, &Widget::onMouseMove, key);
}


bool Widget::onClock(uint64_t delta) {
    return default_event_handler<uint64_t>(_root, _childs, &Widget::onClock, delta);
}

// ---------------------------------------------------------------------------------------------------------------------

void Widget::register_object(Widget *child) {
    Vector vec_mov = {_active_area.low_x, _active_area.low_y};
    recursive_update(child, update_coords, &vec_mov);

    if (child->_pos.x + child->_size.x > _active_area.high_x) {
        child->_size.x = _active_area.high_x - child->_pos.x;
    }

    if (child->_pos.y + child->_size.y > _active_area.high_y) {
        child->_size.y = _active_area.high_y - child->_pos.y;
    }

    register_object_exact_pos(child);
}

void Widget::register_object_exact_pos(Widget *child) {
    assert(child != this);
    
    child->_parent = this;
    recursive_update(child, set_root, _root);
    
    _childs.push_front(child);
    
    _root->recalc_regions();
}

// ---------------------------------------------------------------------------------------------------------------------

void recursive_update(Widget * widget, transform_f func, void* args, 
                     checker_f check, void* check_args) {
    if (check && !check(widget, check_args)) {
        return;
    }

    for (auto child : widget->_childs) {
        recursive_update(child, func, args, check, check_args);
    }

    func(widget, args);
}

// ---------------------------------------------------------------------------------------------------------------------

static inline bool no_hit(const Point& pos, const Vector& size, const mouse_event_t& event) {
    double rel_x = event.position.x - pos.x;
    double rel_y = event.position.y - pos.y;

    return !((0 < rel_x && rel_x < size.x) && (0 < rel_y && rel_y < size.y));
}

// ---------------------------------------------------------------------------------------------------------------------

void update_coords(Widget * widget, void *args) {
    Vector base_point = *static_cast<Vector *>(args);
    widget->move(base_point);
}

void Widget::move(const Vector& shift) {
    _pos += shift;
    _reg.shift(Vector(shift.x, shift.y));
    
    // update active area
    _active_area.low_x  += shift.x;
    _active_area.high_x += shift.x;
    _active_area.low_y  += shift.y;
    _active_area.high_y += shift.y;
}

// ---------------------------------------------------------------------------------------------------------------------

void set_root(Widget * widget, void *args) {
    Widget *root = static_cast<Widget *>(args);
    widget->_root = root;
}

// ---------------------------------------------------------------------------------------------------------------------

void Widget::recalc_regions() {
    Region new_reg = get_default_region();

    if (_parent) {
        new_reg *= _parent->_reg;

        for (const auto& subling: _parent->_childs) {
            if (subling == this) { break; }
            new_reg -= subling->get_default_region();
        }
    }

    _reg = new_reg;

    for (auto& child: _childs) {
        child->recalc_regions();
    }

    for (const auto& child: _childs) {
        _reg -= child->get_default_region();
    }
}

// ---------------------------------------------------------------------------------------------------------------------

bool Widget::recursive_cleanup() {
    bool has_deleted = false;
    for (auto child = _childs.begin(); child != _childs.end(); ++child) {
        if (!((*child)->is_alive())) {
            delete (*child);
            child = _childs.erase(child);
            has_deleted = true;
        }

        (*child)->recursive_cleanup();
    }

    return has_deleted;
}

// ---------------------------------------------------------------------------------------------------------------------

void Widget::render(RenderTarget& target) {
    for (auto child = _childs.rbegin(); child != _childs.rend(); ++child) {
        (*child)->render(target);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void WindowManager::render(RenderTarget& target) {
    cleanup();
    target.drawRect(_reg, _pos, _size, WM_BACKGROUND_COLOR);
    target.drawLine(_reg, _pos + Vector(0, HEADER_HEIGHT), Vector(_size.x, 0), sf::Color::Black);

    Widget::render(target);
}

// ---------------------------------------------------------------------------------------------------------------------

void WindowManager::cleanup() {
    bool cleaned = recursive_cleanup();
    if (cleaned) {
        recalc_regions();
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void ColorIndicator::render(RenderTarget& target) {
    target.drawLine(_reg, _pos,                      Vector(_size.x, 0), sf::Color::White);
    target.drawLine(_reg, _pos + Vector(0, _size.y), Vector(_size.x, 0), sf::Color::White);
    target.drawLine(_reg, _pos,                      Vector(0, _size.y), sf::Color::White);
    target.drawLine(_reg, _pos + Vector(_size.x, 0), Vector(0, _size.y), sf::Color::White);
    target.drawRect(_reg, _pos, _size, *color_ptr);
}

// ---------------------------------------------------------------------------------------------------------------------

static void add_symbol(const keyboard_event_t& key, string& string);

bool TextBox::onKeyboardPress(keyboard_event_t key) {
    if (is_writing) {
        add_symbol(key, content);
        return EVENT_RES::STOP;
    }

    return EVENT_RES::CONT;
}

bool TextBox::onMousePress(mouse_event_t key) {
    bool hit_y = key.position.y >= _pos.y && key.position.y <= _pos.y + _size.y;
    bool hit_x = key.position.x >= _pos.x && key.position.x <= _pos.x + _size.x;
    is_writing = (hit_x && hit_y);

    return is_writing ? EVENT_RES::STOP : EVENT_RES::CONT;
}

bool TextBox::onMouseRelease(mouse_event_t key) {
    bool hit_y = key.position.y >= _pos.y && key.position.y <= _pos.y + _size.y;
    bool hit_x = key.position.x >= _pos.x && key.position.x <= _pos.x + _size.x;
    is_writing = (hit_x && hit_y);

    return is_writing ? EVENT_RES::STOP : EVENT_RES::CONT;
}

bool TextBox::onMouseMove(mouse_event_t key) {
    bool hit_y = key.position.y >= _pos.y && key.position.y <= _pos.y + _size.y;
    bool hit_x = key.position.x >= _pos.x && key.position.x <= _pos.x + _size.x;
    is_writing = (is_writing && hit_x && hit_y);
    
    return (hit_x && hit_y) ? EVENT_RES::STOP : EVENT_RES::CONT;
}

void TextBox::render(RenderTarget& target) {
    target.drawRect(_reg, _pos, _size, TEXTBOX_BACKGROUND_COLOR);
    target.drawText(_reg, _pos, content.c_str(), TITLE_SIZE);
}

static void add_symbol(const keyboard_event_t& key, string& string) {
    int key_code = (int)key.key;

    if (key_code <= sf::Keyboard::Z) {
        if (key.shift) {
            string += 'A' + key_code;
        } else {
            string += 'a' + key_code;
        }

        return; 
    }

    if (key_code >= sf::Keyboard::Num0 && key_code <= sf::Keyboard::Num9) {
        string += '0' + key_code- sf::Keyboard::Num0;
        return;
    }

    if (key_code == sf::Keyboard::Backspace) {
        string.pop_back();
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void WindowManager::createWidgetI(plugin::PluginWidgetI* widget) {
    auto wrapper = new WidgetPlugin(widget, widget);
    widget->host = wrapper;
}
