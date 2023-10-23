#pragma once

#include "rendertarget.h"
#include "button.h"

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
    dynarray<const char *> get_param_names();
    void set_params(const std::vector<double>& params);
};


struct FilterApplyArgs: public CallbackArgs {
    FilterManager &filter_mgr;
    Filter* filter;

    FilterApplyArgs(FilterManager &filter_mgr, Filter* filter = nullptr): filter_mgr(filter_mgr), filter(filter) {}
};

void apply_filter_callback(CallbackArgs *_args);
void recent_filter_callback(CallbackArgs *_args);