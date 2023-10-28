#pragma once

#include <cstring>
#include "rendertarget.h"
#include "button.h"
#include "window.h"

class Filter {
public:
    virtual void apply(RenderTarget &rt) = 0;
    virtual dynarray<const char *> get_param_names() = 0; 
    virtual void set_params(const std::vector<double>& params) = 0;
};

class FilterManager {
private:
    Filter *last_filter = nullptr;
    RenderTarget *active_rt = nullptr;

public:
    void set_filter(Filter* filter) { last_filter = filter; };

    Filter* get_filter() { return last_filter; };

    void set_rt(RenderTarget* rt) { active_rt = rt; };
    RenderTarget* get_rt() { return active_rt; };
};

class RaiseBrightness: public Filter {
public:
    void apply(RenderTarget &rt);
    dynarray<const char *> get_param_names() {return dynarray<const char *>();};
    void set_params(const std::vector<double>& params) {};
};


class FillFilter: public Filter {
    double colors[3] = {};
public:
    void apply(RenderTarget &rt);
    dynarray<const char *> get_param_names() {return {"Red", "Green", "Blue"};};
    void set_params(const std::vector<double>& params) {
        assert(params.size() == 3);
        colors[0] = std::max(std::min(params[0], 255.0), 0.0);
        colors[1] = std::max(std::min(params[1], 255.0), 0.0);
        colors[2] = std::max(std::min(params[2], 255.0), 0.0);
    };
};

struct FilterApplyArgs: public CallbackArgs {
    FilterManager &filter_mgr;
    Filter* filter;
    WindowManager *root;
    EventManager &event_mgr;
    ParametersModalWindow* param_window = nullptr;

    FilterApplyArgs(FilterManager &filter_mgr, EventManager& event_mgr, WindowManager* root, Filter* filter = nullptr):
        filter_mgr(filter_mgr),
        filter(filter),
        root(root),
        event_mgr(event_mgr) {}
};

void apply_filter_callback(CallbackArgs *_args);
void recent_filter_callback(CallbackArgs *_args);