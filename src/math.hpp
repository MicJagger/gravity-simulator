#pragma once
#ifndef _MATH_HPP
#define _MATH_HPP

#include <chrono>

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