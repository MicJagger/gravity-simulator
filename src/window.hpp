#pragma once
#ifndef _WINDOW_HPP
#define _WINDOW_HPP

#include "external/include_sdl.hpp"

class Window {
    SDL_Window* _window;
    SDL_GLContext _context;

    int _horRes;
    int _vertRes;

    // functions
public:
    Window();

    int Setup();
};

#endif