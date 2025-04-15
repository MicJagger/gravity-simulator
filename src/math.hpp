#pragma once
#ifndef _MATH_HPP
#define _MATH_HPP

#include <chrono>

const double c = 299792458;
const double e = 2.7182818285;
const double G = 6.6743e-11;
const double g = 9.80665;
const double pi = 3.1415926536;

class Math {
    double _targetTickSpeed;
    std::chrono::steady_clock::time_point _tickStart;
    std::chrono::steady_clock::time_point _tickEnd;
public:
    static void sleep(double seconds);

    Math();

    int SetTickSpeed(double tickSpeed);

    // tracks beginning of a frame
    void TickStart();
    // tracks end of a frame
    void TickEnd();
    // tracks end of a frame and sleeps to maintain framerate
    void TickEndAndSleep();    
};

#endif