#include "window.h"

void Window::render(RenderTarget& target) const {
    target.drawRect(_pos, _size, {255, 255, 255, 255});
}