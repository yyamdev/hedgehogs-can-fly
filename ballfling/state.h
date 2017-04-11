#pragma once

// base class for a program state
// also manages the stack of states

#include <stack>
#include <SFML/Graphics.hpp>

class World;

class State {
public:
    State(World *world);
    virtual ~State(){};

    static void free_memory(); // call before program closes to ensure all memory is freed

    // <implemented by derived classes>
    virtual void on_event(sf::Event &event) = 0;
    virtual void on_tick() = 0;
    virtual void on_draw(sf::RenderWindow &window) = 0;
    virtual void on_draw_ui(sf::RenderWindow &window){};
    virtual void on_gain_focus(){};
    virtual void on_lose_focus(){};
    // </>

    // state changing
    static void change_state(State *state); // swap the current state for a new one. takes ownership of the pointer
    static void push_state(State *state); // pushes a new state onto the stack. takes ownership of the pointer
    static void pop_state(); // returns to a previous state

    // state stack management
    static void event_current(sf::Event &event);
    static void tick_current();
    static void draw_current(sf::RenderWindow &window);
    static void draw_ui_current(sf::RenderWindow &window);
    static State* get_current();

    virtual sf::Color get_clear_colour();

protected:
    World *world;
    static std::stack<State*> states;
};