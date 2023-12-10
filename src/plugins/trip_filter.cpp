#include "plugin.h"
#include "event.h"
#include "colors.h"
#include <iostream>
#include <string>
#include <cmath>

#define POINT_HALFSIZE 5

namespace {
    class ShittyCurveFilter;

    constexpr plugin::Color bar_colors[3] = {plugin::Color(255, 0, 0), plugin::Color(0, 255, 0), plugin::Color(0,0,255)};

    class CurveSetWidget: public plugin::PluginWidgetI {
    public:
        CurveSetWidget(ShittyCurveFilter* filter, plugin::RenderTargetI *canvas): filter_(filter), canvas_(canvas) {
            for (int i = 0; i < 3; ++i) {
                color_points_[i].push_back(Vector(0,0));
                color_points_[i].push_back(Vector(1,1));
            }
        }

        bool check_hit(const Vector& pos) {
            bool hit_x = pos.x > host->getPos().x && pos.x < host->getPos().x + host->getSize().x;
            bool hit_y = pos.y > host->getPos().y && pos.y < host->getPos().y + host->getSize().y;

            return hit_x && hit_y;
        }

        bool onMousePress(plugin::MouseContext context) final {
            if (!check_hit(context.position)) { return EVENT_RES::CONT; }

            Vector pos = host->getPos();
            if (context.position.y < pos.y + HEADER_HEIGHT) {
                moving = (context.button == plugin::MouseButton::Left);
                last_pos = context.position;
                return EVENT_RES::STOP;
            }

            if (context.button == plugin::MouseButton::Right) {
                if (color == 2) {
                    inform();
                    host->setAvailable(false);
                } else {
                    ++color;
                }

                return EVENT_RES::STOP;
            }

            dynarray<Vector>& points_ = color_points_[color];

            Vector val = context.position - host->getPos();;
            val.x /= host->getSize().x;
            val.y /= host->getSize().y;
            val.y = 1 - val.y;
            points_.push_back(val);

            std::sort(points_.begin(), points_.end(), [](const Vector& a, const Vector& b) { return a.x < b.x; });

            return EVENT_RES::STOP;
        }

        bool onMouseMove(plugin::MouseContext context) final {
            if (!check_hit(context.position)) { return EVENT_RES::CONT; }

            if (moving) {
                host->move(context.position - last_pos);
                last_pos = context.position;
            }

            return EVENT_RES::STOP;
        }

        bool onMouseRelease(plugin::MouseContext context) final {
            moving = false;
            return check_hit(context.position) ? EVENT_RES::STOP : EVENT_RES::CONT;
        }

        bool onKeyboardPress(plugin::KeyboardContext context) final { return EVENT_RES::CONT; }
        bool onKeyboardRelease(plugin::KeyboardContext context) final { return EVENT_RES::CONT; }
        bool onClock(uint64_t delta) { return EVENT_RES::STOP; };

	    virtual uint8_t getPriority() const { return 0; };

        void inform();

        Vector get_pos_of_point(Vector ratios, Vector pos, Vector size) {
            Vector point_pos = size;
            point_pos.x *= ratios.x;
            point_pos.y *= (1-ratios.y);
            point_pos += pos;
            return point_pos;
        }

        virtual void render(plugin::RenderTargetI* texture) {
            Vector pos  = host->getPos();
            Vector size = host->getSize();

            dynarray<Vector>& points_ = color_points_[color];

            texture->drawRect(pos, size, WINDOW_BACKGROUND_COLOR);

            // header sep
            texture->drawLineByVector(pos + Vector(0, HEADER_HEIGHT - LINE_THICKNESS), Vector(size.x, 0));

            texture->drawText(pos + Vector(2* LINE_THICKNESS, LINE_THICKNESS), "SCF params", TITLE_SIZE, Color(0,0,0));

            // borders
            texture->drawLineByVector(pos, Vector(size.x, 0));
            texture->drawLineByVector(pos+Vector(0, size.y-LINE_THICKNESS), Vector(size.x, 0));
            texture->drawLineByVector(pos+Vector(LINE_THICKNESS,0), Vector(0, size.y));
            texture->drawLineByVector(pos+Vector(size.x, 0), Vector(0, size.y));

            pos += Vector(LINE_THICKNESS, HEADER_HEIGHT);
            size -=  Vector(2*LINE_THICKNESS, HEADER_HEIGHT + LINE_THICKNESS);

            texture->drawRect(pos, size, plugin::Color{77,77,77});

            for (int i = 0; i < points_.size() - 1; ++i) {
                texture->drawRect(get_pos_of_point(points_[i], pos, size) - Vector(5, 5), Vector(10, 10), bar_colors[color]);
                texture->drawLine(get_pos_of_point(points_[i], pos, size), get_pos_of_point(points_[i+1], pos, size), bar_colors[color]);
            }

            texture->drawRect(get_pos_of_point(points_[points_.size()-1], pos, size) - Vector(5, 5), Vector(10, 10), bar_colors[color]);

            texture->display();
        }

    private:
        dynarray<Vector> color_points_[3];
        uint color = 0;
        ShittyCurveFilter* filter_;
        plugin::RenderTargetI *canvas_;
        bool moving = false;
        Vector last_pos;
    };

    struct ShittyCurveFilter: public plugin::Plugin, public plugin::FilterI {
        ShittyCurveFilter(uint64_t id_, const char *name_, plugin::InterfaceType type_) {
            id = id_;
            name = name_;
            type = type_;
        }
        
        plugin::Interface *getInterface() const final { return const_cast<ShittyCurveFilter *>(this); };
        ~ShittyCurveFilter() final = default;

        plugin::Array<const char *> getParamNames() const final { return {0, nullptr}; }
            
        plugin::Array<double> getParams() const final { return plugin::Array<double>(0, nullptr); }
        void setParams(plugin::Array<double> params) final {}

        void apply(plugin::RenderTargetI *data) final;
        void finally_draw(plugin::RenderTargetI *data);

        void selectPlugin() final {}

        void set_points(const dynarray<Vector>& red_points, const dynarray<Vector>& green_points, const dynarray<Vector>& blue_points) {
            red_points_ = red_points;
            green_points_ = green_points;
            blue_points_ = blue_points;
        }

        void set_gui_ptr(plugin::GuiI *gui) {
            gui_ = gui;
        }

    private:
        dynarray<Vector> red_points_;
        dynarray<Vector> green_points_;
        dynarray<Vector> blue_points_;
        plugin::GuiI *gui_;
    };

    void ShittyCurveFilter::apply(plugin::RenderTargetI *data) {
        auto widget = new CurveSetWidget(this, data);
        gui_->createWidgetI(widget);

        widget->host->setPos (Vector(200, 200));
        widget->host->setSize(Vector(300, 300));

        gui_->getRoot()->registerSubWidget(widget->host);
    }

    template <uint8_t plugin::Color::*color>
    double update_pixel_color(const dynarray<Vector>& points, plugin::Color pixel) {
        double val = pixel.*color;
        val /= 255;

        int i;
        for (i = 0; i < points.size(); ++i) {
            if (points[i].x > val) {
                break;
            }
        }

        Vector end_point = (i < points.size()) ? points[i] : Vector(1, 1);
        Vector start_point = (i > 0) ? points[i-1] : Vector(0, 0);

        double target = start_point.y + (end_point.y - start_point.y) * (val - start_point.x) / (end_point.x - start_point.x);
        double ratio = (val > 0.01) ? (target / val) : 1;
        
        return std::min(255, (int)(pixel.*color * ratio));
    }

    void ShittyCurveFilter::finally_draw(plugin::RenderTargetI *data) {
        plugin::Texture *image = data->getTexture();

        const double height = image->_height;
        const double width  = image->_width;

        for (uint64_t row = 0; row < height; ++row) {
            for (uint64_t col = 0; col < width; ++col) {
                plugin::Color pixel = image[0][row][col];
               
                image[0][row][col].r = update_pixel_color<&plugin::Color::r>(red_points_, pixel);
                image[0][row][col].g = update_pixel_color<&plugin::Color::g>(green_points_, pixel);
                image[0][row][col].b = update_pixel_color<&plugin::Color::b>(blue_points_, pixel);
            }
        }

        data->drawTexture(Vector(0,0), image->size(), image);
        delete image;
    }

    void CurveSetWidget::inform() {
        filter_->set_points(color_points_[0], color_points_[1], color_points_[2]);
        filter_->finally_draw(canvas_);
    }
}

static ShittyCurveFilter plugin_obj(0xAB0BA | 0x434, "SCF", plugin::InterfaceType::Filter); /* Shitty Curve Filter */

extern "C" plugin::Plugin* getInstance(plugin::App *app) {
    plugin_obj.set_gui_ptr(app->root);
    return &plugin_obj;
}