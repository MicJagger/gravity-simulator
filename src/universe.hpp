#pragma once
#ifndef _UNIVERSE_HPP
#define _UNIVERSE_HPP

#include <map>

#include "body.hpp"
#include "definitions.hpp"
#include "math.hpp"

class Universe {
    std::map<long long, Body> _bodies;
    double _tickSpeed;
    double _timeScaling;
public:
    Math _math;

    Universe();
};

#endif