#include "shared_res.h"

sf::Font fntUi;

sf::Texture txtStart;
sf::Texture txtBye;

void load_shared_res()
{
    fntUi.loadFromFile("data/square-deal.ttf");
    txtStart.loadFromFile("data/start.png");
    txtBye.loadFromFile("data/bye.png");
}
