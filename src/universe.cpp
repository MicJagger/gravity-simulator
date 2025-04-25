#include "universe.hpp"

#include <cmath>
#include <iostream>

#include "body.hpp"
#include "definitions.hpp"
#include "values.hpp"

inline double CalculateAcceleration(const double& mass, const double& distanceSquared) {
    double distance = sqrt(distanceSquared);
    double relativity = 1.0;// / sqrt(1 - (G2oc2 * mass / distance));
    return (G * mass / distanceSquared) * relativity;
}

inline int Accelerate(Body& obj1, const Body& obj2, const double& tickspeedFactor, const double& gravityScaling) {
    double dx = obj2.x - obj1.x;
    double dy = obj2.y - obj1.y;
    double dz = obj2.z - obj1.z;
    double distanceSquared = (dx * dx) + (dy * dy) + (dz * dz);
    double distance = sqrt(distanceSquared);
    double acceleration = CalculateAcceleration(obj2.mass, distanceSquared) * gravityScaling;
    double accelerationFraction = tickspeedFactor * acceleration;
    if (abs(dx) > 0.001) {
        obj1.xVel += accelerationFraction * (dx / distance);
    }
    if (abs(dy) > 0.001) {
        obj1.yVel += accelerationFraction * (dy / distance);
    }
    if (abs(dz) > 0.001) {
        obj1.zVel += accelerationFraction * (dz / distance);
    }
    return SUCCESS;
}

inline bool CheckCollision(const Body& obj1, const Body& obj2) {
    double dx = obj2.x - obj1.x;
    double dy = obj2.y - obj1.y;
    double dz = obj2.z - obj1.z;
    double distance = sqrt((dx * dx) + (dy * dy) + (dz * dz));
    if ((obj1.radius + obj2.radius) >= distance) {
        return true;
    }
    return false;
}

inline bool CheckCollision(const Body& obj1, const Body& obj2, const double& distanceSquared) {
    double distance = sqrt(distanceSquared);
    if ((obj1.radius + obj2.radius) >= distance) {
        return true;
    }
    return false;
}


// 

Universe::Universe() {
    _tickSpeed = 60;
    _timeScaling = 1;
    _gravityScaling = 1;
}

const std::map<long long, Body>& Universe::GetBodies() const {
    return _bodies;
}

const double& Universe::GetTickSpeed() const {
    return _tickSpeed;
}

const double& Universe::GetTimeScaling() const {
    return _timeScaling;
}

const double& Universe::GetGravityScaling() const {
    return _gravityScaling;
}

int Universe::AddBody(const Body& body) {
    _mtx.lock();
    _bodies.emplace(_highestId, body);
    _mtx.unlock();
    std::cout << "Added body: ID " << _highestId << "\n";
    _highestId++;
    return SUCCESS;
}

int Universe::SetTickSpeed(const double& tickSpeed) {
    if (tickSpeed <= 0) {
        return FAIL;
    }
    _mtx.lock();
    math.SetTickSpeed(tickSpeed);
    _tickSpeed = tickSpeed;
    _mtx.unlock();
    return SUCCESS;
}

int Universe::SetTimeScaling(const double& timeScaling) {
    if (timeScaling <= 0) {
        return FAIL;
    }
    _mtx.lock();
    _timeScaling = timeScaling;
    _mtx.unlock();
    return SUCCESS;
}

int Universe::SetGravityScaling(const double& gravityScaling) {
    _mtx.lock();
    _gravityScaling = gravityScaling;
    _mtx.unlock();
    return SUCCESS;
}

int Universe::CalculateTick() {
    double tickspeedFactor = _timeScaling * 1.0 / _tickSpeed;
    _mtx.lock();
    // move positions
    for (auto& [id, obj]: _bodies) {
        obj.x += obj.xVel * tickspeedFactor;
        obj.y += obj.yVel * tickspeedFactor;
        obj.z += obj.zVel * tickspeedFactor;
    }
    // calculate accelerations
    for (auto& [id1, obj1]: _bodies) {
        for (auto& [id2, obj2]: _bodies) {
            // if the same body, skip
            if (id1 == id2) {
                continue;
            }
            Accelerate(obj1, obj2, tickspeedFactor, _gravityScaling);
        }
    }
    _mtx.unlock();
    return SUCCESS;
}
