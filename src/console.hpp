#pragma once
#ifndef _CONSOLE_HPP
#define _CONSOLE_HPP

#include <cmath>
#include <iostream>
#include <string>
#include <vector>

#ifdef __linux__
    #include <signal.h>
    #include <string.h>
    #include <poll.h>
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
inline void InvalidArgCount(int found, int expected);
inline void InvalidArgCount(int found, int expectedLow, int expectedHigh);
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

        #ifdef __linux__
            struct pollfd pfd = { STDIN_FILENO, POLLIN, 0 };
            inputReady = (poll(&pfd, 1, 0));
        #endif
        #ifdef _WIN32
            inputReady = _kbhit();
        #endif

        if (inputReady) {
            if (inputReady == -1) continue;

            #ifdef __linux__
                std::getline(std::cin, input);
            #endif
            #ifdef _WIN32
                key = getch();

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
            #endif

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
        "lock [body] - lock the camera relative to a body\n"
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
        if (args.size() > 3) {
            InvalidArgCount(args.size(), 1, 3);
            return FAIL;
        }
        if (Get(args, universe, window) <= FAIL) {
            std::cout << "Aborting command.\n";
        }
    }

    else if (args[0] == "lock") {
        if (args.size() != 2) {
            InvalidArgCount(args.size(), 2);
            return FAIL;
        }
        if (window.LockCamera(args[1], universe.GetBodies().at(args[1])) == FAIL) {
            std::cout << "Cannot lock camera to this body\n";
            return FAIL;
        }
        std::cout << "locked\n";
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
            return FAIL;
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
        if (args.size() > 7) {
            InvalidArgCount(args.size(), 1, 6);
            return FAIL;
        }
        if (Set(args, universe, window) <= FAIL) {
            std::cout << "aborting command\n";
            return FAIL;
        }
    }

    else if (args[0] == "unlock") {
        if (args.size() != 1) {
            InvalidArgCount(args.size(), 1);
            return FAIL;
        }
        if (window.UnlockCamera() <= FAIL) {
            std::cout << "camera not locked\n";
            return FAIL;
        }
        std::cout << "camera unlocked\n";
    }

    else {
        std::cout << "command not recognized\n";
        return FAIL;
    }

    return SUCCESS;
}


// 

inline int AddBody(Universe& universe) {
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
        body.x = std::stod(tempArgs[0]) * SCALE;
        body.y = std::stod(tempArgs[1]) * SCALE;
        body.z = std::stod(tempArgs[2]) * SCALE;
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
        body.xVel = std::stod(tempArgs[0]) * SCALE;
        body.yVel = std::stod(tempArgs[1]) * SCALE;
        body.zVel = std::stod(tempArgs[2]) * SCALE;
    }
    catch (...) {
        FailedConversion();
        return FAIL;
    }

    std::cout << "Enter radius: ";
    std::getline(std::cin, input);
    try {
        body.radius = std::stod(input) * SCALE * RADIUS_SCALE;
    }
    catch (...) {
        FailedConversion();
        return FAIL;
    }

    std::cout << "Enter mass: ";
    std::getline(std::cin, input);
    try {
        body.mass = std::stod(input) * SCALE;
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
    std::string tempInput;
    std::vector<std::string> input;
    if (args.size() == 1) {
        std::cout << "choices: "
        "bodies\n"
        "body [name]\n"
        "camera\n"
        "cScaling\n"
        "gravityScaling\n"
        "isPaused\n"
        "targetFramerate\n"
        "tickSpeed\n"
        "timeScaling\n"
        "input selection: ";
        std::getline(std::cin, tempInput);
        input = args;
        for (auto i: SplitArguments(tempInput)) {
            input.push_back(i);
        }
    }
    else {
        input = args;
    }

    if (input[1] == "bodies") {
        const auto bodies = universe.GetBodies();
        for (const auto [name, data]: bodies) {
            std::cout << name << "\n";
        }
    }

    else if (input[1] == "body") {
        if (input.size() != 3) {
            InvalidArgCount(input.size(), 3);
            return FAIL;
        }
        if (universe.GetBodies().find(input[2]) == universe.GetBodies().end()) {
            return FAIL;
        }
        const Body body = universe.GetBodies().at(input[2]);
        std::cout << "Body: " << body.name << "\n"
        "Coordinates: " << body.x / SCALE << " " << body.y / SCALE << " " << body.z / SCALE << "\n"
        "Directional Velocities: " << body.xVel / SCALE << " " << body.yVel / SCALE << " " << body.zVel / SCALE << "\n"
        "Velocity: " << sqrt((body.xVel / SCALE * body.xVel / SCALE) + (body.yVel / SCALE * body.yVel / SCALE) + (body.zVel / SCALE * body.zVel / SCALE)) << "\n"
        "Radius: " << body.radius / SCALE / RADIUS_SCALE << "\n"
        "Mass: " << body.mass / SCALE << "\n"
        "Luminosity: " << body.luminosity << "\n"
        "Color: " << body.red << " " << body.green << " " << body.blue << "\n";
    }

    else if (input[1] == "camera") {
        const Camera camera = window.GetCamera();
        std::cout << "Camera:\n"
        "Coordinates: " << camera.x / SCALE << " " << camera.y / SCALE << " " << camera.z / SCALE << "\n"
        "Angles: theta:" << camera.theta << " phi:" << camera.phi << " psi:" << camera.psi << "\n"
        "Movement Speed: " << camera.speed / SCALE << "\n"
        "Rotation Speed: " << camera.rotationSpeed << "\n"
        "Sensitivity: " << camera.sensitivity << "\n";
        if (camera.bodyName != "") {
            std::cout << "Locked Body: " << camera.bodyName << "\n"
            "Distance: " << camera.bodyDistance << "\n";
        }
    }

    else if (input[1] == "cScaling") {
        std::cout << "cScaling = " << universe.GetcScaling() << "\n";
    }

    else if (input[1] == "gravityScaling") {
        std::cout << "gravityScaling = " << universe.GetGravityScaling() << "\n";
    }

    else if (input[1] == "isPaused") {
        std::cout << "isPaused = " << universe.IsPaused() << "\n";
    }

    else if (input[1] == "targetFramerate") {
        std::cout << "targetFramerate = " << window.time.GetTickSpeed() << "\n";
    }

    else if (input[1] == "tickSpeed") {
        std::cout << "tickSpeed = " << universe.GetTickSpeed() << "\n";
    }

    else if (input[1] == "timeScaling") {
        std::cout << "timeScaling = " << universe.GetTimeScaling() << "\n";
    }

    else {
        std::cout << "unrecognized: " << input[1] << "\n";
        return FAIL;
    }

    return SUCCESS;
}

inline void InvalidArgCount(int found, int expected) {
    std::cout << "Invalid argument count. Found: " << found << ", Expected: " << expected << "\n";
}

inline void InvalidArgCount(int found, int expectedLow, int expectedHigh) {
    std::cout << "Invalid argument count. Found: " << found << ", Expected: " << expectedLow << " to " << expectedHigh << "\n";
}

inline int SetBody(std::vector<std::string>& input, Universe& universe) {
    std::string sval;
    if (input.size() == 2) {
        std::cout << "body name: ";
        std::getline(std::cin, sval);
        input.push_back(sval);
    }
    if (universe.GetBodies().find(input[2]) == universe.GetBodies().end()) {
        return FAIL;
    }
    if (input.size() == 3) {
        std::cout << "properties:\n"
        "coordinates (x, y, z)\n"
        "directionalVelocities (xVel, yVel, zVel)\n"
        "velocity\n"
        "radius\n"
        "mass\n"
        "luminosity\n"
        "color (r, g, b)\n"
        "input selection: ";
        std::getline(std::cin, sval);
        input.push_back(sval);
    }
    std::vector<std::string> values;
    if (input.size() == 4) {
        std::cout << "input value(s) (separate with spaces): ";
        std::getline(std::cin, sval);
        values = SplitArguments(sval);
        for (auto i: values) {
            input.push_back(i);
        }
    }

    Body* body = &universe.GetBodiesMut().at(input[2]);

    if (input[3] == "coordinates") {
        if (input.size() != 7) {
            InvalidArgCount(input.size(), 7);
            return FAIL;
        }
        double x, y, z;
        try {
            x = std::stod(input[4]) * SCALE;
            y = std::stod(input[5]) * SCALE;
            z = std::stod(input[6]) * SCALE;
        }
        catch (...) {
            return FAIL;
        }
        body->x = x;
        body->y = y;
        body->z = z;
    }
    else if (input[3] == "directionalVelocities") {
        if (input.size() != 7) {
            InvalidArgCount(input.size(), 7);
            return FAIL;
        }
        double xVel, yVel, zVel;
        try {
            xVel = std::stod(input[4]) * SCALE;
            yVel = std::stod(input[5]) * SCALE;
            zVel = std::stod(input[6]) * SCALE;
        }
        catch (...) {
            return FAIL;
        }
        body->xVel = xVel;
        body->yVel = yVel;
        body->zVel = zVel;
    }
    else if (input[3] == "velocity") {
        if (input.size() != 5) {
            InvalidArgCount(input.size(), 5);
            return FAIL;
        }
        double velocity;
        try {
            velocity = std::stod(input[4]) * SCALE;
        }
        catch (...) {
            return FAIL;
        }
        double currentVelocity = sqrt((body->xVel * body->xVel) + (body->yVel * body->yVel) + (body->zVel * body->zVel));
        double velocityRatio = velocity / currentVelocity;
        body->xVel *= velocityRatio;
        body->yVel *= velocityRatio;
        body->zVel *= velocityRatio;
    }
    else if (input[3] == "radius") {
        if (input.size() != 5) {
            InvalidArgCount(input.size(), 5);
            return FAIL;
        }
        double radius;
        try {
            radius = std::stod(input[4]) * SCALE * RADIUS_SCALE;
        }
        catch (...) {
            return FAIL;
        }
        body->radius = radius;
    }
    else if (input[3] == "mass") {
        if (input.size() != 5) {
            InvalidArgCount(input.size(), 5);
            return FAIL;
        }
        double mass;
        try {
            mass = std::stod(input[4]) * SCALE;
        }
        catch (...) {
            return FAIL;
        }
        body->mass = mass;
    }
    else if (input[3] == "luminosity") {
        if (input.size() != 5) {
            InvalidArgCount(input.size(), 5);
            return FAIL;
        }
        double luminosity;
        try {
            luminosity = std::stod(input[4]);
        }
        catch (...) {
            return FAIL;
        }
        if (luminosity < 0 || luminosity > 1) {
            std::cout << "out of range\n";
            return FAIL;
        }
        body->luminosity = luminosity;
    }
    else if (input[3] == "color") {
        if (input.size() != 7) {
            InvalidArgCount(input.size(), 7);
            return FAIL;
        }
        double r, g, b;
        try {
            r = std::stod(input[4]);
            g = std::stod(input[5]);
            b = std::stod(input[6]);
        }
        catch (...) {
            return FAIL;
        }
        if (r < 0 || r > 1 || g < 0 || g > 1 || b < 0 || b > 1) {
            std::cout << "out of range\n";
            return FAIL;
        }
        body->red = r;
        body->green = g;
        body->blue = b;
    }
    else {
        return FAIL;
    }

    return SUCCESS;
}

inline int SetCamera(std::vector<std::string>& input, Window& window) {
    std::string sval;
    if (input.size() == 2) {
        std::cout << "properties:\n"
        "coordinates (x, y, z)\n"
        "angles (xVel, yVel, zVel)\n"
        "moveSpeed\n"
        "rotationSpeed\n"
        "sensitivity\n"
        "input selection: ";
        std::getline(std::cin, sval);
        input.push_back(sval);
    }
    if (input.size() == 3) {
        std::vector<std::string> values;
        std::cout << "input value(s) (separate with spaces):";
        std::getline(std::cin, sval);
        values = SplitArguments(sval);
        for (auto i: values) {
            input.push_back(i);
        }
    }

    if (input[2] == "coordinates") {
        if (input.size() != 6) {
            InvalidArgCount(input.size(), 6);
            return FAIL;
        }
        double x, y, z;
        try {
            x = std::stod(input[3]) * SCALE;
            y = std::stod(input[4]) * SCALE;
            z = std::stod(input[5]) * SCALE;
        }
        catch (...) {
            return FAIL;
        }
        window.SetCameraPosition(x, y, z);
    }
    else if (input[2] == "angles") {
        if (input.size() != 6) {
            InvalidArgCount(input.size(), 6);
            return FAIL;
        }
        double theta, phi, psi;
        try {
            theta = std::stod(input[3]);
            phi = std::stod(input[4]);
            psi = std::stod(input[5]);
        }
        catch (...) {
            return FAIL;
        }
        window.SetCameraAngle(theta, phi, psi);
    }
    else if (input[2] == "moveSpeed") {
        if (input.size() != 4) {
            InvalidArgCount(input.size(), 4);
            return FAIL;
        }
        double moveSpeed;
        try {
            moveSpeed = std::stod(input[3]) * SCALE;
        }
        catch (...) {
            return FAIL;
        }
        window.SetCameraSpeed(moveSpeed);
    }
    else if (input[2] == "rotationSpeed") {
        if (input.size() != 4) {
            InvalidArgCount(input.size(), 4);
            return FAIL;
        }
        double rotationSpeed;
        try {
            rotationSpeed = std::stod(input[3]);
        }
        catch (...) {
            return FAIL;
        }
        window.SetCameraRotationSpeed(rotationSpeed);
    }
    else if (input[2] == "sensitivity") {
        if (input.size() != 4) {
            InvalidArgCount(input.size(), 4);
            return FAIL;
        }
        double sensitivity;
        try {
            sensitivity = std::stod(input[3]);
        }
        catch (...) {
            return FAIL;
        }
        window.SetCameraSensitivity(sensitivity);
    }
    else {
        return FAIL;
    }

    return SUCCESS;
}

inline int Set(const std::vector<std::string>& args, Universe& universe, Window& window) {
    std::string tempInput;
    std::vector<std::string> input;
    if (args.size() == 1) {
        std::cout << "choices:\n"
        "body [name]\n"
        "camera\n"
        "cScaling [value]\n"
        "gravityScaling [value]\n"
        "targetFramerate [value]\n"
        "tickSpeed [value]\n"
        "timeScaling [value]\n"
        "input selection: ";
        std::getline(std::cin, tempInput);
        input = args;
        for (auto i: SplitArguments(tempInput)) {
            input.push_back(i);
        }
    }
    else {
        input = args;
    }

    std::string sval;
    double value;

    if (input[1] == "body") {
        return SetBody(input, universe);
    }

    else if (input[1] == "camera") {
        return SetCamera(input, window);
    }

    if (input.size() > 3) {
        std::cout << "too many arguments\n";
        return FAIL;
    }

    if (input.size() == 2) {
        std::cout << "input value: ";
        std::getline(std::cin, sval);
    }
    else {
        sval = input[2];
    }
    try { value = std::stod(sval); }
    catch (...) { return FAIL; }

    if (input[1] == "cScaling") {
        return universe.SetcScaling(value);
    }

    if (input[1] == "gravityScaling") {
        return universe.SetGravityScaling(value);
    }

    else if (input[1] == "targetFramerate") {
        return window.time.SetTickSpeed(value);
    }

    else if (input[1] == "tickSpeed") {
        return universe.SetTickSpeed(value);
    }

    else if (input[1] == "timeScaling") {
        return universe.SetTimeScaling(value);
    }

    else {
        std::cout << "unrecognized: " << input[1] << "\n";
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