#include "types.h"
#include "config.h"
#include <chrono>
#include <thread>

namespace chrono = std::chrono;
static EVENT_RES event_dispatcher(const sf::Event& event, sf::RenderWindow& window, WindowManager& wm);

int main() {
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), WINDOW_TITLE);
    auto frame_start_time = chrono::system_clock::now();

    WindowManager WM(WINDOW_WIDTH, WINDOW_HEIGHT);
    auto main_window = new Window(&WM, Point(200, 200), Vector(200, 200), string("Test"));
    WM.register_object(main_window);

    RenderTarget target(Vector(WINDOW_WIDTH, WINDOW_HEIGHT));

    while (window.isOpen()) {
        window.clear(sf::Color::White);
        target.clear(sf::Color::White);

        WM.render(target, {0, 0});
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
    sf::Vector2f point = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
    mouse_event_t mouse_event;
    mouse_event.x = point.x;
    mouse_event.y = point.y;

    if (event.mouseButton.button == sf::Mouse::Button::Left) {
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
            wm.on_mouse_press(get_mouse_event(window, event));
            return EVENT_RES::CONT;

        default:
            return EVENT_RES::CONT;
    }
}