#pragma once

#include <SFML/Graphics.hpp>
#include "point.h"
#include "rendertarget.h"
#include "widget.h"

class ToolManager: public plugin::ToolManagerI {
private:
    plugin::ToolI *active_tool; // owned
    Color color;

public:
    ToolManager(plugin::ToolI *tool = nullptr, const Color& color = Color{0,0,0,255}): active_tool(tool), color(color) {}

    void paintOnPress(plugin::RenderTargetI* permanent, plugin::RenderTargetI* tmp, mouse_event_t point_pos) final {
        active_tool->paintOnPress(permanent, tmp, point_pos, color);
    }

    void paintOnMove(plugin::RenderTargetI* permanent, plugin::RenderTargetI* tmp, mouse_event_t point_pos) final {
        active_tool->paintOnMove(permanent, tmp, point_pos, color);
    }

    void paintOnRelease(plugin::RenderTargetI* permanent, plugin::RenderTargetI* tmp, mouse_event_t point_pos) final {
        active_tool->paintOnRelease(permanent, tmp, point_pos, color);
    }

    void disableTool(plugin::RenderTargetI* permanent, plugin::RenderTargetI* tmp, mouse_event_t point_pos) final {
        active_tool->disable(permanent, tmp, point_pos, color);
    }

    void set_color(const Color& new_color) {
        color = new_color;
    }

    const Color* get_color_ptr() const { return &color; }

    ~ToolManager() = default;

    void setColor(Color color) final { set_color(color); }
    void setTool(plugin::ToolI *tool) final { active_tool = tool; };

    plugin::ToolI *getTool() final { return active_tool; };
    Color  getColor() final { return color; };
};


class Brush: public plugin::ToolI {
private:
    Point last_pos;

    void paint(plugin::RenderTargetI *permanent, Vector point_pos, Color color) const;

public:
    void paintOnPress  (plugin::RenderTargetI *permanent, plugin::RenderTargetI* tmp, mouse_event_t point_pos, Color color) final;
    void paintOnMove   (plugin::RenderTargetI *permanent, plugin::RenderTargetI* tmp, mouse_event_t point_pos, Color color) final;
    void paintOnRelease(plugin::RenderTargetI *permanent, plugin::RenderTargetI* tmp, mouse_event_t point_pos, Color color) final{}

    void disable(plugin::RenderTargetI *permanent, plugin::RenderTargetI* tmp, mouse_event_t point_pos, Color color) final {};

    const plugin::Texture *getIcon() final { return nullptr; /* it is internal tool */ };

    plugin::Array<const char *> getParamNames() final { return plugin::Array<const char *>(0, nullptr); };
    plugin::Array<double> getParams() final { return plugin::Array<double>(0, nullptr); };
    void setParams(plugin::Array<double> params) final {};
};

/*
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

class ColorPicker: public Tool {
private:
    ToolManager *tm;
    bool is_drawing;

    void set_color(RenderTarget& permanent, const mouse_event_t& point_pos);
public:
    ColorPicker(ToolManager *tm): tm(tm) {}

    void paint_on_press  (RenderTarget& permanent, RenderTarget& tmp, const mouse_event_t& point_pos, const Color& color) final;
    void paint_on_move   (RenderTarget& permanent, RenderTarget& tmp, const mouse_event_t& point_pos, const Color& color) final;
    void paint_on_release(RenderTarget& permanent, RenderTarget& tmp, const mouse_event_t& point_pos, const Color& color) final;
};

class FillTool: public Tool {
public:
    void paint_on_press  (RenderTarget& permanent, RenderTarget& tmp, const mouse_event_t& point_pos, const Color& color) final;
    void paint_on_move   (RenderTarget& permanent, RenderTarget& tmp, const mouse_event_t& point_pos, const Color& color) final {};
    void paint_on_release(RenderTarget& permanent, RenderTarget& tmp, const mouse_event_t& point_pos, const Color& color) final {};
};

class SplineTool: public Tool {
private:
    Point points[4];
    uint cnt = 0;
public:
    void paint_on_press  (RenderTarget& permanent, RenderTarget& tmp, const mouse_event_t& point_pos, const Color& color) final;
    void paint_on_move   (RenderTarget& permanent, RenderTarget& tmp, const mouse_event_t& point_pos, const Color& color) final {};
    void paint_on_release(RenderTarget& permanent, RenderTarget& tmp, const mouse_event_t& point_pos, const Color& color) final;
};

*/