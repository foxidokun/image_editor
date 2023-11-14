#pragma once

#include <chrono>
#include <SFML/Graphics.hpp>
#include "dynarray.h"
#include "list.h"
#include "plugin.h"

#define LOG_EVENTS 0

// ---------------------------------------------------------------------------------------------------------------------

enum EVENT_RES {
    CONT = true,
    STOP = false
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

using time_point = std::chrono::time_point<std::chrono::system_clock>;
using keyboard_event_t = plugin::KeyboardContext;
using mouse_event_t = plugin::MouseContext;

// ---------------------------------------------------------------------------------------------------------------------

class EventSubscriber: public plugin::EventProcessableI {
protected:
    EVENT_PRIORITIES _priority = EVENT_PRIORITIES::DEFAULT;

public:
    uint priority() const { return (uint)_priority; }
};

// ---------------------------------------------------------------------------------------------------------------------

template<typename T>
using event_handler_func_t = bool (plugin::EventProcessableI::*)(T event);

class EventManager: EventSubscriber {
private:
    linked_list<EventSubscriber *> _childs;
    int priorities[EVENT_TYPES_NUM] = {};

    template<typename T, EVENT_TYPES type>
    bool default_event_handler(event_handler_func_t<T> handler_func, T event);
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
    virtual bool onKeyboardPress  (keyboard_event_t key) override;
    virtual bool onKeyboardRelease(keyboard_event_t key) override;
    virtual bool onMousePress     (mouse_event_t key)    override;
    virtual bool onMouseRelease   (mouse_event_t key)    override;
    virtual bool onMouseMove      (mouse_event_t key)    override;
    virtual bool onClock          (uint64_t delta)       override;

    // Timer interru^W ebent generator
    void timer_event() {
        onClock(33333); // 30 fps
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

    virtual bool onKeyboardPress  (keyboard_event_t key) override EVENTLOGGER_DEFINED_STUB;
    virtual bool onKeyboardRelease(keyboard_event_t key) override EVENTLOGGER_DEFINED_STUB;
    virtual bool onMousePress     (mouse_event_t key)    override EVENTLOGGER_DEFINED_STUB;
    virtual bool onMouseRelease   (mouse_event_t key)    override EVENTLOGGER_DEFINED_STUB;
    virtual bool onMouseMove      (mouse_event_t key)    override EVENTLOGGER_DEFINED_STUB;
    virtual bool onClock          (uint64_t delta)       override EVENTLOGGER_DEFINED_STUB;
};

// ---------------------------------------------------------------------------------------------------------------------

static std::ostream& operator<<(std::ostream& stream, const mouse_event_t& event) {
    const char BUTTON_NAMES[][10] = {"NONE", "Left", "Right", "UNKNOWN"};

    const char *button_name = BUTTON_NAMES[(int) event.button];

    stream << "{ pos: (" << event.position.x << ", " << event.position.y << ") KEY: " << button_name << "}";
    return stream;
}
