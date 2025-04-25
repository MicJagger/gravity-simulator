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

std::vector<std::string> SplitArguments(std::string input);

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
            if (args.size() == 0) {
                continue;
            }

            // commands
            if (args[0] == "help") {
                std::cout << "List of commands: \n" << 
                "pause - pause universe\n" <<
                "quit - end program\n" <<
                "resume - unpause universe\n";
            }

            else if (args[0] == "pause") {
                if (universe.Pause()) {
                    std::cout << "paused\n";
                }
                else {
                    std::cout << "already paused\n";
                }
            }

            else if (args[0] == "quit") {
                sigOut = 0;
                return;
            }

            else if (args[0] == "resume") {
                if (universe.Unpause()) {
                    std::cout << "resumed\n";
                }
                else {
                    std::cout << "already running\n";
                }
            }

            else {
                std::cout << "Command not recognized.\n";
            }
        }
        if (sigIn <= SUCCESS) {
            break;
        }
        Time::sleep(0.050);
    }
}

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