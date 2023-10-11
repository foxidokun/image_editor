#include "tools.h"

void Brush::paint(sf::RenderTexture& canvas, const click_info_t& click_info, const Color& color) {
    sf::CircleShape circle;
    circle.setRadius(_radius);
    circle.setFillColor(convert_color(color));

    Vector step = (click_info.pos - click_info.last_pos);
    double step_length = step.length();
    double num_of_steps = step_length / BRUSH_RADIUS * 5;
    num_of_steps = std::max(num_of_steps, 1.0);
    step = step / num_of_steps;

    for (uint i = 0; i < (uint)num_of_steps; ++i) {
        Point draw_point = click_info.last_pos + step * i;
        circle.setPosition(draw_point.x, draw_point.y);
        canvas.draw(circle);
    }
}

void AlienBrush::paint(sf::RenderTexture& canvas, const click_info_t& click_info, const Color& color) {
    sf::CircleShape circle;
    circle.setRadius(_radius);
    circle.setFillColor(convert_color(color));

    Vector step = (click_info.pos - click_info.last_pos);
    double step_length = step.length();
    double num_of_steps = step_length / BRUSH_RADIUS * 5;
    num_of_steps = std::max(num_of_steps, 1.0);
    step = step / num_of_steps;

    for (uint i = 0; i < (uint)num_of_steps; ++i) {
        Point draw_point = click_info.start_pos + step * i;
        circle.setPosition(draw_point.x, draw_point.y);
        canvas.draw(circle);
    }
}