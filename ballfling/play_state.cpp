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
    //gui.RemoveAll();
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

    // Add background sprites
    switch (levelNum) {
    case 1:
        // Instructions
        world->add_entity(new EntitySprite("data/how_to_fling.png", 2636.f, 1299.f));
        world->add_entity(new EntitySprite("data/how_to_pause.png", 4079.f, 1232.f));
        world->add_entity(new EntitySprite("data/how_to_nudge.png", 3580.f,  658.f));
        break;
    case 5:
        world->add_entity(new EntitySprite("data/dude.png", 4664.f, 1544.f, 3.f));
        break;
    case 8:
        world->add_entity(new EntitySprite("data/frame.png", 1700.f, 615.f));
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

    if (event.type == sf::Event::MouseButtonPressed && startScreenState == 0) {
        startScreenState = 1;
        counterStart = 256;
    }
}

void StatePlay::on_tick()
{
    if (!world->is_paused())
        particles_tick(world->camera);

    if (startScreenState == 0) {
        ++counterStart;
        if (counterStart >= 200) {
            startScreenState = 1;
            counterStart = 256;
        }
    }

    if (startScreenState == 1) {
        counterStart -= 4;
        if (counterStart < 0) {
            startScreenState = 2;
        }
    }
}

void StatePlay::on_draw(sf::RenderWindow &window)
{
    particles_draw(window, world->camera);

    sf::Uint8 startAlpha = 255;

    sf::Sprite sprStart(txtStart);
    sprStart.setOrigin(sf::Vector2f(112.f, 112.f));
    sprStart.setPosition(WINDOW_WIDTH / 2.f, WINDOW_HEIGHT / 2.f);
    sprStart.setScale(sf::Vector2f(.7f, .7f));

    sf::Text txtLvl(util::to_string(levelNum), fntUi, 80);
    txtLvl.setOrigin(txtLvl.getLocalBounds().width, txtLvl.getLocalBounds().height);
    txtLvl.setPosition(sf::Vector2f(WINDOW_WIDTH / 2.f, WINDOW_HEIGHT / 2.f));
    txtLvl.setColor(sf::Color(95, 205, 228, startAlpha));
    if (levelNum == 1)
        txtLvl.move(4.f, 6.f);
    else if (levelNum < 10)
        txtLvl.move(14.f, 6.f);
    else
        txtLvl.move(20.f, 6.f);

    if (startScreenState == 1) {
        sprStart.setColor(sf::Color(255, 255, 255, counterStart));
        txtLvl.setColor(sf::Color(95, 205, 228, counterStart));
    }

    if (startScreenState == 0 || startScreenState == 1) {
        window.draw(sprStart);
        window.draw(txtLvl);
    }
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
