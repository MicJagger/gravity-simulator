#include <iostream>
#include <mutex>
#include <set>
#include <string>
#include <thread>
#include <vector>

#include "body.hpp"
#include "console.hpp"
#include "definitions.hpp"
#include "math.hpp"
#include "universe.hpp"
#include "window.hpp"

// handles the physics of all the objects
void PhysicsThread(int& sigIn, int& sigOut, Universe& universe);
// handles drawing of the frames
void RenderThread(int& sigIn, int& sigOut, Universe& universe, Window& window);

// handles user inputs
int main(int argc, char* argv[]) {
    int failVal = 0;
    std::mutex mtx;

    Math math;
    math.SetTickSpeed(100);
    Universe universe;
    Window window;
    window.OpenWindow();

    int physIn = 1, physOut = 1;
    std::thread physicsThread = std::thread(PhysicsThread, std::ref(physIn), std::ref(physOut), std::ref(universe));
    int renderIn = 1, renderOut = 1;
    std::thread renderThread = std::thread(RenderThread, std::ref(renderIn), std::ref(renderOut), std::ref(universe), std::ref(window));
    int consoleIn = 1, consoleOut = 1;
    std::thread consoleThread = std::thread(ConsoleThread, std::ref(consoleIn), std::ref(consoleOut), std::ref(universe), std::ref(window));
    
    window.SetCameraPosition(0.0, -5.0, 0.0);

    Body first;
    first._radius = 1;
    universe.AddBody(first);

    Body second;
    second._x = 10;
    second._y = 10;
    second._radius = 2;
    universe.AddBody(second);

    double cameraSpeed = 10.0;
    double cameraRotationSpeed = 120.0;

    int key;
    std::set<int> keys;
    bool forward = false, back = false, left = false, right = false;
    bool running = true;
    while (running) {
        math.TickStart();
        
        std::vector<SDL_Event> events = window.PollEvent();
        for (SDL_Event event: events) {
            switch (event.type) {
                // Press Window X
                case SDL_QUIT:
                    physIn = 0;
                    renderIn = 0;
                    consoleIn = 0;
                    failVal = 0;
                    running = false;
                    std::cout << "main called quit\n";
                    break;
                case SDL_KEYDOWN:
                    key = event.key.keysym.sym;
                    keys.emplace(key);
                    switch (key) {
                        case 'w':
                            forward = true;
                            break;
                        case 's':
                            back = true;
                            break;
                        case 'a':
                            left = true;
                            break;
                        case 'd':
                            right = true;
                            break;
                    }
                    break;
                case SDL_KEYUP:
                    key = event.key.keysym.sym;
                    keys.erase(key);
                    switch (key) {
                        case 'w':
                            forward = false;
                            break;
                        case 's':
                            back = false;
                            break;
                        case 'a':
                            left = false;
                            break;
                        case 'd':
                            right = false;
                            break;
                    }
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    break;
                case SDL_MOUSEBUTTONUP:
                    break;
            }
        }

        mtx.lock();
        for (int key: keys) {
            switch (key) {
                case 'w':
                    if (left || right) {
                        window.MoveCamera(sqrt2o2 * cameraSpeed / math.GetTickSpeed(), 0.0, 0.0);
                    }
                    else {
                        window.MoveCamera(cameraSpeed / math.GetTickSpeed(), 0.0, 0.0);
                    }
                    break;
                case 's':
                    if (left || right) {
                        window.MoveCamera(sqrt2o2 * -cameraSpeed / math.GetTickSpeed(), 0.0, 0.0);
                    }
                    else {
                        window.MoveCamera(-cameraSpeed / math.GetTickSpeed(), 0.0, 0.0);
                    }
                    break;
                case 'a':
                    if (forward || back) {
                        window.MoveCamera(0.0, sqrt2o2 * -cameraSpeed / math.GetTickSpeed(), 0.0);
                    }
                    else {
                        window.MoveCamera(0.0, -cameraSpeed / math.GetTickSpeed(), 0.0);
                    }
                    break;
                case 'd':
                    if (forward || back) {
                        window.MoveCamera(0.0, sqrt2o2 * cameraSpeed / math.GetTickSpeed(), 0.0);
                    }
                    else {
                        window.MoveCamera(0.0, cameraSpeed / math.GetTickSpeed(), 0.0);
                    }
                    break;
                case SDLK_SPACE:
                    window.MoveCamera(0.0, 0.0, cameraSpeed / math.GetTickSpeed());
                    break;
                case SDLK_LCTRL:
                    window.MoveCamera(0.0, 0.0, -cameraSpeed / math.GetTickSpeed());
                    break;
                case SDLK_UP:
                    window.ChangeCameraAngle(0.0f, -cameraRotationSpeed / math.GetTickSpeed(), 0.0f);
                    break;
                case SDLK_DOWN:
                    window.ChangeCameraAngle(0.0f, cameraRotationSpeed / math.GetTickSpeed(), 0.0f);
                    break;
                case SDLK_LEFT:
                    window.ChangeCameraAngle(cameraRotationSpeed / math.GetTickSpeed(), 0.0f, 0.0f);
                    break;
                case SDLK_RIGHT:
                    window.ChangeCameraAngle(-cameraRotationSpeed / math.GetTickSpeed(), 0.0f, 0.0f);
                    break;
            }
        }
        mtx.unlock();

        if (physOut <= SUCCESS) {
            std::cout << "physics called quit\n";
            renderIn = physOut;
            consoleIn = physOut;
            failVal = physOut;
            break;
        }
        if (renderOut <= SUCCESS) {
            std::cout << "render called quit\n";
            physIn = renderOut;
            consoleIn = renderOut;
            failVal = renderOut;
            break;
        }
        if (consoleOut <= SUCCESS) {
            std::cout << "console called quit\n";
            physIn = consoleOut;
            renderIn = consoleOut;
            failVal = consoleOut;
            break;
        }
        math.TickEndAndSleep();
    }
    physicsThread.join();
    renderThread.join();
    consoleThread.join();

    return failVal;
}


// threads

void PhysicsThread(int& sigIn, int& sigOut, Universe& universe) {
    int failVal = 0;
    while (true) {
        universe._math.TickStart();
        universe.CalculateTick();
        if (sigIn <= SUCCESS) {
            break;
        }
        universe._math.TickEndAndSleep();
    }
}

void RenderThread(int& sigIn, int& sigOut, Universe& universe, Window& window) {
    int failVal = 0;
    if ((failVal = window.SetupOpenGL()) < SUCCESS) {
        sigOut = failVal;
        return;
    }
    while (true) {
        window._math.TickStart();
        window.DrawFrame(universe);
        if (sigIn <= SUCCESS) {
            break;
        }
        window._math.TickEndAndSleep();
    }
}
