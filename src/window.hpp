#pragma once
#ifndef _WINDOW_HPP
#define _WINDOW_HPP

#include <mutex>
#include <string>
#include <vector>

#include <SDL.h>

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
    double GetCameraSpeed() const;
    double GetCameraRotationSpeed() const;
    double GetCameraSensitivity() const;

    // setters

    int SetCameraSpeed(double speed);
    int SetCameraRotationSpeed(double rotationSpeed);
    int SetCameraSensitivity(double sensitivity);

    // poll for inputs / events
    std::vector<SDL_Event> PollEvent();

    // camera work

    int SetCameraPosition(double x, double y, double z);
    int SetCameraAngle(float theta, float phi, float psi);

    int ChangeCameraPosition(double x, double y, double z);
    int ChangeCameraAngle(float theta, float phi, float psi);

    int MoveCamera(double forward, double right, double up);

    // locking

    int LockCamera(const std::string& bodyName);
    int LockCamera(const std::string& bodyName, const Body& body);
    int UnlockCamera();
    int SetCameraBodyDistance(double distance);
    int ChangeCameraBodyDistance(double forward);

    // draw current frame
    int DrawFrame(const Universe& universe);
};

#endif