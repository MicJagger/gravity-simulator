#pragma once
#ifndef _BODY_HPP
#define _BODY_HPP

struct Body {
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

    Body() {
        x, y, z = 0;
        xVel, yVel, zVel = 0;
        radius, mass = 0;
    }
};

#endif