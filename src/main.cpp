#include <iostream>
#include <thread>
#include <vector>

#include "body.hpp"
#include "camera.hpp"
#include "definitions.hpp"
#include "math.hpp"
#include "universe.hpp"
#include "window.hpp"

// handles the physics of all the objects
void PhysicsThread(int* sigIn, int* sigOut, Universe* universe);
// handles drawing of the frames
void RenderThread(int* sigIn, int* sigOut, Universe* universe, Window* window, Camera* camera);

// handles user inputs
int main(int argc, char* argv[]) {
    int failVal = 0;

    Camera camera;
    Universe universe;
    Window window;
    window.OpenWindow();
    int physIn, physOut = 1;
    std::thread physicsThread = std::thread(PhysicsThread, &physIn, &physOut, &universe);
    int renderIn, renderOut = 1;
    std::thread renderThread = std::thread(RenderThread, &renderIn, &renderOut, &universe, &window, &camera);
    int returnVal = 0;
    
    bool running = true;
    while (running) {
        std::vector<unsigned int> event = window.PollEvent();
        for (int i = 0; i < event.size(); i++) {
            switch (event[i]) {
                // Press Window X
                case SDL_QUIT:
                    physIn = 0;
                    renderIn = 0;
                    returnVal = 0;
                    running = false;
                    break;
                case SDL_KEYDOWN:
                    break;
                case SDL_KEYUP:
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    break;
                case SDL_MOUSEBUTTONUP:
                    break;
            }
        }

        if (physOut <= SUCCESS) {
            renderIn = physOut;
            returnVal = physOut;
            break;
        }
        if (renderOut <= SUCCESS) {
            physIn = renderOut;
            returnVal = renderOut;
            break;
        }
        Math::sleep(0.010);
    }
    physicsThread.join();
    renderThread.join();

    return returnVal;
}

void PhysicsThread(int* sigIn, int* sigOut, Universe* universe) {
    while (true) {
        universe->_math.TickStart();
        if (*sigIn <= SUCCESS) {
            break;
        }
        universe->_math.TickEndAndSleep();
    }
}

void RenderThread(int* sigIn, int* sigOut, Universe* universe, Window* window, Camera* camera) {
    int failVal = 0;
    if ((failVal = window->SetupOpenGL()) < SUCCESS) {
        *sigOut = failVal;
        return;
    }
    while (true) {
        window->_math.TickStart();
        window->DrawFrame();
        if (*sigIn <= SUCCESS) {
            break;
        }
        window->_math.TickEndAndSleep();
    }
}
