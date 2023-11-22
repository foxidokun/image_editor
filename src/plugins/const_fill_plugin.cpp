#include "plugin.h"
#include <iostream>

static const plugin::Color choosen_color{133, 133, 0};

struct ConstFillFilter: public plugin::Plugin, public plugin::FilterI {
    ConstFillFilter(uint64_t id_, const char *name_, plugin::InterfaceType type_) {
        id = id_;
        name = name_;
        type = type_;
    }
    
    plugin::Interface *getInterface() final { return this; };
    ~ConstFillFilter() final = default;


    plugin::Array<const char *> getParamNames() final { return {0, nullptr}; }
        
    plugin::Array<double> getParams() final { return {0, nullptr}; }
    void setParams(plugin::Array<double> params) final { }

    void apply(plugin::RenderTargetI *data) final;
};

static ConstFillFilter plugin_obj(0xAB0BA, "Const Fill", plugin::InterfaceType::Filter);

extern "C" plugin::Plugin* getInstance(plugin::App *app) {
    return &plugin_obj;
}

void ConstFillFilter::apply(plugin::RenderTargetI *data) {
    plugin::Texture *texture = data->getTexture();

    for (int i = 0; i < texture->height(); ++i) {
        for (int j = 0; j < texture->width(); ++j) {
            (*texture)[i][j] = choosen_color;
        }
    }

    data->drawTexture(Vector(0,0), texture->size(), texture);

    delete texture;
}