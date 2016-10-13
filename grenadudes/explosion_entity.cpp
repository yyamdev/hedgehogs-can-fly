#include "explosion_entity.h"
#include "terrain_entity.h"

EntityExplosion::EntityExplosion() {
    EntityExplosion(sf::Vector2f(0.f, 0.f));
}

EntityExplosion::EntityExplosion(sf::Vector2f pos) {
    position = pos;
    tag = "explosion";
    collisionRadius = EXPLOSION_RADIUS;
    explosionTime.restart();
    justCreated = true;
}

void EntityExplosion::event(sf::Event &e) {
}

void EntityExplosion::tick(std::vector<Entity*> &entities) {
    if (justCreated) { // remove circular reigion of terrain at start
        justCreated = false;
        EntityTerrain *terrain = NULL;
        for (Entity *e : entities) {
            if (e->get_tag() == "terrain") {
                terrain = (EntityTerrain*)e;
            }
        }
        terrain->set_circle(position, EXPLOSION_DEFORMATION_RADIUS, false);
    }

    for (Entity *e : entities) {
        if (intersects(*e)) {
            e->damage(50);
        }
    }

    if (explosionTime.getElapsedTime().asMilliseconds() > EXPLOSION_MILLISECONDS_ALIVE)
        remove = true;
}

void EntityExplosion::draw(sf::RenderWindow &window) {
    sf::CircleShape circle(collisionRadius);
    circle.setPosition(position);
    circle.setOrigin(sf::Vector2f(circle.getRadius(), circle.getRadius()));
    circle.setFillColor(sf::Color::White);
    window.draw(circle);
}