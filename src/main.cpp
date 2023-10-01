#include "types.h"
#include "config.h"
#include <chrono>
#include <thread>

namespace chrono = std::chrono;

int main() {
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), WINDOW_TITLE);
    auto frame_start_time = chrono::system_clock::now();

    Window main_window(Point(200, 200), Vector(200, 200), string("Test"));
    RenderTarget target(Vector(WINDOW_WIDTH, WINDOW_HEIGHT));

    while (window.isOpen()) {
        window.clear();

        main_window.render(target);
        target.display(window);
        window.display();

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                return 0;
            }
        }

        std::this_thread::sleep_until(frame_start_time + std::chrono::milliseconds(33));
    }
}