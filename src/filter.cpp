#include "filter.h"
/*
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

void FillFilter::apply(RenderTarget &rt) {
    RawImage img = rt.get_image();

    for (uint i = 0; i < img.height(); ++i) {
        for (uint j = 0; j < img.width(); ++j) {
            Color pixel_color = img[i][j];
            pixel_color.r = colors[0];
            pixel_color.g = colors[1];
            pixel_color.b = colors[2];

            img[i][j] = pixel_color;
        }
    }

    rt.set_image(img);
}
*/
void parameters_get_and_apply(CallbackArgs *_args) {
    FilterApplyArgs* args = static_cast<FilterApplyArgs *> (_args);

    dynarray<string> str_params = args->param_window->get_params();
    dynarray<double> params;
    for (const auto& str_param: str_params) {
        try {
            params.push_back(std::stod(str_param));
        } catch (std::invalid_argument) {
            return;
        }
    }

    args->filter_mgr.setFilter(args->filter);
    args->filter->setParams(params);
    auto rt = args->filter_mgr.get_rt();
    if (rt) {
        args->filter->apply(rt);
    }
}


void apply_filter_callback(CallbackArgs *_args) {
    FilterApplyArgs* args = static_cast<FilterApplyArgs *>(_args);
    
    if (args->plugin) {
        args->plugin->selectPlugin();
    }

    if (args->filter->getParamNames().size > 0) {
        auto params = new ParametersModalWindow(Vector(200, 200), Vector(300, 300), "Filter Params", args->event_mgr, parameters_get_and_apply, new FilterApplyArgs(*args), args->filter->getParamNames());
        args->param_window = params;
        args->root->register_object(params);
    } else {
        args->filter_mgr.setFilter(args->filter);
        plugin::RenderTargetI *rt = args->filter_mgr.get_rt();
        if (rt) {
            args->filter->apply(rt);
        }
    }
}

void recent_filter_callback(CallbackArgs *_args) {
    FilterApplyArgs* args = static_cast<FilterApplyArgs *> (_args);

    plugin::FilterI *filter = args->filter_mgr.get_filter();
    plugin::RenderTargetI *rt = args->filter_mgr.get_rt();

    if (filter && rt) {
        filter->apply(rt);
    }
}
