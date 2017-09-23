#pragma once

// enemy

#include "entity.h"
#include "observer.h"

enum EnemyState {
    ENEMY_STATE_INACTIVE,
    ENEMY_STATE_WAITING,
    ENEMY_STATE_ACTIVE,
};

class EntityEnemy : public Entity, public Observer {
public:
    EntityEnemy();
    EntityEnemy(sf::Vector2f pos, sf::Vector2f *playerPos);

    void event(sf::Event &e);
    void tick(std::vector<Entity*> &entities);
    void draw(sf::RenderWindow &window);

    void on_notify(Event event, void *data);
private:
    sf::Vector2f *player;
    EnemyState state;
    sf::Clock waitingTimer;
};
