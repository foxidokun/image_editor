#include "plugin.h"
#include <iostream>
#include <string>
#include <cmath>

namespace {
    struct SphereFilter: public plugin::Plugin, public plugin::FilterI {
        SphereFilter(uint64_t id_, const char *name_, plugin::InterfaceType type_) {
            id = id_;
            name = name_;
            type = type_;
        }
        
        plugin::Interface *getInterface() const final { return const_cast<SphereFilter *>(this); };
        ~SphereFilter() final = default;

        plugin::Array<const char *> getParamNames() const final { return {0, nullptr}; }
            
        plugin::Array<double> getParams() const final { return plugin::Array<double>(0, nullptr); }
        void setParams(plugin::Array<double> params) final {}

        void apply(plugin::RenderTargetI *data) final;

        void selectPlugin() final {/* I don't care */}
    };


    void SphereFilter::apply(plugin::RenderTargetI *data) {
        const plugin::Texture *image = data->getTexture();
        plugin::Texture new_texture = plugin::Texture(image->width(), image->height());
        std::fill(new_texture._pixels, new_texture._pixels + image->_width * image->_height, plugin::Color(0,0,0,255));

        // TODO make 1:1
        const double height = image->_height;
        const double width  = image->_width;
        const double max_radius = std::min(height, width) / 2;

        for (uint64_t row = 0; row < height; ++row) {
            double radius = max_radius * row / height;

            for (uint64_t col = 0; col < width; ++col) {
                double angle = 2 * std::numbers::pi * ((double)col) / width - std::numbers::pi / 2;
                double pos_x = width/2  + radius * cos(angle);
                double pos_y = height/2 + radius * sin(angle);

                new_texture[(uint64_t)pos_y][(uint64_t)pos_x] = image[0][row][col];
            }
        }

        data->drawTexture(Vector(0,0), new_texture.size(), &new_texture);
        delete image;
    }
}

static SphereFilter plugin_obj(0xAB0BA, "Sphere", plugin::InterfaceType::Filter);

extern "C" plugin::Plugin* getInstance(plugin::App *app) {
    return &plugin_obj;
}