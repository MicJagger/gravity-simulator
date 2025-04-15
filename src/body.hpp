#pragma once
#ifndef _BODY_HPP
#define _BODY_HPP

class Body {

public:
    // in m
    double _x;
    double _y;
    double _z;
    // in m/s
    double _xVel;
    double _yVel;
    double _zVel;
    // in kg
    double _mass;
};

#endif