#include "event.h"

template<class T>
EVENT_RES EventManager::default_event_handler(event_handler_func_t<T> handler_func, const T& event) {
    bool was_stop = false;

    for (const auto& child: _childs) {
        EVENT_RES res = (child->*handler_func)(event);

        if (res == EVENT_RES::STOP) {
            was_stop = true;
        }
    }

    if (was_stop) {
        return EVENT_RES::STOP;
    } else {
        return EVENT_RES::CONT;
    }
}

EVENT_RES EventManager::on_keyboard_press(const keyboard_event_t& key) {
    return default_event_handler<keyboard_event_t>(&EventSubscriber::on_keyboard_press, key);
}

EVENT_RES EventManager::on_keyboard_release(const keyboard_event_t& key) {
    return default_event_handler<keyboard_event_t>(&EventSubscriber::on_keyboard_release, key);
}

EVENT_RES EventManager::on_mouse_press(const mouse_event_t& key) {
    return default_event_handler<mouse_event_t>(&EventSubscriber::on_mouse_press, key);
}

EVENT_RES EventManager::on_mouse_release(const mouse_event_t& key) {
    return default_event_handler<mouse_event_t>(&EventSubscriber::on_mouse_release, key);
}

EVENT_RES EventManager::on_mouse_move(const mouse_event_t& key) {
    return default_event_handler<mouse_event_t>(&EventSubscriber::on_mouse_move, key);
}

EVENT_RES EventManager::on_timer(const time_point& time) {
    return default_event_handler<time_point>(&EventSubscriber::on_timer, time);
}


#if LOG_EVENTS
EVENT_RES EventLogger::on_keyboard_press(const keyboard_event_t& key) {
    stream << "Keyboard Press <" << key << ">\n";
    return EVENT_RES::CONT;
}

EVENT_RES EventLogger::on_keyboard_release(const keyboard_event_t& key) {
    stream << "Keyboard Release <" << key << ">\n";
    return EVENT_RES::CONT;
}

EVENT_RES EventLogger::on_mouse_press(const mouse_event_t& key) {
    stream << "Mouse Press <" << key << ">\n";
    return EVENT_RES::CONT;
}

EVENT_RES EventLogger::on_mouse_release(const mouse_event_t& key) {
    stream << "Mouse Release <" << key << ">\n";
    return EVENT_RES::CONT;
}

EVENT_RES EventLogger::on_mouse_move(const mouse_event_t& key) {
    stream << "Mouse Move <" << key << ">\n";
    return EVENT_RES::CONT;
}

EVENT_RES EventLogger::on_timer(const time_point& time) {
    stream << "Timer <" << time << ">\n";
    return EVENT_RES::CONT;
}
#endif