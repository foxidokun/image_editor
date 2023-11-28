#include "plugin.h"
#include <iostream>

static const char *param_names[3] = {"R", "G", "B"};

struct ConstFillFilter: public plugin::Plugin, public plugin::FilterI {
    double color[3] = {133, 133, 0};

    ConstFillFilter(uint64_t id_, const char *name_, plugin::InterfaceType type_) {
        id = id_;
        name = name_;
        type = type_;
    }
    
    plugin::Interface *getInterface() final { return this; };
    ~ConstFillFilter() final = default;

    plugin::Array<const char *> getParamNames() final { return {0, nullptr}; }
        
    plugin::Array<double> getParams() final { return plugin::Array<double>(0, nullptr); }
    void setParams(plugin::Array<double> params) final {}

    void apply(plugin::RenderTargetI *data) final;
};

static ConstFillFilter plugin_obj(0xAB0BA, "Sphere", plugin::InterfaceType::Filter);

extern "C" plugin::Plugin* getInstance(plugin::App *app) {
    return &plugin_obj;
}

void ConstFillFilter::apply(plugin::RenderTargetI *data) {
}