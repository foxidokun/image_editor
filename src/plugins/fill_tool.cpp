#include "plugin.h"
#include "vector.h"
#include "list.h"
#include <iostream>

extern plugin::Texture fill_tool_texture;

namespace {
    struct FillTool: public plugin::Plugin, public plugin::ToolI {
        plugin::Texture *_texture;

        constexpr FillTool(uint64_t id_, const char *name_): _texture(&fill_tool_texture) {
            id = id_;
            name = name_;
            type = plugin::InterfaceType::Tool;
        }
        
        plugin::Interface *getInterface() final {
            return static_cast<plugin::Interface *>(this);
        };

        ~FillTool() final = default;

        plugin::Array<const char *> getParamNames() final { return {0, nullptr}; }
        plugin::Array<double> getParams() final { return plugin::Array<double>(0, nullptr); }
        void setParams(plugin::Array<double> params) final {}

        void paintOnPress(plugin::RenderTargetI *data, plugin::RenderTargetI *tmp, plugin::MouseContext context, plugin::Color color) final {
            tmp->clear();

            Vector pos = context.position;
            auto img = data->getTexture();
            plugin::Color ref_color = img->get_pixel(pos.x, pos.y);
            
            linked_list<Vector> queue;
            queue.push_back(pos);
            while (queue.size() > 0) {
                Vector cur_pos = queue.front();
                queue.pop_front();

                if (cur_pos.x < 0 || cur_pos.x + 1 > img->width()) {
                    continue;
                }

                if (cur_pos.y < 0 || cur_pos.y + 1 > img->height()) {
                    continue;
                }

                if (img->get_pixel(cur_pos.x, cur_pos.y) == ref_color) {
                    img->set_pixel(cur_pos.x, cur_pos.y, color);
                    Vector diff = cur_pos - pos;
                    double diff_length = diff.length_square();
                    for (int i = -1; i <= 1; ++i) {
                        for (int j = -1; j <= 1; ++j) {
                            if ((diff+Vector(i, j)).length_square() > diff_length + 0.1) {
                                queue.push_back(cur_pos + Vector(i, j));
                            }
                        }
                    }
                }
            }

            data->drawTexture(Vector(0,0), img->size(), img);

            delete img;
        }
        
        void paintOnRelease(plugin::RenderTargetI *, plugin::RenderTargetI *, plugin::MouseContext, plugin::Color) final {}
        void paintOnMove(plugin::RenderTargetI *, plugin::RenderTargetI *, plugin::MouseContext, plugin::Color) final {}
        void disable(plugin::RenderTargetI *,plugin::RenderTargetI *, plugin::MouseContext, plugin::Color) final {}

        const plugin::Texture *getIcon() final {
            return _texture;
        };
    };
}

static FillTool plugin_obj(0xAB0BA << 1, "Fill");

extern "C"
plugin::Plugin* getInstance(plugin::App *app) {
    return static_cast<plugin::Plugin *>(&plugin_obj);
}