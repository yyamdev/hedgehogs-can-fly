#include "observer.h"
#include "subject.h"

Observer::Observer() {
    Subject::add_observer(this);
}

Observer::~Observer() {
    Subject::remove_observer(this);
}