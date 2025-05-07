#pragma once
#ifndef _UNIVERSE_HPP
#define _UNIVERSE_HPP

#include <map>
#include <mutex>
#include <string>

#include "body.hpp"
#include "definitions.hpp"
#include "time.hpp"

class Universe {
    std::map<std::string, Body> _bodies;
    std::mutex _mtx;

    double _tickSpeed;
    double _timeScaling;
    double _gravityScaling;
    double _cScaling; // scaling speed of causality

    bool _paused;
public:
    Time time;

    Universe();
    

    // getters

    const std::map<std::string, Body>& GetBodies() const;
    std::map<std::string, Body>& GetBodiesMut(); // bodies are mutable
    double GetTickSpeed() const;
    double GetTimeScaling() const;
    double GetGravityScaling() const;
    double GetcScaling() const;
    bool IsPaused() const;


    // setters / manipulators

    int AddBody(const std::string& name, const Body& body);
    int RemoveBody(const std::string& name);
    int ClearBodies();
    int SetTickSpeed(double tickSpeed);
    int SetTimeScaling(double timeScaling);
    int SetGravityScaling(double gravityScaling);
    int SetcScaling(double cScaling);
    int Pause();
    int Unpause();

    int CalculateTick();
};

#endif