#pragma once
#ifndef _UNIVERSE_HPP
#define _UNIVERSE_HPP

#include <map>
#include <mutex>

#include "body.hpp"
#include "definitions.hpp"
#include "math.hpp"

class Universe {
    long long _highestId = 0;
    std::map<long long, Body> _bodies;
    std::mutex _mtx;

    double _tickSpeed;
    double _timeScaling;
    double _gravityScaling;
public:
    Math math;

    Universe();
    

    // getters

    const std::map<long long, Body>& GetBodies() const;
    const double& GetTickSpeed() const;
    const double& GetTimeScaling() const;
    const double& GetGravityScaling() const;


    // setters / manipulators

    int AddBody(const Body& body);
    int SetTickSpeed(const double& tickSpeed);
    int SetTimeScaling(const double& timeScaling);
    int SetGravityScaling(const double& gravityScaling);

    int CalculateTick();
};

#endif