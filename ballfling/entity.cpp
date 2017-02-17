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
    invulnerable = true;
    hp = maxHp = 100;
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

sf::Vector2f Entity::reflect(const sf::Vector2f incident, const sf::Vector2f unitNormal) {
    float speed = util::distance(0.f, 0.f, incident.x, incident.y);
    sf::Vector2f incidentUnit = incident / speed;
    sf::Vector2f reflect = incidentUnit + unitNormal * 2.f; // reflect
    sf::Vector2f reflectUnit = reflect / util::distance(0.f, 0.f, reflect.x, reflect.y);
    return reflectUnit * speed;
}

sf::Vector2f Entity::apply_friction(const sf::Vector2f vector, const float friction) {
    float speed = util::distance(0.f, 0.f, vector.x, vector.y);
    float speedReduced = std::max(0.f, speed - friction);
    sf::Vector2f vectorUnit = vector / speed;
    return vectorUnit * speedReduced;
}

float Entity::dot(const sf::Vector2f a, const sf::Vector2f b) {
    return (a.x * b.x) + (a.y * b.y);
}

int Entity::get_hp() { return hp; }

int Entity::get_max_hp() { return maxHp; }

void Entity::set_hp(int hp) {
    if (invulnerable) return;
    this->hp = util::clamp(hp, 0, maxHp);
    notify(EVENT_ENTITY_HP_CHANGE, (void*)(&(this->hp)));
    if (this->hp == 0) {
        remove = true;
        notify(EVENT_ENTITY_DIE, (void*)this);
    }
}

void Entity::damage(int dmg) {
    if (invulnerable || clockDamageCooldown.getElapsedTime().asMilliseconds() < ENTITY_DAMAGE_COOLDOWN_MILLISECONDS)
        return;
    clockDamageCooldown.restart();
    hp = util::clamp(hp - dmg, 0, maxHp);
    notify(EVENT_ENTITY_HP_CHANGE, (void*)(&hp));
    if (this->hp == 0) {
        remove = true;
        notify(EVENT_ENTITY_DIE, (void*)this);
    }
}