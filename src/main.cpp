#include "types.h"
#include "config.h"
#include <chrono>
#include <thread>
#include <unistd.h> 

namespace chrono = std::chrono;
static EVENT_RES event_dispatcher(const sf::Event& event, sf::RenderWindow& window, WindowManager& wm);
static void test_regions();
static void setup_objects(WindowManager& wm, ToolManager *tools);



int main() {
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), WINDOW_TITLE);
    auto frame_start_time = chrono::system_clock::now();

    WindowManager WM(WINDOW_WIDTH, WINDOW_HEIGHT);
    ToolManager TM;
    setup_objects(WM, &TM);

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
            EVENT_RES res = event_dispatcher(event, window, WM);
            if (res == EVENT_RES::STOP) {
                return 0;
            }
        }

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
    mouse_event.x = point.x;
    mouse_event.y = point.y;

    if (event.type == sf::Event::MouseMoved) {
        mouse_event.button = mouse_event_t::button_type::UNKNOWN;
    } else if (event.mouseButton.button == sf::Mouse::Button::Left) {
        mouse_event.button = mouse_event_t::button_type::LEFT;
    } else if (event.mouseButton.button == sf::Mouse::Button::Right) {
        mouse_event.button = mouse_event_t::button_type::RIGHT;
    } else {
        mouse_event.button = mouse_event_t::button_type::UNKNOWN;
    }

    return mouse_event;
} 

static EVENT_RES event_dispatcher(const sf::Event& event, sf::RenderWindow& window, WindowManager& wm) {
    switch (event.type) {
        case (sf::Event::Closed):
            window.close();
            return EVENT_RES::STOP;
        
        case (sf::Event::MouseButtonPressed):
            wm.on_mouse_press(get_mouse_event(window, event));
            return EVENT_RES::CONT;

        case (sf::Event::MouseButtonReleased):
            wm.on_mouse_release(get_mouse_event(window, event));
            return EVENT_RES::CONT;

        case (sf::Event::MouseMoved):
            wm.on_mouse_move(get_mouse_event(window, event));
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

static void setup_canvas_window(WindowManager& wm, const ToolManager *tools);
static void setup_tool_window(WindowManager& wm, ToolManager *tools);
static void setup_color_window(WindowManager& wm, ToolManager *tools);
static void setup_color_button(Window& win, ToolManager *tools, const Color& color, const Point& pos);

template<typename BrushType>
static void set_brush(CallbackArgs *_args);
static void set_color(CallbackArgs *_args);

struct ToolArgs: public CallbackArgs {
    ToolManager *tools;

    ToolArgs(ToolManager *tools): tools(tools) {}
};

struct ColorArgs: public CallbackArgs {
    ToolManager *tools;
    Color color;

    ColorArgs(ToolManager *tools, const Color &color_): tools(tools),  color(color_) {}
};

static void setup_objects(WindowManager& wm, ToolManager *tools) {
    setup_canvas_window(wm, tools);
    setup_color_window(wm, tools);
    setup_tool_window(wm, tools);
}

static void setup_canvas_window(WindowManager& wm, const ToolManager *tools) {
    auto win      = new Window(Point(0,0), Vector(600, 600), "Canvas");
    double width  = win->active_area().high_x - win->active_area().low_x;
    double height = win->active_area().high_y - win->active_area().low_y;

    // auto subwindow      = new Window(Point(240,240), Vector(200, 200), "SubWindow");

    auto canvas = new Canvas(Point(0,0), Vector(width, height), tools);
    win->register_object(canvas);
    wm.register_object(win);
}

static void setup_tool_window(WindowManager& wm, ToolManager *tools) {
    tools->set_tool(new Brush());

    auto win     = new Window(Point(850,0), Vector(200,400), "Tools");
    auto br_btn  = new TextureButton(Point(0,   0), Vector(50, 50), set_brush<Brush>,      new ToolArgs(tools), brush_tool);
    auto al_btn  = new TextureButton(Point(50,  0), Vector(50, 50), set_brush<AlienBrush>, new ToolArgs(tools), alien_brush_tool);
    auto pol_btn = new TextureButton(Point(100, 0), Vector(50, 50), set_brush<Polyline>,   new ToolArgs(tools), polyline_tool);
    auto rec_btn = new TextureButton(Point(150, 0), Vector(50, 50), set_brush<RectTool>,   new ToolArgs(tools), rectangle_tool);
    auto ell_btn = new TextureButton(Point(0,  50), Vector(50, 50), set_brush<EllipseTool>,new ToolArgs(tools), rectangle_tool);

    win->register_object(br_btn);
    win->register_object(al_btn);
    win->register_object(pol_btn);
    win->register_object(rec_btn);
    win->register_object(ell_btn);
    wm.register_object(win);
}

static void setup_color_window(WindowManager& wm, ToolManager *tools) {
    tools->set_color({0,0,0,255});


    auto win    = new Window(Point(850,410), Vector(200, 400), "Colors");
    setup_color_button(*win, tools, Color{255,   0,   0, 255}, Point(  0,  0));
    setup_color_button(*win, tools, Color{0,   255,   0, 255}, Point( 50,  0));
    setup_color_button(*win, tools, Color{0,     0, 255, 255}, Point(100,  0));
    setup_color_button(*win, tools, Color{255, 255, 255, 255}, Point(150,  0));
    setup_color_button(*win, tools, Color{0,     0,   0, 255}, Point(  0, 50));
    wm.register_object(win);
}

static void setup_color_button(Window& win, ToolManager *tools, const Color& color, const Point& pos) {
    auto red_args = new ColorArgs(tools, color);

    auto color_texture = new sf::RenderTexture();
    color_texture->create(30, 30);
    color_texture->clear(convert_color(color));
    sf::RectangleShape borders;
    borders.setOutlineColor(sf::Color(0,0,0,100));
    borders.setFillColor(sf::Color::Transparent);
    borders.setOutlineThickness(30);
    borders.setPosition(1, 1);
    borders.setSize(sf::Vector2f(24, 24));
    color_texture->draw(borders);
    color_texture->display();
    auto button = new TextureButton(pos + Vector(10, 10), Vector(30, 30), set_color, red_args, color_texture->getTexture());
    win.register_object(button);
}

template<typename BrushType>
static void set_brush(CallbackArgs *_args) {
    ToolArgs *args = static_cast<ToolArgs *>(_args);

    args->tools->set_tool(new BrushType());
}

static void set_color(CallbackArgs *_args) {
    ColorArgs *args = static_cast<ColorArgs *>(_args);

    args->tools->set_color(args->color);
}