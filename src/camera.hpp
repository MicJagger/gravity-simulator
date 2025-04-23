#pragma once
#ifndef _CAMERA_HPP
#define _CAMERA_HPP

struct Camera {
    // m
    double x;
    // m
    double y;
    // m
    double z;
    // degrees - horizontal
    float theta;
    // degrees - vertical
    float phi;
    // degrees - spin
    float psi;

    Camera() {
        x = 0, y = 0, z = 0;
        theta = 90;
        phi = 90;
        psi = 0;
    }
};

#endif