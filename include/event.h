#pragma once

#include <chrono>
#include <SFML/Graphics.hpp>
#include "dynarray.h"
#include "list.h"

#define LOG_EVENTS 0

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

enum EVENT_TYPES {
    MOUSE_EVENT    = 0,
    KEYBOARD_EVENT,
    TIMER_EVENT,
    EVENT_TYPES_NUM
};

enum class EVENT_PRIORITIES {
    DEFAULT = 0,
    MODAL_WINDOW = 1,
    EVENT_LOGGER = 228
};

typedef sf::Event::KeyEvent keyboard_event_t;

using time_point = std::chrono::time_point<std::chrono::system_clock>;

// ---------------------------------------------------------------------------------------------------------------------

class EventSubscriber {
protected:
    EVENT_PRIORITIES _priority = EVENT_PRIORITIES::DEFAULT;

public:
    virtual EVENT_RES on_keyboard_press  (const keyboard_event_t& key) = 0;
    virtual EVENT_RES on_keyboard_release(const keyboard_event_t& key) = 0;
    virtual EVENT_RES on_mouse_press     (const mouse_event_t& key)    = 0;
    virtual EVENT_RES on_mouse_release   (const mouse_event_t& key)    = 0;
    virtual EVENT_RES on_mouse_move      (const mouse_event_t& key)    = 0;
    virtual EVENT_RES on_timer           (const time_point& time)      = 0;

    uint priority() const { return (uint)_priority; }
};

// ---------------------------------------------------------------------------------------------------------------------

template<typename T>
using event_handler_func_t = EVENT_RES (EventSubscriber::*)(const T& event);

class EventManager: EventSubscriber {
private:
    linked_list<EventSubscriber *> _childs;
    int priorities[EVENT_TYPES_NUM] = {};

    template<typename T, EVENT_TYPES type>
    EVENT_RES default_event_handler(event_handler_func_t<T> handler_func, const T& event);
public:
    void register_object(EventSubscriber *child) {
        _childs.push_back(child);
    }

    void unregister_object(EventSubscriber *child) {
        _childs.erase(std::remove(_childs.begin(), _childs.end(), child), _childs.end());
    }

    void set_priority(dynarray<EVENT_TYPES> types, int new_priority);
    void reset_priority();

    // EventSubscriber API
    virtual EVENT_RES on_keyboard_press  (const keyboard_event_t& key) override;
    virtual EVENT_RES on_keyboard_release(const keyboard_event_t& key) override;
    virtual EVENT_RES on_mouse_press     (const mouse_event_t& key)    override;
    virtual EVENT_RES on_mouse_release   (const mouse_event_t& key)    override;
    virtual EVENT_RES on_mouse_move      (const mouse_event_t& key)    override;
    virtual EVENT_RES on_timer           (const time_point& time)      override;

    // Timer interru^W ebent generator
    void timer_event() {
        on_timer(std::chrono::system_clock::now());
    }
};

// ---------------------------------------------------------------------------------------------------------------------

#if LOG_EVENTS
    #define EVENTLOGGER_DEFINED_STUB ;
#else
    #define EVENTLOGGER_DEFINED_STUB {return EVENT_RES::CONT;}
#endif

class EventLogger: public EventSubscriber {
private:
    std::ostream& stream;

public:
    EventLogger(std::ostream& stream): stream(stream) {}

    virtual EVENT_RES on_keyboard_press  (const keyboard_event_t& key) override EVENTLOGGER_DEFINED_STUB;
    virtual EVENT_RES on_keyboard_release(const keyboard_event_t& key) override EVENTLOGGER_DEFINED_STUB;
    virtual EVENT_RES on_mouse_press     (const mouse_event_t& key)    override EVENTLOGGER_DEFINED_STUB;
    virtual EVENT_RES on_mouse_release   (const mouse_event_t& key)    override EVENTLOGGER_DEFINED_STUB;
    virtual EVENT_RES on_mouse_move      (const mouse_event_t& key)    override EVENTLOGGER_DEFINED_STUB;
    virtual EVENT_RES on_timer           (const time_point& time)      override EVENTLOGGER_DEFINED_STUB;
};

// ---------------------------------------------------------------------------------------------------------------------

static std::ostream& operator<<(std::ostream& stream, const mouse_event_t& event) {
    const char BUTTON_NAMES[][10] = {"NONE", "Left", "Right", "UNKNOWN"};

    const char *button_name = BUTTON_NAMES[(int) event.button];

    stream << "{ pos: (" << event.x << ", " << event.y << ") KEY: " << button_name << "}";
    return stream;
}
