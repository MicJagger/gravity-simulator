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
    const double& GetTickSpeed() const;
    const double& GetTimeScaling() const;
    const double& GetGravityScaling() const;
    const double& GetcScaling() const;
    const bool& IsPaused() const;


    // setters / manipulators

    int AddBody(const std::string& name, const Body& body);
    int RemoveBody(const std::string& name);
    int ClearBodies();
    int SetTickSpeed(const double& tickSpeed);
    int SetTimeScaling(const double& timeScaling);
    int SetGravityScaling(const double& gravityScaling);
    int SetcScaling(const double& cScaling);
    int Pause();
    int Unpause();

    int CalculateTick();
};

#endif