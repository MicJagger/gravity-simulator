#include "universe.hpp"

#include <cmath>

#include "body.hpp"
#include "definitions.hpp"

inline double CalculateAcceleration(double mass, double distanceSquared) {
    return G * mass / distanceSquared;
}

inline double CalculateDistanceSquared(Body obj1, Body obj2) {
    double x = obj1._x - obj2._x;
    double y = obj1._y - obj2._y;
    double z = obj1._z - obj2._z;
    return (x * x) + (y * y) + (z * z);
}


Universe::Universe() {
    _tickSpeed = 1;
    _timeScaling = 1;
}

const std::map<long long, Body>* Universe::GetBodies() const {
    return &_bodies;
}

double Universe::GetTickSpeed() const {
    return _tickSpeed;
}

double Universe::GetTimeScaling() const {
    return _timeScaling;
}

int Universe::AddBody(Body body) {
    _bodies.emplace(_highestId, body);
    return SUCCESS;
}

int Universe::SetTickSpeed(double tickSpeed) {
    if (tickSpeed <= 0) {
        return FAIL;
    }
    _tickSpeed = tickSpeed;
    return SUCCESS;
}

int Universe::SetTimeScaling(double timeScaling) {
    if (timeScaling <= 0) {
        return FAIL;
    }
    _timeScaling = timeScaling;
    return SUCCESS;
}

int Universe::CalculateTick() {
    for (auto obj1 = _bodies.begin(); obj1 != _bodies.end(); obj1++) {
        for (auto obj2 = _bodies.begin(); obj2 != _bodies.end(); obj2++) {
            // if the same body, skip
            if (obj1->first == obj2->first) {
                continue;
            }
            double obj1Accel = CalculateAcceleration(obj2->second._mass, CalculateDistanceSquared(obj1->second, obj2->second));
        }
    }
    return SUCCESS;
}
