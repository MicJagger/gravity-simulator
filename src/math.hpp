#pragma once
#ifndef _MATH_HPP
#define _MATH_HPP

#include <chrono>

constexpr double c = 299792458.0;
constexpr double e = 2.7182818285;
constexpr double G = 6.6743e-11;
constexpr double g = 9.80665;
constexpr double pi = 3.1415926536;
constexpr double sqrt2 = 1.41421356237;
constexpr double sqrt2o2 = 0.70710678118;

class Math {
    double _targetTickSpeed;
    std::chrono::steady_clock::time_point _tickStart;
    std::chrono::steady_clock::time_point _tickEnd;
public:
    static void sleep(double seconds);

    Math();

    const double& GetTickSpeed();
    int SetTickSpeed(const double& tickSpeed);

    // tracks beginning of a frame
    void TickStart();
    // tracks end of a frame
    void TickEnd();
    // tracks end of a frame and sleeps to maintain framerate
    void TickEndAndSleep();    
};

#endif