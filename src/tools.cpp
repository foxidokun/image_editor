#include "tools.h"

void Brush::paint(sf::RenderTexture& canvas, const click_info_t& click_info, const Color& color) {
    sf::CircleShape circle;
    circle.setRadius(_radius);
    circle.setFillColor(convert_color(color));

    Vector step = (click_info.pos - click_info.last_pos);
    double step_length = step.length();
    double num_of_steps = step_length / _radius * 5;
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

    Vector dist = (click_info.pos - click_info.start_pos);
    double dist_length = dist.length();

    uint num_of_steps = (uint) floor(dist_length /(_radius * 3));

    if (num_of_steps == 0) {
        return;
    }

    Vector step = dist.norm() * (3 * _radius);

    for (uint i = 0; i < num_of_steps; ++i) {
        Point draw_point = click_info.start_pos + step * i;
        circle.setPosition(draw_point.x, draw_point.y);
        canvas.draw(circle);
    }
}