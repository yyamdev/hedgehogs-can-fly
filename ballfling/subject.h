#pragma once

/*
 * Base class for an object that can trigger events.
 */

#include <algorithm>
#include <vector>
#include "events.h"
#include "observer.h"

class Subject {
public:
    static void add_observer(Observer *obs);
    static void remove_observer(Observer *obs);
protected:
    static void notify(Event event, void *data);
    static std::vector<Observer*> observers;
private:

};
