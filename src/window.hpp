#pragma once
#ifndef _WINDOW_HPP
#define _WINDOW_HPP

#include <mutex>
#include <vector>

#include "external/include_sdl.hpp"

#include "camera.hpp"
#include "math.hpp"
#include "universe.hpp"

class Window {
    SDL_Window* _window;
    SDL_GLContext _context;
    SDL_Event _windowEvent;
    
    std::mutex _mtx;

    Camera _camera;

    int _horRes;
    int _vertRes;
    float _fov;

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
    const Camera& GetCamera();

    // setters


    // poll for inputs / events
    std::vector<SDL_Event> PollEvent();

    // camera work

    int SetCameraPosition(const double& x, const double& y, const double& z);
    int SetCameraAngle(const float& theta, const float& phi, const float& psi);

    int ChangeCameraPosition(const double& x, const double& y, const double& z);
    int ChangeCameraAngle(const float& theta, const float& phi, const float& psi);

    int MoveCamera(const double& forward, const double& right, const double& up);

    // draw current frame
    int DrawFrame(const Universe& universe);
};

#endif