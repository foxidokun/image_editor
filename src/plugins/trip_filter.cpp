#include "plugin.h"
#include "event.h"
#include <iostream>
#include <string>
#include <cmath>

namespace {
    class ShittyCurveFilter;

    class CurveSetWidget: public plugin::PluginWidgetI {
    public:
        CurveSetWidget(ShittyCurveFilter* filter, plugin::RenderTargetI *canvas): filter_(filter), canvas_(canvas) {}

        bool check_hit(const Vector& pos) {
            bool hit_x = pos.x > host->getPos().x && pos.x < host->getPos().x + host->getSize().x;
            bool hit_y = pos.y > host->getPos().y && pos.y < host->getPos().y + host->getSize().y;

            return hit_x && hit_y;
        }

        bool onMousePress(plugin::MouseContext context) final {
            if (!check_hit(context.position)) { return EVENT_RES::CONT; }

            if (context.button == plugin::MouseButton::Right) {
                inform();
                host->setAvailable(false);
                return EVENT_RES::STOP;
            }

            Vector val = context.position - host->getPos();;
            val.x /= host->getSize().x;
            val.y /= host->getSize().y;
            val.y = 1 - val.y;
            points_.push_back(val);

            std::sort(points_.begin(), points_.end(), [](const Vector& a, const Vector& b) { return a.x < b.x; });

            return EVENT_RES::STOP;
        }

        bool onMouseMove(plugin::MouseContext context) final { return check_hit(context.position) ? EVENT_RES::STOP : EVENT_RES::CONT; }
        bool onMouseRelease(plugin::MouseContext context) final { return check_hit(context.position) ? EVENT_RES::STOP : EVENT_RES::CONT; }
        bool onKeyboardPress(plugin::KeyboardContext context) final { return EVENT_RES::CONT; }
        bool onKeyboardRelease(plugin::KeyboardContext context) final { return EVENT_RES::CONT; }
        bool onClock(uint64_t delta) { return EVENT_RES::STOP; };

	    virtual uint8_t getPriority() const { return 0; };

        void inform();

        Vector get_pos_of_point(Vector ratios) {
            Vector point_pos = host->getSize();
            point_pos.x *= ratios.x;
            point_pos.y *= (1-ratios.y);
            point_pos += host->getPos();
            return point_pos;
        }

        virtual void render(plugin::RenderTargetI* texture) {
            Vector pos  = host->getPos();
            Vector size = host->getSize();

            texture->drawRect(pos, size, plugin::Color{255,0,0});

            if (points_.size() == 0) {
                texture->drawLine(get_pos_of_point(Vector(0, 0)), get_pos_of_point(Vector(1, 1)), {0, 255, 0});
            } else {
                texture->drawLine(get_pos_of_point(Vector(0, 0)), get_pos_of_point(points_[0]), {0, 255, 0});
                texture->drawLine(get_pos_of_point(points_[points_.size() - 1]), get_pos_of_point(Vector(1, 1)), {0, 255, 0});

                for (int i = 0; i < points_.size() - 1; ++i) {
                    texture->drawLine(get_pos_of_point(points_[i]), get_pos_of_point(points_[i+1]), {0, 255, 0});
                }
            }

            texture->display();
        }

    private:
        dynarray<Vector> points_;
        ShittyCurveFilter* filter_;
        plugin::RenderTargetI *canvas_;
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

        void selectPlugin() final { std::cerr << "selectPlugin & apply flow is the worst thing ever happened to me, so i'll ignore it\n\t\tLove Yan and his ideas :3\n"; }

        void set_points(const dynarray<Vector>& points) {
            points_ = points;
        }

        void set_gui_ptr(plugin::GuiI *gui) {
            gui_ = gui;
        }

    private:
        dynarray<Vector> points_;
        plugin::GuiI *gui_;
    };

    void ShittyCurveFilter::apply(plugin::RenderTargetI *data) {
        auto widget = new CurveSetWidget(this, data);
        gui_->createWidgetI(widget);

        widget->host->setPos (Vector(200, 200));
        widget->host->setSize(Vector(300, 300));

        gui_->getRoot()->registerSubWidget(widget->host);
    }

    void ShittyCurveFilter::finally_draw(plugin::RenderTargetI *data) {
        plugin::Texture *image = data->getTexture();

        // TODO make 1:1
        const double height = image->_height;
        const double width  = image->_width;

        for (uint64_t row = 0; row < height; ++row) {
            for (uint64_t col = 0; col < width; ++col) {
                plugin::Color pixel = image[0][row][col];
                double val = pixel.r + pixel.g + pixel.b;
                val /= 255*3;

                int i;
                for (i = 0; i < points_.size(); ++i) {
                    if (points_[i].x > val) {
                        break;
                    }
                }

                Vector end_point = (i < points_.size()) ? points_[i] : Vector(1, 1);
                Vector start_point = (i > 0) ? points_[i-1] : Vector(0, 0);

                double target = start_point.y + (end_point.y - start_point.y) * (val - start_point.x) / (end_point.x - start_point.x);

                double ratio = (val > 0.01) ? (target / val) : 1;

                pixel.r = std::min(255, (int)(pixel.r * ratio));
                pixel.g = std::min(255, (int)(pixel.g * ratio));
                pixel.b = std::min(255, (int)(pixel.b * ratio));

                image[0][row][col] = pixel;
            }
        }

        data->drawTexture(Vector(0,0), image->size(), image);
        delete image;
    }

    void CurveSetWidget::inform() {
        filter_->set_points(points_);
        filter_->finally_draw(canvas_);
    }
}

static ShittyCurveFilter plugin_obj(0xAB0BA | 0x434, "SCF", plugin::InterfaceType::Filter); /* Shitty Curve Filter */

extern "C" plugin::Plugin* getInstance(plugin::App *app) {
    plugin_obj.set_gui_ptr(app->root);
    return &plugin_obj;
}