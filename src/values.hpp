#pragma once
#ifndef _VALUES_HPP
#define _VALUES_HPP

// important constants

constexpr double c = 299792458.0;
constexpr double e = 2.7182818285;
constexpr double G = 6.6743e-11;
constexpr double g = 9.80665;
constexpr double pi = 3.1415926536;
constexpr double sqrt2 = 1.41421356237;

constexpr double G2oc2 = 2 * G / (c * c);
constexpr double sqrt2o2 = 0.70710678118;

// solar system

// sun

constexpr double sunRadius = 695700000.0;
constexpr double sunMass = 1.9885e30;
constexpr double sunDistance = 0.0;
constexpr double sunVelocity = 0.0;

// mercury

constexpr double mercuryRadius = 2439.7e3;
constexpr double mercuryMass = 3.3011e23;
constexpr double mercuryDistance = 57.91e9;
constexpr double mercuryVelocity = 47.36e3;

// venus

constexpr double venusRadius = 6051800.0;
constexpr double venusMass = 4.8675e24;
constexpr double venusDistance = 108.21e9;
constexpr double venusVelocity = 35.02e3;

// earth

constexpr double earthRadius = 6371.0e3;
constexpr double earthMass = 5.972168e24;
constexpr double earthDistance = 149.598023e9;
constexpr double earthVelocity = 29.7827e3;

// moon

constexpr double moonRadius = 1737.4e3;
constexpr double moonMass = 7.346e22;
constexpr double moonDistance = 384399.0e3; // from earth
constexpr double moonVelocity = 1.022e3;

// mars
constexpr double marsRadius = 3389.5e3;
constexpr double marsMass = 6.4171e23;
constexpr double marsDistance = 227.939366e9;
constexpr double marsVelocity = 24.07e3;

// jupiter
constexpr double jupiterRadius = 69911.0e3;
constexpr double jupiterMass = 1.8982e27;
constexpr double jupiterDistance = 778.479e9;
constexpr double jupiterVelocity = 13.06e3;

// saturn
constexpr double saturnRadius = 58232.0e3;
constexpr double saturnMass = 5.6834e26;
constexpr double saturnDistance = 1433.53e9;
constexpr double saturnVelocity = 9.68e3;

// uranus
constexpr double uranusRadius = 25362.0e3;
constexpr double uranusMass = 8.681e25;
constexpr double uranusDistance = 2870.972e9;
constexpr double uranusVelocity = 6.8e3;

// neptune
constexpr double neptuneRadius = 24622.0e3;
constexpr double neptuneMass = 1.02409e26;
constexpr double neptuneDistance = 4.5e12;
constexpr double neptuneVelocity = 5.43e3;

#endif