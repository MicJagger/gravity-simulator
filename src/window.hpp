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
    Math math;

    Window();

    // inits window and opengl
    int Setup();

    // poll for inputs / events
    std::vector<unsigned int> PollEvent();

    // draw current frame
    int DrawFrame();
};

#endif