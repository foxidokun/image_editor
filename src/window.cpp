#include "window.h"

void Window::render(RenderTarget& target, const Point& start_pos) const {
    target.drawRect(_pos + start_pos, _size);

    target.drawLine(_pos + start_pos + Point(0, HEADER_HEIGHT), {_size.x, 0});
    target.drawText(_pos + start_pos +  Point(LINE_THICKNESS, LINE_THICKNESS), _title.c_str(), TITLE_SIZE);
}