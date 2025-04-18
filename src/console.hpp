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
#include "math.hpp"
#include "universe.hpp"
#include "window.hpp"

std::vector<std::string> SplitArguments(std::string input);

// TODO: implement linux version for nonblocking IO
#ifdef __linux__
// handles console I/O
void ConsoleThread(int* sigIn, int* sigOut, Universe* universe, Window* window, Camera* camera) {
    std::string input;
    std::vector<std::string> args;
    while (true) {
        if () {
            char key = ();
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
            if (args.size() == 0) {
                continue;
            }
            // commands
            if (args[0] == "quit") {
                *sigOut = 0;
                return;
            }
        }
        if (*sigIn <= SUCCESS) {
            break;
        }
        Math::sleep(0.050);
    }
}
#endif

#ifdef _WIN32
// handles console I/O
void ConsoleThread(int* sigIn, int* sigOut, Universe* universe, Window* window) {
    std::string input;
    std::vector<std::string> args;
    while (true) {
        if (_kbhit()) {
            char key = _getch();
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
            if (args.size() == 0) {
                continue;
            }

            // commands
            if (args[0] == "help") {
                std::cout << "List of commands: \n" << 
                "quit - end program\n";
            }
            else if (args[0] == "quit") {
                *sigOut = 0;
                return;
            }
            else {
                std::cout << "Command not recognized.\n";
            }
        }
        if (*sigIn <= SUCCESS) {
            break;
        }
        Math::sleep(0.050);
    }
}
#endif

std::vector<std::string> SplitArguments(std::string input) {
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