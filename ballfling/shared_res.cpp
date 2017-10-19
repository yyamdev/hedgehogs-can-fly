#include "shared_res.h"

sf::Font fntUi;
sf::Texture txtStart;

void load_shared_res() {
    fntUi.loadFromFile("data/square-deal.ttf");
    txtStart.loadFromFile("data/start.png");
}
