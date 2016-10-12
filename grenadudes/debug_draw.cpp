#include "debug_draw.h"
#include "util.h"

void draw_vector(const sf::Vector2f &pos, const sf::Vector2f &dir, float len, const sf::Color &col, sf::RenderWindow &window) {
    float inputLen = util::distance(0.f, 0.f, dir.x, dir.y);
    sf::Vector2f dirScaled = dir / inputLen * len;
    sf::Vertex line[] =
    {
        sf::Vertex(pos, col),
        sf::Vertex(pos + dirScaled, col )
    };
    window.draw(line, 2, sf::Lines);
}