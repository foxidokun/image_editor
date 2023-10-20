#pragma once

#include <chrono>

// ---------------------------------------------------------------------------------------------------------------------

struct mouse_event_t {
    double x;
    double y;

    enum class button_type {
        NONE,
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
    virtual EVENT_RES on_keyboard_press  (const keyboard_event_t& key) = 0;
    virtual EVENT_RES on_keyboard_release(const keyboard_event_t& key) = 0;
    virtual EVENT_RES on_mouse_press     (const mouse_event_t& key) = 0;
    virtual EVENT_RES on_mouse_release   (const mouse_event_t& key) = 0;
    virtual EVENT_RES on_mouse_move      (const mouse_event_t& key) = 0;
    virtual EVENT_RES on_timer           (const time_point& time) = 0;
};
