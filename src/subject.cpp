#include <algorithm>
#include "subject.h"

std::vector<Observer*> Subject::observers;

void Subject::add_observer(Observer *obs)
{
    if (std::find(Subject::observers.begin(), Subject::observers.end(), obs) == Subject::observers.end())
        Subject::observers.push_back(obs); // add if not already present
}

void Subject::remove_observer(Observer *obs)
{
    Subject::observers.erase(std::remove(observers.begin(), observers.end(), obs), observers.end());
}

void Subject::notify(Event event, void *data)
{
    std::vector<Observer*> observersCopy = Subject::observers;
    /*
     * Iterate over a copy of the vector to account for the case where Observers
     * are created or deleted from inside an on_notify() function.
     */
    for (auto &obs : observersCopy) {
        obs->on_notify(event, data);
    }
}
