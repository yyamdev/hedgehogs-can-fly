#include "spawner_entity.h"
#include "util.h"
#include "weapons.h"
#include "weapon_entity.h"
#include "world.h"
#include "build_options.h"

EntitySpawner::EntitySpawner() {
    tag = "spawner";
    spawnTimer.restart();
    nextSpawn = get_next_spawn_time();
}

float EntitySpawner::get_next_spawn_time() {
    return util::rnd(8.f, 15.f);
}

void EntitySpawner::event(sf::Event &e) {
}

void EntitySpawner::tick(std::vector<Entity*> &entities) {
    if (spawnTimer.getElapsedTime().asSeconds() > nextSpawn) {
        WeaponType w = (WeaponType)util::rnd(1, NUM_WEAPONS - 1);
        sf::Vector2f pos = sf::Vector2f(util::rnd(32.f, (float)WINDOW_WIDTH - 32.f), -64.f);
        world->add_entity(new EntityWeapon(pos, w));
        nextSpawn = get_next_spawn_time();
        spawnTimer.restart();
    }
}

void EntitySpawner::draw(sf::RenderWindow &window) {
}