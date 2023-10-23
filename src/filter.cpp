#include "filter.h"

void RaiseBrightness::apply(RenderTarget &rt) {
    RawImage img = rt.get_image();

    for (uint i = 0; i < img.height(); ++i) {
        for (uint j = 0; j < img.width(); ++j) {
            Color pixel_color = img[i][j];
            pixel_color.r = std::min((double)pixel_color.r + 30, 255.0);
            pixel_color.g = std::min((double)pixel_color.g + 30, 255.0);
            pixel_color.b = std::min((double)pixel_color.b + 30, 255.0);

            img[i][j] = pixel_color;
        }
    }

    rt.set_image(img);
}

dynarray<const char *> RaiseBrightness::get_param_names() {
    return dynarray<const char *>();
}

void RaiseBrightness::set_params(const std::vector<double>& params) {} 

void apply_filter_callback(CallbackArgs *_args) {
    FilterApplyArgs* args = static_cast<FilterApplyArgs *> (_args);
    
    args->filter_mgr.set_filter(args->filter);
    RenderTarget *rt = args->filter_mgr.get_rt();
    if (rt) {
        args->filter->apply(*rt);
    }
}

void recent_filter_callback(CallbackArgs *_args) {
    FilterApplyArgs* args = static_cast<FilterApplyArgs *> (_args);

    Filter *filter = args->filter_mgr.get_filter();
    RenderTarget *rt = args->filter_mgr.get_rt();

    if (filter && rt) {
        filter->apply(*rt);
    }
}