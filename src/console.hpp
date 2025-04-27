#pragma once
#ifndef _CONSOLE_HPP
#define _CONSOLE_HPP

#include <iostream>
#include <string>
#include <vector>

#ifdef __linux__
    //#include
#endif
#ifdef _WIN32
    #include <conio.h>
#endif

#include "camera.hpp"
#include "time.hpp"
#include "universe.hpp"
#include "window.hpp"

inline int RunCommand(int& sigIn, int& sigOut, const std::vector<std::string>& args, Universe& universe, Window& window);

inline int AddBody(Universe& universe);
inline void FailedConversion();
inline int Get(const std::vector<std::string>& args, Universe& universe, Window& window);
inline void InvalidArgCount(const int& found, const int& expected);
inline void InvalidArgCount(const int& found, const int& expectedLow, const int& expectedHigh);
inline int Set(const std::vector<std::string>& args, Universe& universe, Window& window);
std::vector<std::string> SplitArguments(const std::string& input);

// TODO: implement linux version for nonblocking IO
// handles console I/O
void ConsoleThread(int& sigIn, int& sigOut, Universe& universe, Window& window) {
    int failVal = 0;
    int inputReady;
    char key;
    std::string input;
    std::vector<std::string> args;
    while (true) {

        #ifdef _WIN32
            inputReady = _kbhit();
        #endif
        #ifdef __linux__
            inputReady = ();
        #endif

        if (inputReady) {

            #ifdef _WIN32
                key = getch();
            #endif
            #ifdef __linux__
                key = ();
            #endif

            if (key == '\b') {
                if (input.size() > 0) {
                    input.pop_back();
                }
                std::cout << "\b \b";
            }
            else if (key == '\r') {
                std::cout << "\n";
            }
            else { //key != '\b' && key != '\r'
                input += key;
                std::cout << key;
            }
            if (key != '\n' && key != '\r') {
                continue;
            }
            args = SplitArguments(input);
            input = "";

            if (args.size() > 0) {
                RunCommand(sigIn, sigOut, args, universe, window);
            }
        }
        if (sigIn <= SUCCESS) {
            break;
        }
        if (sigOut <= SUCCESS) {
            break;
        }
        Time::sleep(0.050);
    }
}

inline int RunCommand(int& sigIn, int& sigOut, const std::vector<std::string>& args, Universe& universe, Window& window) {
    if (args[0] == "help") {
        if (args.size() > 1) {
            InvalidArgCount(args.size(), 1);
            return FAIL;
        }
        std::cout << "List of commands:\n"
        "add - add a new body (further prompts)\n"
        "clear - remove all bodies\n"
        "get - print values of objects or settings\n"
        "lock - lock the camera relative to a body\n";
        "pause - pause universe\n"
        "quit - end program\n"
        "remove [name] - remove a body\n"
        "resume - unpause universe\n"
        "set - change values of objects or settings (further prompts)\n"
        "unlock - unbind camera from body it is locked to\n";
    }

    else if (args[0] == "add") {
        if (args.size() != 1) {
            InvalidArgCount(args.size(), 1);
            return FAIL;
        }
        if (AddBody(universe) <= FAIL) {
            std::cout << "Aborting command.\n";
            return FAIL;
        }
    }

    else if (args[0] == "clear") {
        if (args.size() != 1) {
            InvalidArgCount(args.size(), 1);
            return FAIL;
        }
        universe.ClearBodies();
    }

    else if (args[0] == "get") {
        if (args.size() > 2) {
            InvalidArgCount(args.size(), 1, 2);
            return FAIL;
        }
        if (Get(args, universe, window) <= FAIL) {
            std::cout << "Aborting command.\n";
        }
    }

    else if (args[0] == "lock") {
        std::cout << "WIP\n";
    }

    else if (args[0] == "pause") {
        if (args.size() != 1) {
            InvalidArgCount(args.size(), 1);
            return FAIL;
        }
        if (universe.Pause()) {
            std::cout << "paused\n";
        }
        else {
            std::cout << "already paused\n";
        }
    }

    else if (args[0] == "quit") {
        if (args.size() != 1) {
            InvalidArgCount(args.size(), 1);
            return FAIL;
        }
        sigOut = 0;
        return SUCCESS;
    }

    else if (args[0] == "remove") {
        if (args.size() != 2) {
            InvalidArgCount(args.size(), 2);
        }
        universe.RemoveBody(args[1]);
    }

    else if (args[0] == "resume" || args[0] == "unpause") {
        if (args.size() != 1) {
            InvalidArgCount(args.size(), 1);
            return FAIL;
        }
        if (universe.Unpause()) {
            std::cout << "resumed\n";
        }
        else {
            std::cout << "already running\n";
        }
    }

    else if (args[0] == "set") {
        if (args.size() > 6) {
            InvalidArgCount(args.size(), 1, 6);
            return FAIL;
        }
        if (Get(args, universe, window) <= FAIL) {
            std::cout << "aborting command\n";
        }
    }

    else if (args[0] == "unlock") {
        std::cout << "WIP\n";
    }

    else {
        std::cout << "command not recognized\n";
        return FAIL;
    }

    return SUCCESS;
}


// 

inline int AddBody(Universe &universe) {
    Body body;
    std::string input;
    std::vector<std::string> tempArgs;

    std::cout << "Enter body name: ";
    std::getline(std::cin, body.name);

    std::cout << "Enter x, y, and z coordinates: ";
    std::getline(std::cin, input);
    tempArgs = SplitArguments(input);
    if (tempArgs.size() != 3) {
        InvalidArgCount(tempArgs.size(), 3);
        return FAIL;
    }
    try {
        body.x = std::stod(tempArgs[0]);
        body.y = std::stod(tempArgs[1]);
        body.z = std::stod(tempArgs[2]);
    }
    catch (...) {
        FailedConversion();
        return FAIL;
    }

    std::cout << "Enter x, y, and z velocities: ";
    std::getline(std::cin, input);
    tempArgs = SplitArguments(input);
    if (tempArgs.size() != 3) {
        InvalidArgCount(tempArgs.size(), 3);
        return FAIL;
    }
    try {
        body.xVel = std::stod(tempArgs[0]);
        body.yVel = std::stod(tempArgs[1]);
        body.zVel = std::stod(tempArgs[2]);
    }
    catch (...) {
        FailedConversion();
        return FAIL;
    }

    std::cout << "Enter radius: ";
    std::getline(std::cin, input);
    try {
        body.radius = std::stod(input);
    }
    catch (...) {
        FailedConversion();
        return FAIL;
    }

    std::cout << "Enter mass: ";
    std::getline(std::cin, input);
    try {
        body.mass = std::stod(input);
    }
    catch (...) {
        FailedConversion();
        return FAIL;
    }

    std::cout << "Enter luminosity (0.0 to 1.0): ";
    std::getline(std::cin, input);
    try {
        body.luminosity = std::stof(input);
    }
    catch (...) {
        FailedConversion();
        return FAIL;
    }
    if (body.luminosity > 1.0 || body.luminosity < 0.0) {
        std::cout << "Out of range.\n";
        return FAIL;
    }

    std::cout << "Enter rgb values (0.0 to 1.0): ";
    std::getline(std::cin, input);
    tempArgs = SplitArguments(input);
    if (tempArgs.size() != 3) {
        InvalidArgCount(tempArgs.size(), 3);
        return FAIL;
    }
    try {
        body.red = std::stof(tempArgs[0]);
        body.green = std::stof(tempArgs[1]);
        body.blue = std::stof(tempArgs[2]);
    }
    catch (...) {
        FailedConversion();
        return FAIL;
    }
    if (body.red > 1.0 || body.red < 0.0 || 
        body.green > 1.0 || body.green < 0.0 || 
        body.blue > 1.0 || body.blue < 0.0) {
        std::cout << "Out of range.\n";
        return FAIL;
    }

    universe.AddBody(body.name, body);

    return SUCCESS;
}

inline void FailedConversion() {
    std::cout << "Failed conversion.\n";
}

inline int Get(const std::vector<std::string>& args, Universe& universe, Window& window) {
    std::string choice;
    if (args.size() == 1) {
        std::cout << "choices: "
        "bodies\n"
        "body [name]\n"
        "camera\n"
        "cameraSpeed\n"
        "cScaling\n"
        "gravityScaling\n"
        "isPaused\n"
        "targetFramerate\n"
        "tickSpeed\n"
        "timeScaling\n";
        std::cin >> choice;
    }
    else {
        choice = args[1];
    }

    if (choice == "bodies") {
        const auto bodies = universe.GetBodies();
        for (const auto [name, data]: bodies) {
            std::cout << name << "\n";
        }
    }

    else if (choice == "body") {
        return FAIL;
    }

    else if (choice == "camera") {
        const Camera camera = window.GetCamera();
        return FAIL;
    }

    else if (choice == "cameraSpeed") {
        return FAIL;
    }

    else if (choice == "cScaling") {
        std::cout << "cScaling = " << universe.GetcScaling() << "\n";
    }

    else if (choice == "gravityScaling") {
        std::cout << "gravityScaling = " << universe.GetGravityScaling() << "\n";
    }

    else if (choice == "isPaused") {
        std::cout << "isPaused = " << universe.IsPaused() << "\n";
    }

    else if (choice == "targetFramerate") {
        std::cout << "targetFramerate = " << window.time.GetTickSpeed() << "\n";
    }

    else if (choice == "tickSpeed") {
        std::cout << "tickSpeed = " << universe.GetTickSpeed() << "\n";
    }

    else if (choice == "timeScaling") {
        std::cout << "timeScaling = " << universe.GetTimeScaling() << "\n";
    }

    else {
        std::cout << "unrecognized\n";
        return FAIL;
    }

    return SUCCESS;
}

inline void InvalidArgCount(const int &found, const int &expected) {
    std::cout << "Invalid argument count. Found: " << found << ", Expected: " << expected << "\n";
}

inline void InvalidArgCount(const int &found, const int &expectedLow, const int& expectedHigh) {
    std::cout << "Invalid argument count. Found: " << found << ", Expected: " << expectedLow << " to " << expectedHigh << "\n";
}

inline int Set(const std::vector<std::string>& args, Universe& universe, Window& window) {
    std::string choice;
    if (args.size() == 1) {
        std::cout << "choices: "
        "body [name]\n"
        "camera\n"
        "cameraSpeed\n"
        "cScaling\n"
        "gravityScaling\n"
        "isPaused\n"
        "targetFramerate\n"
        "tickSpeed\n"
        "timeScaling\n";
        std::cin >> choice;
    }
    else {
        choice = args[1];
    }

    if (choice == "body") {
        return FAIL;
    }

    else if (choice == "camera") {
        return FAIL;
    }

    else if (choice == "cameraSpeed") {
        return FAIL;
    }

    else if (choice == "cScaling") {
        return FAIL;
    }

    else if (choice == "gravityScaling") {
        return FAIL;
    }

    else if (choice == "isPaused") {
        return FAIL;
    }

    else if (choice == "targetFramerate") {
        return FAIL;
    }

    else if (choice == "tickSpeed") {
        return FAIL;
    }

    else if (choice == "timeScaling") {
        return FAIL;
    }

    else {
        std::cout << "unrecognized\n";
        return FAIL;
    }

    return SUCCESS;
}

std::vector<std::string> SplitArguments(const std::string& input) {
    std::vector<std::string> args = { "" };
    int argIndex = 0;
    for (int i = 0; i < input.size(); i++) {
        if (input[i] == ' ') {
            args.push_back("");
            argIndex++;
        }
        else if (input[i] == '\n' || input[i] == '\r') {
            return args;
        }
        else {
            args[argIndex] += input[i];
        }
    }
    return args;
}

#endif