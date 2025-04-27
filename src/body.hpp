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
    // color
    float red, green, blue;

    Body() {
        name = "";
        x = 0.0, y = 0.0, z = 0.0;
        xVel = 0.0, yVel = 0.0, zVel = 0.0;
        radius = 0.0, mass = 0.0;
        luminosity = 0.1;
        red = 1.0f, green = 1.0f, blue = 1.0f;
    }
};

#endif