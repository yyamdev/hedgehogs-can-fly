#include "play_state.h"
#include "world.h"
#include "dude_entity.h"
#include "terrain_entity.h"
#include "keyboard_driver.h"
#include "ai_driver.h"
#include "weapon_entity.h"

StatePlay::StatePlay(World *world) : State(world) {
    // set up game
    terrain = new EntityTerrain(sf::Vector2u(WINDOW_WIDTH, WINDOW_HEIGHT), 2.f);
    terrain->generate_normal(500.f, 100.f);
    world->remove_entity(ENTITY_TAG_ALL);
    world->add_entity(terrain);

    addPlayer = true;
    addAi = true;
    playerLives = 3;
    aiLives = 3;
}

void StatePlay::on_event(sf::Event &event) {
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Right) {
        sf::Vector2f mouse((float)event.mouseButton.x, (float)event.mouseButton.y);
        world->add_entity(new EntityWeapon(mouse, W_STICKY_GRENADE));
    }
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::B) {
        world->toggle_pause();
    }
}

void StatePlay::on_tick() {
    if (addPlayer && playerLives > 0) {
        EntityDude* dudePlayer = new EntityDude(sf::Vector2f(32.f, 0), PLAYER_NUMBER);
        dudePlayer->set_driver(std::make_unique<KeyboardDriver>(dudePlayer));
        world->add_entity(dudePlayer);
        addPlayer = false;
    }

    if (addAi && aiLives > 0) {
        EntityDude* dudeAi = new EntityDude(sf::Vector2f(WINDOW_WIDTH - 32.f, 0), AI_NUMBER);
        dudeAi->set_driver(std::make_unique<AiDriver>(dudeAi, terrain));
        world->add_entity(dudeAi);
        addAi = false;
    }

    if (playerLives <= 0)
        notify(EVENT_AI_WINS, NULL);

    if (aiLives <= 0)
        notify(EVENT_AI_WINS, NULL);
}

void StatePlay::on_draw(sf::RenderWindow &window) {
    hud.draw(window);
}

void StatePlay::on_gain_focus() {
}

void StatePlay::on_lose_focus() {
}

void StatePlay::on_notify(Event event, void *data) {
    if (event == EVENT_DUDE_DIE) {
        if (*((int*)data) == PLAYER_NUMBER) {
            addPlayer = true;
            playerLives --;
        }
        if (*((int*)data) == AI_NUMBER) {
            addAi = true;
            aiLives --;
        }
    }
}