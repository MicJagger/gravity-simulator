#include <iostream>
#include <mutex>
#include <set>
#include <string>
#include <thread>
#include <vector>

#include "body.hpp"
#include "console.hpp"
#include "definitions.hpp"
#include "time.hpp"
#include "universe.hpp"
#include "values.hpp"
#include "window.hpp"

// handles the physics of all the objects
void PhysicsThread(int& sigIn, int& sigOut, Universe& universe);
// handles drawing of the frames
void RenderThread(int& sigIn, int& sigOut, Universe& universe, Window& window);

void SpawnSolarSystemScaled(Universe& universe, double& scaleValue, double& radiusScale);

// handles user inputs
int main(int argc, char* argv[]) {
    int failVal = 0;
    std::mutex mtx;

    Time time;
    time.SetTickSpeed(100);
    Universe universe;
    Window window;
    window.OpenWindow();

    int physIn = 1, physOut = 1;
    std::thread physicsThread = std::thread(PhysicsThread, std::ref(physIn), std::ref(physOut), std::ref(universe));
    int renderIn = 1, renderOut = 1;
    std::thread renderThread = std::thread(RenderThread, std::ref(renderIn), std::ref(renderOut), std::ref(universe), std::ref(window));
    int consoleIn = 1, consoleOut = 1;
    std::thread consoleThread = std::thread(ConsoleThread, std::ref(consoleIn), std::ref(consoleOut), std::ref(universe), std::ref(window));
    
    window.SetCameraPosition(-100.0, -100.0, 0);

    universe.SetTimeScaling(86400 * 7);
    universe.SetGravityScaling(1);

    double scale = 1e-9;
    double radiusScale = 40.0;
    SpawnSolarSystemScaled(universe, scale, radiusScale);

    double cameraSpeed = c * scale * 1000;
    double cameraRotationSpeed = 120.0;

    int key;
    std::set<int> keys;
    bool forward = false, back = false, left = false, right = false;
    bool running = true;
    while (running) {
        time.TickStart();
        
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
        double tickMove = cameraSpeed / time.GetTickSpeed();
        double tickRotate = cameraRotationSpeed / time.GetTickSpeed();
        for (int key: keys) {
            switch (key) {
                case 'w':
                    if (left || right) {
                        window.MoveCamera(sqrt2o2 * tickMove, 0.0, 0.0);
                    }
                    else {
                        window.MoveCamera(tickMove, 0.0, 0.0);
                    }
                    break;
                case 's':
                    if (left || right) {
                        window.MoveCamera(sqrt2o2 * -tickMove, 0.0, 0.0);
                    }
                    else {
                        window.MoveCamera(-tickMove, 0.0, 0.0);
                    }
                    break;
                case 'a':
                    if (forward || back) {
                        window.MoveCamera(0.0, sqrt2o2 * -tickMove, 0.0);
                    }
                    else {
                        window.MoveCamera(0.0, -tickMove, 0.0);
                    }
                    break;
                case 'd':
                    if (forward || back) {
                        window.MoveCamera(0.0, sqrt2o2 * tickMove, 0.0);
                    }
                    else {
                        window.MoveCamera(0.0, tickMove, 0.0);
                    }
                    break;
                case SDLK_SPACE:
                    window.MoveCamera(0.0, 0.0, tickMove);
                    break;
                case SDLK_LCTRL:
                    window.MoveCamera(0.0, 0.0, -tickMove);
                    break;
                case SDLK_UP:
                    window.ChangeCameraAngle(0.0f, -tickRotate, 0.0f);
                    break;
                case SDLK_DOWN:
                    window.ChangeCameraAngle(0.0f, tickRotate, 0.0f);
                    break;
                case SDLK_LEFT:
                    window.ChangeCameraAngle(tickRotate, 0.0f, 0.0f);
                    break;
                case SDLK_RIGHT:
                    window.ChangeCameraAngle(-tickRotate, 0.0f, 0.0f);
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
        time.TickEndAndSleep();
    }
    physicsThread.join();
    renderThread.join();
    consoleThread.join();

    return failVal;
}

// threads

void PhysicsThread(int& sigIn, int& sigOut, Universe& universe) {
    int failVal = 0;
    universe.SetTickSpeed(100);
    while (true) {
        universe.time.TickStart();
        if (!universe.IsPaused()) {
            universe.CalculateTick();
        }
        if (sigIn <= SUCCESS) {
            break;
        }
        universe.time.TickEndAndSleep();
    }
}

void RenderThread(int& sigIn, int& sigOut, Universe& universe, Window& window) {
    int failVal = 0;
    if ((failVal = window.SetupOpenGL()) < SUCCESS) {
        sigOut = failVal;
        return;
    }
    while (true) {
        window.time.TickStart();
        window.DrawFrame(universe);
        if (sigIn <= SUCCESS) {
            break;
        }
        window.time.TickEndAndSleep();
    }
}


// testing / example

void SpawnSolarSystemScaled(Universe& universe, double& scaleValue, double& radiusScale) {
    std::mutex mtx;
    mtx.lock();

    universe.SetcScaling(scaleValue);

    Body body;
    // mass is scaled^3 to account for gravity being 1/r^2 and distance being r * scale
    double massScaling = scaleValue * scaleValue * scaleValue;
    // sun
    body.name = "sol";
    body.radius = sunRadius * scaleValue * radiusScale;
    body.mass = sunMass * massScaling;
    body.x = -sunDistance * scaleValue;
    body.yVel = sunVelocity * scaleValue;
    body.luminosity = 1.0;
    universe.AddBody(body);
    body.luminosity = 0.2f;

    // mercury
    body.name = "mercury";
    body.radius = mercuryRadius * scaleValue * radiusScale;
    body.mass = mercuryMass * massScaling;
    body.x = -mercuryDistance * scaleValue;
    body.yVel = mercuryVelocity * scaleValue;
    universe.AddBody(body);

    // venus
    body.name = "venus";
    body.radius = venusRadius * scaleValue * radiusScale;
    body.mass = venusMass * massScaling;
    body.x = -venusDistance * scaleValue;
    body.yVel = venusVelocity * scaleValue;
    universe.AddBody(body);

    // earth
    body.name = "earth";
    body.radius = earthRadius * scaleValue * radiusScale;
    body.mass = earthMass * massScaling;
    body.x = -earthDistance * scaleValue;
    body.yVel = earthVelocity * scaleValue;
    universe.AddBody(body);

    // mars
    body.name = "mars";
    body.radius = marsRadius * scaleValue * radiusScale;
    body.mass = marsMass * massScaling;
    body.x = -marsDistance * scaleValue;
    body.yVel = marsVelocity * scaleValue;
    universe.AddBody(body);

    // jupiter
    body.name = "jupiter";
    body.radius = jupiterRadius * scaleValue * radiusScale;
    body.mass = jupiterMass * massScaling;
    body.x = -jupiterDistance * scaleValue;
    body.yVel = jupiterVelocity * scaleValue;
    universe.AddBody(body);

    // saturn
    body.name = "saturn";
    body.radius = saturnRadius * scaleValue * radiusScale;
    body.mass = saturnMass * massScaling;
    body.x = -saturnDistance * scaleValue;
    body.yVel = saturnVelocity * scaleValue;
    universe.AddBody(body);

    // uranus
    body.name = "uranus";
    body.radius = uranusRadius * scaleValue * radiusScale;
    body.mass = uranusMass * massScaling;
    body.x = -uranusDistance * scaleValue;
    body.yVel = uranusVelocity * scaleValue;
    universe.AddBody(body);

    // neptune
    body.name = "neptune";
    body.radius = neptuneRadius * scaleValue * radiusScale;
    body.mass = neptuneMass * massScaling;
    body.x = -neptuneDistance * scaleValue;
    body.yVel = neptuneVelocity * scaleValue;
    universe.AddBody(body);

    // moon
    body.name = "luna";
    body.radius = moonRadius * scaleValue * radiusScale;
    body.mass = moonMass * massScaling;
    body.x = -(earthDistance + moonDistance) * scaleValue;
    body.yVel = (earthVelocity + moonVelocity) * scaleValue;
    universe.AddBody(body);

    mtx.unlock();
}
