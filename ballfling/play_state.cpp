#include <string>
#include <fstream>
#include "play_state.h"
#include "win_state.h"
#include "pause_state.h"
#include "terrain_entity.h"
#include "ball_entity.h"
#include "timer_entity.h"
#include "sprite_entity.h"
#include "world.h"
#include "gui.h"
#include "particle.h"
#include "util.h"
#include "build_options.h"
#include "shared_res.h"

StatePlay::StatePlay(World *world, int levelNum) :
    State(world, "play"), levelNum(levelNum)
{
    world->remove_entity(ENTITY_TAG_ALL);

    // Decide level colour scheme
    if (levelNum <= 4) {
        levelColour = sf::Color(99, 155, 255);
        backgroundColor = sf::Color(34, 32, 52);
    }
    else if (levelNum <= 8) {
        levelColour = sf::Color(203, 219, 252);
        backgroundColor = sf::Color(50, 60, 57);
    }
    else {
        levelColour = sf::Color(55, 148, 110);
        backgroundColor = sf::Color(69, 40, 60);
    }

    // Add background sprites for each level
    switch (levelNum) {
    case 1:
        world->add_entity(new EntitySprite("data/lvl1txt.png", 2898.f, 1625.f));
        world->add_entity(new EntitySprite("data/how_to_fling.png", 2636.f, 1299.f));
        world->add_entity(new EntitySprite("data/how_to_nudge.png", 4079.f, 1232.f));
        world->add_entity(new EntitySprite("data/how_to_pause.png", 2829.f,  199.f));
        world->add_entity(new EntitySprite("data/nudge_instruction.png", 3317.f,  448.f, .5f));
        world->add_entity(new EntitySprite("data/arrow_instruction.png", 3519.f,  666.f, 1.f, -110.f));
        break;
    case 2:
        world->add_entity(new EntitySprite("data/lvl2txt.png", 1647.f, 2165.f));
        break;
    case 3:
        world->add_entity(new EntitySprite("data/lvl3txt.png", 451.f, 1313.f));
        break;
    case 4:
        world->add_entity(new EntitySprite("data/lvl4txt.png", 2562.f, 1873.f));
        break;
    case 5:
        world->add_entity(new EntitySprite("data/lvl5txt.png", 402.f, 2830.f));
        world->add_entity(new EntitySprite("data/dude.png", 4664.f, 1544.f, 3.f, 0.f, true));
        break;
    case 6:
        world->add_entity(new EntitySprite("data/lvl6txt.png", 253.f, 768.f));
        break;
    case 7:
        world->add_entity(new EntitySprite("data/lvl7txt.png", 1587.f, 1695.f));
        break;
    case 8:
        world->add_entity(new EntitySprite("data/frame.png", 1700.f, 615.f, 1.f, 0.f, true));
        world->add_entity(new EntitySprite("data/lvl8txt.png", 1026.f, 1926.f));
        break;
    case 9:
        world->add_entity(new EntitySprite("data/lvl9txt.png", 356.f, 2522.f));
        break;
    case 10:
        world->add_entity(new EntitySprite("data/lvl10txt.png", 993.f, 1698.f));
        break;
    case 11:
        world->add_entity(new EntitySprite("data/lvl11txt.png", 425.f, 1729.f));
        break;
    case 12:
        world->add_entity(new EntitySprite("data/lvl12txt.png", 1951.f, 2499.f));
        break;
    }

    // Load level data
    std::string filename = "data/lvl" + util::to_string(levelNum) + ".png";
    terrain = new EntityTerrain(filename, levelColour);
    if (!terrain->loaded_successfuly()) {
        return;
    }
    world->add_entity(terrain);

    timer = new EntityTimer();
    world->add_entity(timer);

    player = new EntityBall(terrain->playerSpawn, sf::Vector2f(), levelColour);
    world->add_entity(player);

    notify(EVENT_LEVEL_START, NULL);

    initSuccess = true;
}

sf::Color StatePlay::get_clear_colour()
{
    return backgroundColor;
}

void StatePlay::on_event(sf::Event &event)
{
    if (event.type == sf::Event::KeyPressed &&
        (event.key.code == sf::Keyboard::Escape || event.key.code == sf::Keyboard::P)) {

        notify(EVENT_PLAY_PAUSE, NULL);
        State::push_state(new StatePause(world, backgroundColor));
    }
}

void StatePlay::on_tick()
{
    if (!world->is_paused())
        particles_tick(world->camera);
}

void StatePlay::on_draw(sf::RenderWindow &window)
{
    particles_draw(window, world->camera);
}

void StatePlay::on_draw_ui(sf::RenderWindow &window)
{
    hud.draw(window, world->camera, levelColour);
}

void StatePlay::on_gain_focus()
{
    if (!initSuccess) {
        State::pop_state();
        return;
    }

    if (restartOnResume) {
        State::change_state(new StatePlay(world, levelNum));
        return;
    }

    gui.RemoveAll();
    world->set_pause(false);
    notify(EVENT_PLAY_RESUME, NULL);
    notify(EVENT_ENTER_GAME, NULL);
}

void StatePlay::on_lose_focus() {}

void StatePlay::on_notify(Event event, void *data)
{
    if (event == EVENT_BALL_HIT_FINISH) {
        world->set_pause(true);
        notify(EVENT_PLAY_PAUSE, NULL);
        State::push_state(new StateWin(world, levelNum, backgroundColor, timer->get_time()));
        return;
    }

    if (event == EVENT_PLAY_SIGNAL_LEVEL_RESTART) {
        restartOnResume = true;
    }
}
