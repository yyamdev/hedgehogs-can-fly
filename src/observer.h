#pragma once

/*
 * Base class for objects that can detect events.
 */

#include "events.h"

class Observer {
public:
    Observer();
    virtual ~Observer();

    virtual void on_notify(Event event, void *data) = 0;
};
