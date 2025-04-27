#pragma once
#ifndef _WINDOW_HPP
#define _WINDOW_HPP

#include <mutex>
#include <string>
#include <vector>

#include "external/include_sdl.hpp"

#include "camera.hpp"
#include "time.hpp"
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

    // openGL "objects"

    unsigned int _shaderProgram;
    // array object
    unsigned int _VAO;
    // vertices
    unsigned int _VBO;
    // elements
    unsigned int _EBO;
public:
    Time time;

    Window();

    // run before setup
    int OpenWindow();

    // inits opengl
    int SetupOpenGL();

    // getters
    const Camera& GetCamera() const;
    bool CameraLocked() const;

    // setters


    // poll for inputs / events
    std::vector<SDL_Event> PollEvent();

    // camera work

    int SetCameraPosition(const double& x, const double& y, const double& z);
    int SetCameraAngle(const float& theta, const float& phi, const float& psi);

    int ChangeCameraPosition(const double& x, const double& y, const double& z);
    int ChangeCameraAngle(const float& theta, const float& phi, const float& psi);

    int MoveCamera(const double& forward, const double& right, const double& up);

    // locking

    int LockCamera(const std::string& bodyName);
    int LockCamera(const std::string& bodyName, const Body& body);
    int UnlockCamera();
    int SetCameraBodyDistance(const double& distance);
    int ChangeCameraBodyDistance(const double& forward);

    // draw current frame
    int DrawFrame(const Universe& universe);
};

#endif