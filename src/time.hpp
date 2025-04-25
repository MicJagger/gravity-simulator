#pragma once
#ifndef _TIME_HPP
#define _TIME_HPP

#include <chrono>

class Time {
    double _targetTickSpeed;
    std::chrono::steady_clock::time_point _tickStart;
    std::chrono::steady_clock::time_point _tickEnd;
public:
    static void sleep(double seconds);

    Time();

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