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

using transform_f = void(*)(Widget *, void *);
using checker_f   = bool(*)(Widget *, void *); 

// ---------------------------------------------------------------------------------------------------------------------

class Widget: public Renderable, public EventSubscriber, public plugin::WidgetI {
protected:
    Widget * _parent = nullptr;
    Widget * _root   = this;
    linked_list<Widget *> _childs;
    Point _pos;
    Vector _size;
    Region _reg;
    Rectangle _active_area;
    bool _is_alive = true;

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

    bool onKeyboardPress  (keyboard_event_t key) override;
    bool onKeyboardRelease(keyboard_event_t key) override;
    bool onMousePress     (mouse_event_t key) override;
    bool onMouseRelease   (mouse_event_t key) override;
    bool onMouseMove      (mouse_event_t key) override;
    bool onClock          (uint64_t delta) override;

    void render(RenderTarget& target) override;

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
    Widget *         parent()      const { return _parent;      }
    
    void             set_pos   (const Point&   pos) { _pos  = pos;  }
    void             set_size  (const Vector& size) { _size = size; }
    
    Region& reg() { return _reg; }

    virtual ~Widget() {
        for (auto &x: _childs) {
            delete x;
        }
    }

    friend void recursive_update(Widget * widget, transform_f func, void* args, 
                     checker_f check, void* check_args);
    friend void update_coords(Widget * widget, void *args);
    friend void set_root(Widget * widget, void *args);

    void recalc_regions();

    virtual void print(std::ostream& stream) const {
        stream << "basic widget";
    }

    Rectangle get_hit_rectangle() {
        return {_pos.x, _pos.y, _pos.x + _size.x, _pos.y + _size.y};
    }

    void registerSubWidget(plugin::WidgetI* object) final { register_object(static_cast<Widget *>(object)); };
    void unregisterSubWidget(WidgetI* object) final { (static_cast<Widget *>(object))->kill(); };

    Vec2 getSize() const final { return _size; };
    void setSize(Vec2 new_size) final { _size = new_size; };

    Vec2 getPos() const final { return _pos; };
    void setPos(Vec2 new_pos) final {_pos = new_pos;};

    WidgetI *getParent() const final { return static_cast<WidgetI *>(_parent); };
    void setParent(WidgetI *root) final {_parent = static_cast<Widget *>(root);};

    void move(Vec2 shift) { move((Vector)shift); };

    // Жив ли виджет
    // Если true, то это идейно равносильно вызову деструктору, то есть его не надо рендерить, ему не надо передавать 
    // ивенты и тд и тп
    bool getAvailable() const final {return _is_alive;};
    void setAvailable(bool alive) { _is_alive = alive; };
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

    void render(RenderTarget& target) override;

    virtual Vec2 getSize() { assert(0); };

    plugin::WidgetI* getRoot() const final { assert(0); };

    void createWidgetI(plugin::PluginWidgetI* widget) final {assert(0);};

    // плагин через это у хоста запрашивает, есть ли плагин c таким id. nullptr если нет
    plugin::Plugin *queryPlugin(uint64_t id) final { return nullptr; };

    // принимает имя файла
    // например, если у хоста все asset'ы этого плагина валяются в assets/shit/<filename>, то 
    // сюда надо передавать только filename
    plugin::Texture *loadTextureFromFile(const char *filename) final { return nullptr; };

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

    void render(RenderTarget& target) override;
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

    bool onKeyboardPress  (keyboard_event_t key) override;
    bool onKeyboardRelease(keyboard_event_t key) override {return EVENT_RES::CONT;}
    bool onMousePress     (mouse_event_t key) override;
    bool onMouseRelease   (mouse_event_t key) override;
    bool onMouseMove      (mouse_event_t key) override;

    void print(std::ostream& stream) const final { stream << "Text Box with content <" << content << ">"; }

    const string& get_content() const { return content; }

    void render(RenderTarget& target) override;
};

// ---------------------------------------------------------------------------------------------------------------------

class WidgetPlugin: public Widget {
    WidgetPlugin(plugin::EventProcessableI *events, plugin::RenderableI *render):
        Widget(Vector(), Vector()),
        events_(events), render_(render) {}

    bool onKeyboardPress  (keyboard_event_t key) override { return events_.onKeyboardPress(key); }
    bool onKeyboardRelease(keyboard_event_t key) override { return events_.onKeyboardRelease(key); }
    bool onMousePress     (mouse_event_t key)    override { return events_.onMousePress(key); }
    bool onMouseRelease   (mouse_event_t key)    override { return events_.onMouseRelease(key); }
    bool onMouseMove      (mouse_event_t key)    override { return events_.onMouseMove(key); }
    bool onClock          (uint64_t delta)       override { return events_.onClock(delta); }

    void render(RenderTarget& target) override {
        render_->render(&target);
    }

private:
    plugin::EventProcessableI *events_;
    plugin::RenderableI *render_;
};

// ---------------------------------------------------------------------------------------------------------------------

void recursive_update(Widget * widget, transform_f func, void* args, 
                     checker_f check = nullptr, void* check_args = nullptr);
void update_coords(Widget * widget, void *args);
