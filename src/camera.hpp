#pragma once
#ifndef _CAMERA_HPP
#define _CAMERA_HPP

#include <string>

struct Camera {
    // m
    double x = 0.0, y = 0.0, z = 0.0;
    // degrees - horizontal
    float theta = 90.0;
    // degrees - vertical
    float phi = 90.0;
    // degrees - spin
    float psi = 0.0;
    // m / s
    double speed = 1.0;
    // d / s
    double rotationSpeed = 1.0;
    // d / p
    double sensitivity = 1.0;

    // locked to
    std::string bodyName = "";
    // m
    double bodyDistance = 0.0;
};

#endif