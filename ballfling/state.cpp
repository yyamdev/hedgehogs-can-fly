#include "state.h"

std::stack<State*> State::states;

State::State(World *world) {
    this->world = world;
}

void State::free_memory() {
    // free all memory used by states
    while (!State::states.empty()) {
        State::states.top()->on_lose_focus();
        delete State::states.top();
        State::states.pop();
    }
}

void State::change_state(State *state) {
    if (!State::states.empty()) {
        State::states.top()->on_lose_focus();
        delete State::states.top();
        State::states.pop();
    }
    State::states.push(state);
    state->on_gain_focus();
}

void State::push_state(State *state) {
    if (!State::states.empty()) {
        State::states.top()->on_lose_focus();
    }
    State::states.push(state);
    State::states.top()->on_gain_focus();
}

void State::pop_state() {
    if (!State::states.empty()) {
        State::states.top()->on_lose_focus();
        delete State::states.top();
        State::states.pop();
            if (!State::states.empty())
                State::states.top()->on_gain_focus();
    }
}

void State::event_current(sf::Event &event) {
    if (!State::states.empty())
        State::states.top()->on_event(event);
}

void State::tick_current() {
    if (!State::states.empty())
        State::states.top()->on_tick();
}

void State::draw_current(sf::RenderWindow &window) {
    if (!State::states.empty())
        State::states.top()->on_draw(window);
    if (!State::states.empty())
        State::states.top()->on_draw_ui(window);
}
