#include "universe.hpp"

#include <cmath>
#include <iostream>

#include "body.hpp"
#include "definitions.hpp"
#include "values.hpp"

inline double CalculateGravitationalAcceleration(const double& mass, const double& distanceSquared, const double& cScaling) {
    double distance = sqrt(distanceSquared);
    double G2oc2Scaled = G2oc2 / (cScaling * cScaling); // 1 / c^2 -> 1 / cScaling^2
    double relativity = 1.0 / sqrt(1.0 - (G2oc2Scaled * mass / distance));
    return (G * mass / distanceSquared) * relativity;
    // https://physics.stackexchange.com/questions/47379/what-is-the-weight-equation-through-general-relativity
}

inline int Accelerate(Body& obj1, const Body& obj2, const double& tickspeedFactor, const double& gravityScaling, const double& cScaling) {
    double dx = obj2.x - obj1.x;
    double dy = obj2.y - obj1.y;
    double dz = obj2.z - obj1.z;
    double distanceSquared = (dx * dx) + (dy * dy) + (dz * dz);
    double distance = sqrt(distanceSquared);
    double acceleration = CalculateGravitationalAcceleration(obj2.mass, distanceSquared, cScaling) * gravityScaling;
    double accelerationFraction = tickspeedFactor * acceleration / distance;
    // TODO: adjust acceleration/velocity based on relativity
    if (fabs(distance) > 1e-18) {
        obj1.xVel += accelerationFraction * dx;
        obj1.yVel += accelerationFraction * dy;
        obj1.zVel += accelerationFraction * dz;
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
    _cScaling = 1;
    _paused = false;
}


// 

const std::map<std::string, Body>& Universe::GetBodies() const {
    return _bodies;
}

std::map<std::string, Body>& Universe::GetBodiesMut() {
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

const double& Universe::GetcScaling() const {
    return _cScaling;
}

const bool& Universe::IsPaused() const {
    return _paused;
}


// 

int Universe::AddBody(const std::string& name, const Body& body) {
    if (name == "") {
        std::cout << "Name must not be empty\n";
        return FAIL;
    }
    _mtx.lock();
    _bodies.emplace(name, body);
    _mtx.unlock();
    std::cout << "Added body: " << name << "\n";
    return SUCCESS;
}

int Universe::RemoveBody(const std::string &name) {
    _mtx.lock();
    if (_bodies.find(name) == _bodies.end()) {
        _mtx.unlock();
        return FAIL;
    }
    _bodies.erase(name);
    _mtx.unlock();
    return SUCCESS;
}

int Universe::ClearBodies() {
    _mtx.lock();
    _bodies.clear();
    _mtx.unlock();
    return SUCCESS;
}

int Universe::SetTickSpeed(const double& tickSpeed) {
    if (tickSpeed <= 0) {
        return FAIL;
    }
    _mtx.lock();
    time.SetTickSpeed(tickSpeed);
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

int Universe::SetcScaling(const double& cScaling) {
    if (cScaling <= 0) {
        return FAIL;
    }
    _mtx.lock();
    _cScaling = cScaling;
    _mtx.unlock();
    return SUCCESS;
}

int Universe::Pause() {
    _mtx.lock();
    if (_paused) {
        return false;
    }
    _paused = true;
    _mtx.unlock();
    return true;
}

int Universe::Unpause() {
    _mtx.lock();
    if (!_paused) {
        return false;
    }
    _paused = false;
    _mtx.unlock();
    return true;
}

int Universe::CalculateTick() {
    _mtx.lock();
    double tickspeedFactor = _timeScaling * 1.0 / _tickSpeed;
    // move positions
    for (auto& [name, body]: _bodies) {
        body.x += body.xVel * tickspeedFactor;
        body.y += body.yVel * tickspeedFactor;
        body.z += body.zVel * tickspeedFactor;
        body.theta += body.thetaVel * tickspeedFactor;
        body.phi += body.phiVel * tickspeedFactor;
        body.psi += body.psiVel * tickspeedFactor;
        body.theta = fmod(body.theta, 360.0);
        body.phi = fmod(body.phi, 360.0);
        body.psi = fmod(body.psi, 360.0);
    }
    // calculate accelerations
    for (auto& [name1, body1]: _bodies) {
        for (auto& [name2, body2]: _bodies) {
            // if the same body, skip
            if (name1 == name2) {
                continue;
            }
            Accelerate(body1, body2, tickspeedFactor, _gravityScaling, _cScaling);
        }
    }
    _mtx.unlock();
    return SUCCESS;
}
