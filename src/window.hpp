#pragma once
#ifndef _WINDOW_HPP
#define _WINDOW_HPP

#include <vector>

#include "external/include_sdl.hpp"

#include "math.hpp"

class Window {
    SDL_Window* _window;
    SDL_GLContext _context;
    SDL_Event _windowEvent;

    int _horRes;
    int _vertRes;
public:
    Math _math;

    Window();

    // run before setup
    int OpenWindow();

    // inits opengl
    int SetupOpenGL();

    // poll for inputs / events
    std::vector<unsigned int> PollEvent();

    // draw current frame
    int DrawFrame();
};

#endif