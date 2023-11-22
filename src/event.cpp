#include "event.h"
#include <cstring>

template<class T, EVENT_TYPES type>
bool EventManager::default_event_handler(event_handler_func_t<T> handler_func, T event) {
    bool was_stop = false;

    for (const auto& child: _childs) {
        if (child->getPriority() < priorities[type]) { continue; }

        bool res = (child->*handler_func)(event);

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

bool EventManager::onKeyboardPress(keyboard_event_t key) {
    return default_event_handler<keyboard_event_t, KEYBOARD_EVENT>(&EventSubscriber::onKeyboardPress, key);
}

bool EventManager::onKeyboardRelease(keyboard_event_t key) {
    return default_event_handler<keyboard_event_t, KEYBOARD_EVENT>(&EventSubscriber::onKeyboardRelease, key);
}

bool EventManager::onMousePress(mouse_event_t key) {
    return default_event_handler<mouse_event_t, MOUSE_EVENT>(&EventSubscriber::onMousePress, key);
}

bool EventManager::onMouseRelease(mouse_event_t key) {
    return default_event_handler<mouse_event_t, MOUSE_EVENT>(&EventSubscriber::onMouseRelease, key);
}

bool EventManager::onMouseMove(mouse_event_t key) {
    return default_event_handler<mouse_event_t, MOUSE_EVENT>(&EventSubscriber::onMouseMove, key);
}

bool EventManager::onClock(uint64_t delta) {
    return default_event_handler<uint64_t, TIMER_EVENT>(&EventSubscriber::onClock, delta);
}

void EventManager::set_priority(dynarray<EVENT_TYPES> types, int new_priority) {
    for (const auto type: types) {
        priorities[type] = new_priority;
    }
}

void EventManager::reset_priority() {
    std::memset(priorities, 0, EVENT_TYPES_NUM * sizeof(int));
}

#if LOG_EVENTS
EVENT_RES EventLogger::onKeyboardPress(const keyboard_event_t& key) {
    stream << "Keyboard Press <" << key << ">\n";
    return EVENT_RES::CONT;
}

EVENT_RES EventLogger::onKeyboardRelease(const keyboard_event_t& key) {
    stream << "Keyboard Release <" << key << ">\n";
    return EVENT_RES::CONT;
}

EVENT_RES EventLogger::onMousePress(const mouse_event_t& key) {
    stream << "Mouse Press <" << key << ">\n";
    return EVENT_RES::CONT;
}

EVENT_RES EventLogger::onMouseRelease(const mouse_event_t& key) {
    stream << "Mouse Release <" << key << ">\n";
    return EVENT_RES::CONT;
}

EVENT_RES EventLogger::onMouseMove(const mouse_event_t& key) {
    stream << "Mouse Move <" << key << ">\n";
    return EVENT_RES::CONT;
}

EVENT_RES EventLogger::onClock(const time_point& time) {
    stream << "Timer <" << time << ">\n";
    return EVENT_RES::CONT;
}
#endif