#pragma once
#ifndef _CAMERA_HPP
#define _CAMERA_HPP

struct Camera {
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
    // 
    double _theta;
    // 
    double _phi;
    // m / s
    double _moveSpeed;

    Camera() {
        _x, _y, _z = 0;
        _xVel, _yVel, _zVel = 0;
        _theta = 0;
        _phi = 90;
        _moveSpeed = 1;
    }
};

#endif