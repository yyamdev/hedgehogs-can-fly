#include "entity.h"
#include <math.h>
#include "util.h"

Entity::Entity() {
    world = NULL;
    velocity = sf::Vector2f();
    tag = "";
    remove = false;
    velocity = sf::Vector2f(0.f, 0.f);
    position = sf::Vector2f(0.f, 0.f);
    collisionRadius = 0.f;
}

bool Entity::intersects(const Entity &other) {
    float distance = sqrtf(pow(other.position.x - position.x, 2) + pow(other.position.y - position.y, 2));
    return distance < collisionRadius + other.collisionRadius;
}

std::string Entity::get_tag() {
    return tag;
}

void Entity::set_world_ptr(World *world) {
    this->world = world;
}