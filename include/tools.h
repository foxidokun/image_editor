#pragma once

#include <SFML/Graphics.hpp>
#include "point.h"
#include "rendertarget.h"
#include "widget.h"

class Tool {
public:
    virtual void paint_on_press  (RenderTarget& permanent, RenderTarget& tmp, const mouse_event_t& point_pos, const Color& color) = 0;
    virtual void paint_on_move   (RenderTarget& permanent, RenderTarget& tmp, const mouse_event_t& point_pos, const Color& color) = 0;
    virtual void paint_on_release(RenderTarget& permanent, RenderTarget& tmp, const mouse_event_t& point_pos, const Color& color) = 0;
    virtual ~Tool() = default;
};

class ToolManager {
private:
    Tool *active_tool; // owned
    Color color;

public:
    ToolManager(Tool *tool = nullptr, const Color& color = Color{0,0,0,255}): active_tool(tool), color(color) {}

    void paint_on_press(RenderTarget& permanent, RenderTarget& tmp, const mouse_event_t& point_pos) const {
        active_tool->paint_on_press(permanent, tmp, point_pos, color);
    }

    void paint_on_move(RenderTarget& permanent, RenderTarget& tmp, const mouse_event_t& point_pos) const {
        active_tool->paint_on_move(permanent, tmp, point_pos, color);
    }

    void paint_on_release(RenderTarget& permanent, RenderTarget& tmp, const mouse_event_t& point_pos) const {
        active_tool->paint_on_release(permanent, tmp, point_pos, color);
    }

    void set_tool(Tool *new_tool) {
        delete active_tool;
        active_tool = new_tool;
    }

    void set_color(const Color& new_color) {
        color = new_color;
    }

    ~ToolManager() {
        delete active_tool;
    }
};

class Brush: public Tool {
private:
    Point last_pos;

    void paint(RenderTarget& permanent, const Point& point_pos, const Color& color) const;

public:
    void paint_on_press  (RenderTarget& permanent, RenderTarget& tmp, const mouse_event_t& point_pos, const Color& color) final;
    void paint_on_move   (RenderTarget& permanent, RenderTarget& tmp, const mouse_event_t& point_pos, const Color& color) final;
    void paint_on_release(RenderTarget& permanent, RenderTarget& tmp, const mouse_event_t& point_pos, const Color& color) final
    {}
};

class AlienBrush: public Tool {
private:
    Point start_pos;

    void paint(RenderTarget& permanent, const Point& point_pos, const Color& color) const;

public:
    void paint_on_press  (RenderTarget& permanent, RenderTarget& tmp, const mouse_event_t& point_pos, const Color& color) final;
    void paint_on_move   (RenderTarget& permanent, RenderTarget& tmp, const mouse_event_t& point_pos, const Color& color) final;
    void paint_on_release(RenderTarget& permanent, RenderTarget& tmp, const mouse_event_t& point_pos, const Color& color) final {}
};

class Polyline: public Tool {
private:
    Point last_point;
    bool has_drawn = false;
    bool unclear_tmp = false;

public:
    Polyline() = default;

    void paint_on_press  (RenderTarget& permanent, RenderTarget& tmp, const mouse_event_t& point_pos, const Color& color) final;
    void paint_on_move   (RenderTarget& permanent, RenderTarget& tmp, const mouse_event_t& point_pos, const Color& color) final;
    void paint_on_release(RenderTarget& permanent, RenderTarget& tmp, const mouse_event_t& point_pos, const Color& color) final;
};

class RectTool: public Tool {
private:
    Point start_point;
    bool is_drawing;

public:
    RectTool() = default;

    void paint_on_press  (RenderTarget& permanent, RenderTarget& tmp, const mouse_event_t& point_pos, const Color& color) final;
    void paint_on_move   (RenderTarget& permanent, RenderTarget& tmp, const mouse_event_t& point_pos, const Color& color) final;
    void paint_on_release(RenderTarget& permanent, RenderTarget& tmp, const mouse_event_t& point_pos, const Color& color) final;
};

class EllipseTool: public Tool {
private:
    Point start_point;
    bool is_drawing;

public:
    EllipseTool() = default;

    void paint_on_press  (RenderTarget& permanent, RenderTarget& tmp, const mouse_event_t& point_pos, const Color& color) final;
    void paint_on_move   (RenderTarget& permanent, RenderTarget& tmp, const mouse_event_t& point_pos, const Color& color) final;
    void paint_on_release(RenderTarget& permanent, RenderTarget& tmp, const mouse_event_t& point_pos, const Color& color) final;
};