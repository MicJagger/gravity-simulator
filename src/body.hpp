#pragma once
#ifndef _BODY_HPP
#define _BODY_HPP

#include <string>

struct Body {
    std::string name = "";
    // m
    double x = 0.0, y = 0.0, z = 0.0;
    // m/s
    double xVel = 0.0, yVel = 0.0, zVel = 0.0;
    // degrees - horizontal
    double theta = 0.0;
    // degrees - vertical
    double phi = 0.0;
    // degrees - spin
    double psi = 0.0;
    // degrees / s - horizontal
    double thetaVel = 0.0;
    // degrees / s - vertical
    double phiVel = 0.0;
    // degrees / s - spin
    double psiVel = 0.0;
    // m
    double radius = 0.0;
    // kg
    double mass = 0.0;
    // value of brightness (maybe later W)
    float luminosity = 0.1f;
    // color
    float red = 1.0f, green = 1.0f, blue = 1.0f;
};

#endif