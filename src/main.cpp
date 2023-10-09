#include "types.h"
#include "config.h"
#include <chrono>
#include <thread>

namespace chrono = std::chrono;
static EVENT_RES event_dispatcher(const sf::Event& event, sf::RenderWindow& window, WindowManager& wm);
static void test_regions();

int main() {
    test_regions();

    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), WINDOW_TITLE);
    auto frame_start_time = chrono::system_clock::now();

    WindowManager WM(WINDOW_WIDTH, WINDOW_HEIGHT);
    auto main_window    = new Window(Point(200, 200), Vector(300, 300), string("Test"));
    auto another_window = new Window(Point(600, 200), Vector(200, 200), string("aboba"));
    auto child_window   = new Window(Point(100, 100), Vector(100, 100), string("Child"));
    main_window->register_object(child_window);
    WM.register_object(main_window);
    WM.register_object(another_window);

    RenderTarget target(Vector(WINDOW_WIDTH, WINDOW_HEIGHT));

    while (window.isOpen()) {
        window.clear(sf::Color::Cyan);
        target.clear(sf::Color::Cyan);

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
}
