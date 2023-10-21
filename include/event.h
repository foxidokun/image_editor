#pragma once

#include <chrono>
#include "dynarray.h"

// ---------------------------------------------------------------------------------------------------------------------

struct mouse_event_t {
    double x;
    double y;

    enum class button_type {
        NONE = 0,
        LEFT,
        RIGHT,
        UNKNOWN
    } button;
};


enum class EVENT_RES {
    CONT,
    STOP
};

typedef char keyboard_event_t;

using time_point = std::chrono::time_point<std::chrono::system_clock>;

// ---------------------------------------------------------------------------------------------------------------------

class EventSubscriber {
public:
    virtual EVENT_RES on_keyboard_press  (const keyboard_event_t& key) = 0;
    virtual EVENT_RES on_keyboard_release(const keyboard_event_t& key) = 0;
    virtual EVENT_RES on_mouse_press     (const mouse_event_t& key)    = 0;
    virtual EVENT_RES on_mouse_release   (const mouse_event_t& key)    = 0;
    virtual EVENT_RES on_mouse_move      (const mouse_event_t& key)    = 0;
    virtual EVENT_RES on_timer           (const time_point& time)      = 0;
};

// ---------------------------------------------------------------------------------------------------------------------

template<typename T>
using event_handler_func_t = EVENT_RES (EventSubscriber::*)(const T& event);

class EventManager: EventSubscriber {
private:
    dynarray<EventSubscriber *> _childs;

    template<typename T>
    EVENT_RES default_event_handler(event_handler_func_t<T> handler_func, const T& event);
public:
    void register_object(EventSubscriber *child) {
        _childs.push_back(child);
    }

    virtual EVENT_RES on_keyboard_press  (const keyboard_event_t& key) override;
    virtual EVENT_RES on_keyboard_release(const keyboard_event_t& key) override;
    virtual EVENT_RES on_mouse_press     (const mouse_event_t& key)    override;
    virtual EVENT_RES on_mouse_release   (const mouse_event_t& key)    override;
    virtual EVENT_RES on_mouse_move      (const mouse_event_t& key)    override;
    virtual EVENT_RES on_timer           (const time_point& time)      override;

    void timer_event() {
        on_timer(std::chrono::system_clock::now());
    }
};

// ---------------------------------------------------------------------------------------------------------------------

class EventLogger: public EventSubscriber {
private:
    std::ostream& stream;

public:
    EventLogger(std::ostream& stream): stream(stream) {}

    virtual EVENT_RES on_keyboard_press  (const keyboard_event_t& key) override;
    virtual EVENT_RES on_keyboard_release(const keyboard_event_t& key) override;
    virtual EVENT_RES on_mouse_press     (const mouse_event_t& key)    override;
    virtual EVENT_RES on_mouse_release   (const mouse_event_t& key)    override;
    virtual EVENT_RES on_mouse_move      (const mouse_event_t& key)    override;
    virtual EVENT_RES on_timer           (const time_point& time)      override;
};

// ---------------------------------------------------------------------------------------------------------------------

static std::ostream& operator<<(std::ostream& stream, const mouse_event_t& event) {
    const char BUTTON_NAMES[][10] = {"NONE", "Left", "Right", "UNKNOWN"};

    const char *button_name = BUTTON_NAMES[(int) event.button];

    stream << "{ pos: (" << event.x << ", " << event.y << ") KEY: " << button_name << "}";
    return stream;
}
