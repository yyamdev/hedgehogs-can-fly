#pragma once

// base class for an object that can trigger events

#include "events.h"
#include <vector>
#include <algorithm>
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