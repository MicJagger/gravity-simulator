#pragma once
#ifndef _WINDOW_HPP
#define _WINDOW_HPP

#include <vector>

#include "external/include_sdl.hpp"

#include "camera.hpp"
#include "math.hpp"
#include "universe.hpp"

class Window {
    SDL_Window* _window;
    SDL_GLContext _context;
    SDL_Event _windowEvent;

    Camera _camera;

    int _horRes;
    int _vertRes;

    // openGL objects
    // im not putting underscores here because ¯\_(ツ)_/¯

    unsigned int shaderProgram;
    unsigned int VBO;
    unsigned int VAO;
    unsigned int EBO;
public:
    Math _math;

    Window();

    // run before setup
    int OpenWindow();

    // inits opengl
    int SetupOpenGL();

    // getters
    const Camera* GetCamera();

    // setters


    // poll for inputs / events
    std::vector<SDL_Event> PollEvent();

    // camera work

    int SetCameraPosition(double x, double y, double z);
    int SetCameraVelocity(double xVel, double yVel, double zVel);
    int SetCameraAngle(double theta, double phi);

    int ChangeCameraPosition(double x, double y, double z);
    int ChangeCameraVelocity(double xVel, double yVel, double zVel);
    int ChangeCameraAngle(double theta, double phi);

    // draw current frame
    int DrawFrame(Universe* universe);
};

#endif