#pragma once
#ifndef _BODY_HPP
#define _BODY_HPP

#include <string>

struct Body {
    std::string name;
    // m
    double x;
    // m
    double y;
    // m
    double z;
    // m/s
    double xVel;
    // m/s
    double yVel;
    // m/s
    double zVel;
    // m
    double radius;
    // kg
    double mass;
    // W
    double luminosity;

    Body() {
        name = "";
        x, y, z = 0.0;
        xVel, yVel, zVel = 0.0;
        radius, mass = 0.0;
        luminosity = 0.1;
    }
};

#endif