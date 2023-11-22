#pragma once
#include "rendertarget.h"
#include "list.h"
#include "point.h"
#include "vector.h"
#include <cassert>
#include <iostream>
#include "regions.h"
#include "string.h"
#include "event.h"

// ---------------------------------------------------------------------------------------------------------------------

class Renderable {
    virtual void render(RenderTarget& target) = 0;
};

// ---------------------------------------------------------------------------------------------------------------------

class Widget;

// ---------------------------------------------------------------------------------------------------------------------

class WidgetPtr: public EventSubscriber {
private:
    union {
        Widget* internal_ptr_;
        plugin::WidgetI* external_ptr_;
    };
    plugin::EventProcessableI* event_ptr;

    bool is_internal_;

public:
    WidgetPtr(Widget *widget);
    WidgetPtr(plugin::WidgetI *widget);
    WidgetPtr(): internal_ptr_(nullptr), event_ptr(nullptr), is_internal_(true) {}

    bool is_internal() const { return is_internal_; }
    Widget* internal_ptr() const { return internal_ptr_; }
    plugin::WidgetI* external_ptr() const { return external_ptr_; }

    Region get_default_region() const;
    void free();
    bool is_alive() const;

    Vector size() const;
    void set_size(Vector val) const;

    bool onKeyboardPress  (keyboard_event_t key) final { return event_ptr->onKeyboardPress(key); };
    bool onKeyboardRelease(keyboard_event_t key) final { return event_ptr->onKeyboardRelease(key); };
    bool onMousePress     (mouse_event_t key)    final { return event_ptr->onMousePress(key); };
    bool onMouseRelease   (mouse_event_t key)    final { return event_ptr->onMouseRelease(key); };
    bool onMouseMove      (mouse_event_t key)    final { return event_ptr->onMouseMove(key); };
    bool onClock          (uint64_t delta)       final { return event_ptr->onClock(delta); };

    Rectangle active_area() const;

    void recalc_regions();

    operator bool() const { return static_cast<bool>(internal_ptr_); }

    bool operator==(Widget* widget_ptr) const { return is_internal_ && (internal_ptr_ == widget_ptr); }

    void set_root(WidgetPtr root);
    void move(Vector delta);
};

using transform_f = void(*)(WidgetPtr, void *);
using checker_f   = bool(*)(WidgetPtr, void *); 

// ---------------------------------------------------------------------------------------------------------------------

class Widget: public Renderable, public EventSubscriber {
protected:
    WidgetPtr _parent;
    WidgetPtr _root   = this;
    linked_list<WidgetPtr> _childs;
    Point _pos;
    Vector _size;
    Region _reg;
    Rectangle _active_area;
    bool _is_alive = true;

    friend WidgetPtr;

protected:
    bool recursive_cleanup();

public:
    Widget(const Point& pos, const Vector& size):
        _pos(pos),
        _size(size)
        {
            Rectangle whole = {_pos.x, _pos.y, _pos.x+_size.x, _pos.y+_size.y};
            _reg.add_rectangle(whole);
            _active_area = whole;
        }

    virtual bool onKeyboardPress  (keyboard_event_t key) override;
    virtual bool onKeyboardRelease(keyboard_event_t key) override;
    virtual bool onMousePress     (mouse_event_t key) override;
    virtual bool onMouseRelease   (mouse_event_t key) override;
    virtual bool onMouseMove      (mouse_event_t key) override;
    virtual bool onClock          (uint64_t delta) override;

    virtual void render(RenderTarget& target) override;

    virtual Region get_default_region() const {
        return Region(Rectangle{_pos.x, _pos.y, _pos.x + _size.x, _pos.y + _size.y});
    }

    virtual void move(const Vector& shift);

    void register_object(Widget *child);
    void register_object_exact_pos(Widget *child);

    void kill()     { _is_alive = false; }
    bool is_alive() { return _is_alive; }

    const Point&     pos()         const { return _pos;         }
    const Vector&    size()        const { return _size;        }
    const Rectangle& active_area() const { return _active_area; }
    WidgetPtr        parent()      const { return _parent;      }
    
    void             set_pos   (const Point&   pos) { _pos  = pos;  }
    void             set_size  (const Vector& size) { _size = size; }
    
    Region& reg() { return _reg; }

    virtual ~Widget() {
        for (auto &x: _childs) {
            x.free();
        }
    }

    friend void recursive_update(WidgetPtr widget, transform_f func, void* args, 
                     checker_f check, void* check_args);
    friend void update_coords(WidgetPtr widget, void *args);
    friend void set_root(WidgetPtr widget, void *args);

    void recalc_regions();

    virtual void print(std::ostream& stream) const {
        stream << "basic widget";
    }

    Rectangle get_hit_rectangle() {
        return {_pos.x, _pos.y, _pos.x + _size.x, _pos.y + _size.y};
    }
};

// ---------------------------------------------------------------------------------------------------------------------

class WindowManager: public Widget, public plugin::GuiI {
public:
    WindowManager(double width, double height):
        Widget(Point(0,0), Vector(width, height))
        {
            _root = this;
            _active_area.low_y += MENU_HEIGHT;
        }

    void print(std::ostream& stream) const final { stream << "Window manager"; }

    void cleanup();

    virtual void render(RenderTarget& target) override;

    virtual Vec2 getSize() { assert(0); };

    plugin::RenderTargetI* getRenderTarget(Vec2 size, Vec2 pos, plugin::Plugin *self) final { assert(0); };
    void createParamWindow(plugin::Array<const char *> param_names, plugin::Interface * self) final { assert(0); };
    plugin::WidgetI* getRoot() final { assert(0); };
};

// ---------------------------------------------------------------------------------------------------------------------

class ColorIndicator: public Widget {
private:
    const Color *color_ptr;
public:
    ColorIndicator(const Point& pos, const Vector& size, const Color* color_ptr): 
        Widget(pos, size),
        color_ptr(color_ptr)
        {}

    void print(std::ostream& stream) const final { stream << "Color Indicator"; }

    virtual void render(RenderTarget& target) override;
};

// ---------------------------------------------------------------------------------------------------------------------

class TextBox: public Widget {
private:
    string content;
    bool is_writing = false;
public:
    TextBox(const Point& pos, const Vector& size): 
        Widget(pos, size)
        {}

    virtual bool onKeyboardPress  (keyboard_event_t key) override;
    virtual bool onKeyboardRelease(keyboard_event_t key) override {return EVENT_RES::CONT;}
    virtual bool onMousePress     (mouse_event_t key) override;
    virtual bool onMouseRelease   (mouse_event_t key) override;
    virtual bool onMouseMove      (mouse_event_t key) override;

    void print(std::ostream& stream) const final { stream << "Text Box with content <" << content << ">"; }

    const string& get_content() const { return content; }

    virtual void render(RenderTarget& target) override;
};

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

inline WidgetPtr::WidgetPtr(Widget *widget):
    internal_ptr_(widget),
    event_ptr(static_cast<plugin::EventProcessableI*>(widget)),
    is_internal_(true) {}

inline WidgetPtr::WidgetPtr(plugin::WidgetI *widget):
    external_ptr_(widget),
    event_ptr(static_cast<plugin::EventProcessableI*>(widget)),
    is_internal_(!(widget->isExtern())) {}

inline void WidgetPtr::free() {
    if (is_internal_) {
        delete internal_ptr_;
    } else {
        delete external_ptr_;
    }
}

inline bool WidgetPtr::is_alive() const {
    return (is_internal_) ? internal_ptr_->is_alive(): external_ptr_->getAvailable();
}

inline Region WidgetPtr::get_default_region() const {
    if (is_internal_) {
        return internal_ptr_->get_default_region();
    } else {
        Vec2 pos = external_ptr_->getPos();
        Vec2 size = external_ptr_->getSize();

        double min_x = std::min(pos.x, pos.x + size.x);
        double max_x = std::max(pos.x, pos.x + size.x);
        double min_y = std::min(pos.y, pos.y + size.y);
        double max_y = std::max(pos.y, pos.y + size.y);

        return Region(Rectangle(min_x, min_y, max_x, max_y));
    }
}

inline Rectangle WidgetPtr::active_area() const {
    if (is_internal_) {
        return internal_ptr_->active_area();
    } else {
        Vec2 pos = external_ptr_->getPos();
        Vec2 size = external_ptr_->getSize();

        double min_x = std::min(pos.x, pos.x + size.x);
        double max_x = std::max(pos.x, pos.x + size.x);
        double min_y = std::min(pos.y, pos.y + size.y);
        double max_y = std::max(pos.y, pos.y + size.y);

        return Rectangle(min_x, min_y, max_x, max_y);
    }
}

inline Vector WidgetPtr::size() const {
    return (is_internal_) ? internal_ptr_->size() : (Vector)external_ptr_->getSize();
}

inline void WidgetPtr::set_size(Vector val) const {
    if (is_internal_) {
        internal_ptr_->set_size(val);
    } else {
        external_ptr_->setSize(val);
    }
}

inline void WidgetPtr::recalc_regions() {
    if (is_internal_) {
        internal_ptr_->recalc_regions();
    } else {
        external_ptr_->recalcRegion();
    }
}

inline void WidgetPtr::set_root(WidgetPtr root) {
    if (is_internal_) {
        internal_ptr_->_root = root;
    } else {
        static bool warning = true;
        if (warning) {
            std::cerr << "Set root for external ... impossible. Lets just hope that set setParent is enough for him\n";
            warning = false;
        }
    }
}

inline void WidgetPtr::move(Vector delta) {
    if (is_internal_) {
        internal_ptr_->move(delta);
    } else {
        external_ptr_->move(delta);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void recursive_update(WidgetPtr widget, transform_f func, void* args, 
                     checker_f check = nullptr, void* check_args = nullptr);
void update_coords(WidgetPtr widget, void *args);
