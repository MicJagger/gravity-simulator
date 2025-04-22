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
    // degrees - horizontal
    float _theta;
    // degrees - vertical
    float _phi;
    // degrees - spin
    float _psi;

    Camera() {
        _x = 0, _y = 0, _z = 0;
        _theta = 90;
        _phi = 90;
        _psi = 0;
    }
};

#endif