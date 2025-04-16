#pragma once
#ifndef _BODY_HPP
#define _BODY_HPP

struct Body {
    // m
    double _x;
    // m
    double _y;
    // m
    double _z;
    // m/s
    double _xVel;
    // m/s
    double _yVel;
    // m/s
    double _zVel;
    // m
    double _radius;
    // kg
    double _mass;

    Body() {
        _x, _y, _z = 0;
        _xVel, _yVel, _zVel = 0;
        _radius, _mass = 0;
    }
};

#endif