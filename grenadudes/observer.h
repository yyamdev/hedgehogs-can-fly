#pragma once

// base class for objects that can listen for events

#include "events.h"

class Observer {
public:
    inline virtual ~Observer(){};
    virtual void on_notify(Event event, void *data) = 0; // must not destroy the instance or add entities in this function
};