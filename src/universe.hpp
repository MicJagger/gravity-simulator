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
    double _tickSpeed;
    double _timeScaling;
    std::mutex _mtx;
public:
    Math _math;

    Universe();
    

    // getters

    const std::map<long long, Body>& GetBodies() const;
    const double& GetTickSpeed() const;
    const double& GetTimeScaling() const;


    // setters / manipulators

    int AddBody(const Body& body);
    int SetTickSpeed(const double& tickSpeed);
    int SetTimeScaling(const double& timeScaling);

    int CalculateTick();
};

#endif