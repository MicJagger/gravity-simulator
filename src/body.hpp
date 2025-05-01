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
    // degrees - horizontal
    double theta;
    // degrees - vertical
    double phi;
    // degrees - spin
    double psi;
    // degrees / s - horizontal
    double thetaVel;
    // degrees / s - vertical
    double phiVel;
    // degrees / s - spin
    double psiVel;
    // m
    double radius;
    // kg
    double mass;
    // W
    float luminosity;
    // color
    float red, green, blue;

    Body() {
        name = "";
        x = 0.0, y = 0.0, z = 0.0;
        xVel = 0.0, yVel = 0.0, zVel = 0.0;
        theta = 0.0, phi = 0.0, psi = 0.0;
        thetaVel = 0.0, phiVel = 0.0, psiVel = 0.0;
        radius = 0.0, mass = 0.0;
        luminosity = 0.1;
        red = 1.0f, green = 1.0f, blue = 1.0f;
    }
};

#endif