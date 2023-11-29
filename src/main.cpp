#include "types.h"
#include "config.h"
#include <chrono>
#include <thread>
#include <unistd.h> 
#include <QApplication>
#include <QColorDialog>
#include <dlfcn.h>
#include <filesystem>

static void set_color(CallbackArgs *_args);
static void load_plugin(const char * path, WindowManager& win_mgr, EventManager& event_mgr, ToolManager& tool_mgr, FilterManager& flt_mgr, Menu& filter_menu, Window& tool_window);
static void load_plugins( WindowManager& win_mgr, EventManager& event_mgr, ToolManager& tool_mgr, FilterManager& flt_mgr);

namespace chrono = std::chrono;
static EVENT_RES event_dispatcher(const sf::Event& event, sf::RenderWindow& window, EventManager& win_mgr);
static void test_regions();
static void setup_objects(WindowManager& win_mgr, ToolManager *tools, FilterManager &filter_mgr, EventManager& event_mgr);

int main(int argc, char **argv) {
    QApplication app(argc, argv); // for color picker
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), WINDOW_TITLE);
    auto frame_start_time = chrono::system_clock::now();

    WindowManager WM(WINDOW_WIDTH, WINDOW_HEIGHT);
    ToolManager TM;
    EventManager EM;
    FilterManager FM;
    EventLogger event_logger(std::cout);
    EM.register_object(&WM);
    EM.register_object(&event_logger);
    setup_objects(WM, &TM, FM, EM);

    RenderTarget target(Vector(WINDOW_WIDTH, WINDOW_HEIGHT));

    window.clear(sf::Color::Cyan);
    target.clear(sf::Color::Cyan);

    while (window.isOpen()) {
        frame_start_time = chrono::system_clock::now();
        WM.render(target);
        target.display(window);
        window.display();

        sf::Event event;
        while (window.pollEvent(event)) {
            EVENT_RES res = event_dispatcher(event, window, EM);
            if (res == EVENT_RES::STOP) {
                app.quit();
                return 0;
            }
        }
        EM.timer_event();

        std::this_thread::sleep_until(frame_start_time + chrono::milliseconds(33));
    }
}

// ---------------------------------------------------------------------------------------------------------------------
// Event managment
// ---------------------------------------------------------------------------------------------------------------------

static inline mouse_event_t get_mouse_event(const sf::RenderWindow& window, const sf::Event& event) {
    sf::Vector2f point;
    if (event.type == sf::Event::MouseButtonPressed || event.type == sf::Event::MouseButtonReleased) {
        point = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
    } else {
        assert(event.type == sf::Event::MouseMoved);
        point = window.mapPixelToCoords(sf::Vector2i(event.mouseMove.x, event.mouseMove.y));
    }

    mouse_event_t mouse_event;
    mouse_event.position = (Vector)point;

    if (event.type == sf::Event::MouseMoved) {
        mouse_event.button = plugin::MouseButton::Right;
    } else if (event.mouseButton.button == sf::Mouse::Button::Left) {
        mouse_event.button = plugin::MouseButton::Left;
    } else if (event.mouseButton.button == sf::Mouse::Button::Right) {
        mouse_event.button = plugin::MouseButton::Right;
    } else {
        mouse_event.button = plugin::MouseButton::Right;
    }

    return mouse_event;
} 

static EVENT_RES event_dispatcher(const sf::Event& event, sf::RenderWindow& window, EventManager& event_mgr) {
    switch (event.type) {
        case (sf::Event::Closed):
            window.close();
            return EVENT_RES::STOP;
        
        case (sf::Event::MouseButtonPressed):
            event_mgr.onMousePress(get_mouse_event(window, event));
            return EVENT_RES::CONT;

        case (sf::Event::MouseButtonReleased):
            event_mgr.onMouseRelease(get_mouse_event(window, event));
            return EVENT_RES::CONT;

        case (sf::Event::MouseMoved):
            event_mgr.onMouseMove(get_mouse_event(window, event));
            return EVENT_RES::CONT;

        case (sf::Event::KeyPressed):
            event_mgr.onKeyboardPress(event.key);
            return EVENT_RES::CONT;

        case (sf::Event::KeyReleased):
            event_mgr.onKeyboardRelease(event.key);
            return EVENT_RES::CONT;

        default:
            return EVENT_RES::CONT;
    }
}

// ---------------------------------------------------------------------------------------------------------------------
// Test suite
// ---------------------------------------------------------------------------------------------------------------------

#include "regions.h"

static void test_regions() {
    Rectangle a1{0, 0, 3, 3};
    Rectangle a2{6, 0, 10, 4};
    Rectangle a3{2, 4, 7, 8};
    Rectangle a4{8, 7, 13, 10};

    Rectangle b1{2, 2, 7, 5};
    Rectangle b2{6, 5, 10, 8};
    Rectangle b3{8, 1, 9, 2};
    Rectangle b4{10, 8, 15,11};

    Region r1;
    Region r2;

    r1.add_rectangle(a1);
    r1.add_rectangle(a2);
    r1.add_rectangle(a3);
    r1.add_rectangle(a4);

    r2.add_rectangle(b1);
    r2.add_rectangle(b2);
    r2.add_rectangle(b3);
    r2.add_rectangle(b4);

    Region r3 = r1 - r2;
    std::cout << "Diff is \n" << r3; 

    Region r4 = r1 * r2;
    std::cout << "Intersection is \n" << r4; 

    Rectangle c1(0, 0, 3, 2);
    Rectangle c2(3, 0, 5, 2);
    Rectangle c3(0, 2, 2, 4);
    Rectangle c4(2, 2, 3, 3);
    Rectangle c5(3, 2, 4, 3);
    Rectangle c6(2, 3, 4, 4);
    Rectangle c7(4, 2, 5, 4);

    Region opt;
    opt.add_rectangle(c1);
    opt.add_rectangle(c2);
    opt.add_rectangle(c3);
    opt.add_rectangle(c4);
    opt.add_rectangle(c5);
    opt.add_rectangle(c6);
    opt.add_rectangle(c7);

    opt.optimize();

    std::cout << "Optimised is \n" << opt;

    Region r5;
    Region r6;
    r5.add_rectangle({0,   0, 400, 400});
    r6.add_rectangle({100, 100, 100, 100});

    r6 -= r5;

    std::cout << "r6\n" << r6 << "\n";
}


// ---------------------------------------------------------------------------------------------------------------------
// Setup
// ---------------------------------------------------------------------------------------------------------------------

static void setup_canvas_window(WindowManager& win_mgr, ToolManager *tools, FilterManager &filter_mgr);
static void setup_file_menu(WindowManager& win_mgr, Canvas* canvas);
// static void setup_filter_menu(WindowManager& win_mgr, FilterManager& filter_mgr, EventManager& event_mgr);
static void setup_color_window(WindowManager& win_mgr, ToolManager *tools);
static void setup_color_button(Window& win, ToolManager *tools, const Color& color, const Point& pos);


static void set_brush(CallbackArgs *_args);

static void set_color(CallbackArgs *_args);
static void ask_color(CallbackArgs *_args);

struct ToolArgs: public CallbackArgs {
    ToolManager *tools;
    plugin::ToolI *tool;

    ToolArgs(ToolManager *tools, plugin::ToolI *tool): tools(tools), tool(tool) {}
};

struct ColorArgs: public CallbackArgs {
    ToolManager *tools;
    Color color;

    ColorArgs(ToolManager *tools, const Color &color_ = sf::Color::Black): tools(tools),  color(color_) {}
};

static void setup_objects(WindowManager& win_mgr, ToolManager *tools, FilterManager& filter_mgr, EventManager& event_mgr) {
    setup_canvas_window(win_mgr, tools, filter_mgr);
    setup_color_window(win_mgr, tools);

    load_plugins(win_mgr, event_mgr, *tools, filter_mgr);
}

static void setup_canvas_window(WindowManager& win_mgr, ToolManager *tools, FilterManager& filter_mgr) {
    auto win      = new Window(Point(110,0), Vector(800, 600), "Canvas");
    double width  = win->active_area().high_x - win->active_area().low_x; // - 12;
    double height = win->active_area().high_y - win->active_area().low_y; // - 10;

    auto canvas = new Canvas(Point(0,0), Vector(width, height), tools, filter_mgr, Point(0,0), Vector(width, height));
    // auto h_scrollbar = new Scrollbar<Orientation::Horizontal>(Vector(0, 365), Vector(600, 10));
    // auto v_scrollbar = new Scrollbar<Orientation::Vertical>  (Vector(588, 0), Vector(12, 400));

    win->register_object(canvas);
    // win->register_object(h_scrollbar);
    // win->register_object(v_scrollbar);

    win_mgr.register_object(win);
    // auto h_controller = new ScrollController(*h_scrollbar, *canvas);
    // auto v_controller = new ScrollController(*v_scrollbar, *canvas);

    setup_file_menu(win_mgr, canvas);
}

static void setup_color_window(WindowManager& win_mgr, ToolManager *tools) {
    tools->set_color({0,0,0,255});

    auto win    = new Window(Point(0,345), Vector(100, 345), "Colors");
    setup_color_button(*win, tools, Color{255,   0,   0, 255}, Point(0,   0));
    setup_color_button(*win, tools, Color{0,   255,   0, 255}, Point(50,  0));
    setup_color_button(*win, tools, Color{0,     0, 255, 255}, Point(0,  50));
    setup_color_button(*win, tools, Color{255, 255, 255, 255}, Point(50, 50));
    setup_color_button(*win, tools, Color{0,     0,   0, 255}, Point(0, 100));
    setup_color_button(*win, tools, Color{0,   255, 255, 255}, Point(50, 100));
    setup_color_button(*win, tools, Color{0xFF,0xD7,  0, 255}, Point(0,  150));
    setup_color_button(*win, tools, Color{0x7B,0x68,0xEE,255}, Point(50, 150));

    auto color_picker = new TextureButton(Vector(25, 210), Vector(50, 50), ask_color, new ColorArgs(tools),
                                            global_resources::palette);
    auto indicator = new ColorIndicator(Vector(40, 275), Vector(20, 20), tools->get_color_ptr());
    win->register_object(color_picker);
    win->register_object(indicator);

    win_mgr.register_object(win);
}

static void setup_color_button(Window& win, ToolManager *tools, const Color& color, const Point& pos) {
    auto red_args = new ColorArgs(tools, color);

    auto color_texture = new sf::RenderTexture();
    color_texture->create(30, 30);
    color_texture->clear(color);
    sf::RectangleShape borders;
    borders.setOutlineColor(sf::Color(100,100,100,255));
    borders.setFillColor(sf::Color::Transparent);
    borders.setOutlineThickness(30);
    borders.setPosition(1, 1);
    borders.setSize(sf::Vector2f(24, 24));
    color_texture->draw(borders);
    color_texture->display();
    auto button = new TextureButton(pos + Vector(10, 10), Vector(30, 30), set_color, red_args, color_texture->getTexture());
    win.register_object(button);
}

static void setup_file_menu(WindowManager& win_mgr, Canvas* canvas) {
    auto file_menu = new Menu(Point(1,0), Vector(49, HEADER_HEIGHT), "File");

    auto load_button = new TextButton(Point(), Vector(), load_canvas_callback, new SaveLoadCanvasArgs(canvas), "Open");
    auto save_button = new TextButton(Point(), Vector(), save_canvas_callback, new SaveLoadCanvasArgs(canvas), "Save");

    file_menu->register_object(load_button);
    file_menu->register_object(save_button);
    win_mgr.register_object_exact_pos(file_menu);
}

static inline void add_filter_button(WindowManager& win_mgr, Menu& menu, FilterManager& filter_mgr, EventManager& event_mgr,
    const char* button_name, plugin::FilterI *filter)
{
    auto filter_args = new FilterApplyArgs(filter_mgr, event_mgr, &win_mgr, filter);
    auto flt_btn = new TextButton(Point(), Vector(300, 0), apply_filter_callback, filter_args, button_name);
    menu.register_object(flt_btn);
}

static void set_brush(CallbackArgs *_args) {
    ToolArgs *args = static_cast<ToolArgs *>(_args);

    args->tools->setTool(args->tool);
}

static void set_color(CallbackArgs *_args) {
    ColorArgs *args = static_cast<ColorArgs *>(_args);

    args->tools->set_color(args->color);
}

static void ask_color(CallbackArgs *_args) {
    ColorArgs *args = static_cast<ColorArgs *>(_args);

    QColor color = QColorDialog::getColor();
    if (color.isValid()) {
        args->tools->set_color(Color(color.red(), color.green(), color.blue(), color.alpha()));
    }
}

static void load_plugins(WindowManager& win_mgr, EventManager& event_mgr, ToolManager& tool_mgr, FilterManager& flt_mgr)
{
    auto filter_menu = new Menu(Point(51,0), Vector(99, HEADER_HEIGHT), "Filter");

    auto filter_args = new FilterApplyArgs(flt_mgr, event_mgr, &win_mgr);
    auto recent_button = new TextButton(Point(), Vector(), recent_filter_callback,
                                                                filter_args, "Recent");
    filter_menu->register_object(recent_button);

    plugin::ToolI *brush = new Brush();
    tool_mgr.setTool(brush);
    auto tool_win = new Window(Point(0,0), Vector(100,345), "Tools");
    auto br_btn   = new TextureButton(Point(10, 10) , Vector(30, 30), set_brush, new ToolArgs(&tool_mgr, brush), global_resources::brush);

    tool_win->register_object(br_btn);

    for (const auto& x: std::filesystem::directory_iterator("./compiled_plugins")) {
        load_plugin(x.path().c_str(), win_mgr, event_mgr, tool_mgr, flt_mgr, *filter_menu, *tool_win);
    }

    win_mgr.register_object_exact_pos(filter_menu);
    win_mgr.register_object(tool_win);
}

static void load_plugin(const char * path, WindowManager& win_mgr, EventManager& event_mgr, ToolManager& tool_mgr,
    FilterManager& flt_mgr, Menu& filter_menu, Window& tool_window)
{
    static Vector tool_pos = Vector(60, 10);

    void* handle = dlopen(path, RTLD_NOW | RTLD_LOCAL);
    if (handle) {
        void *func = dlsym(handle, "getInstance");
        if (!func) {
            std::cerr << "init func for plugin <" << path << "> is null, load aborted!\n";
            return;
        }

        auto init_func = (decltype(getInstance)*)(func);
        auto app = plugin::App{&win_mgr, &event_mgr, &tool_mgr, &flt_mgr};
        auto plugin = init_func(&app);
        std::cerr << "Got plugin " << plugin->name << " (" << plugin->id << ")\n";

        if (plugin->type == plugin::InterfaceType::Filter) {
            auto filter = static_cast<plugin::FilterI *>(plugin->getInterface());
            add_filter_button(win_mgr, filter_menu, flt_mgr, event_mgr, plugin->name, filter);
        } else if (plugin->type == plugin::InterfaceType::Tool) {
            auto tool = static_cast<plugin::ToolI *>(plugin->getInterface());
            auto texture = tool->getIcon();

            Button *tool_btn;
            if (texture) {
                tool_btn = new PluginTextureButton(tool_pos, Vector(30, 30), set_brush, new ToolArgs(&tool_mgr, tool), new plugin::Texture(*texture));
            } else { 
                tool_btn = new TextureButton(tool_pos, Vector(30, 30), set_brush, new ToolArgs(&tool_mgr, tool), global_resources::brush);
            }

            tool_window.register_object(tool_btn);
            if (fabs(tool_pos.x - 60) < 1) {
                tool_pos.x  = 10;
                tool_pos.y += 50;
            } else {
                tool_pos.x = 60;
            }
        }
    } else {
        std::cerr << "failed to load plugin <" << path << "> with error: " << dlerror() << "\n";
    }
}
