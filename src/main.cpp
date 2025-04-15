#include <iostream>
#include <thread>
#include <vector>

#include "body.hpp"
#include "definitions.hpp"
#include "math.hpp"
#include "universe.hpp"
#include "window.hpp"

// handles the physics of all the objects
void PhysicsThread(int* sigIn, int* sigOut, Universe* universe);
// handles drawing of the frames
void RenderThread(int* sigIn, int* sigOut, Universe* universe, Window* window);

// handles user inputs
int main(int argc, char* argv[]) {
    int failVal = 0;

    Universe universe;
    Window window;
    if ((failVal = window.Setup()) < SUCCESS) {
        return failVal;
    }
    int physIn, physOut = 1;
    std::thread physicsThread = std::thread(PhysicsThread, &physIn, &physOut, &universe);
    int renderIn, renderOut = 1;
    std::thread renderThread = std::thread(RenderThread, &renderIn, &renderOut, &universe, &window);
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

void RenderThread(int* sigIn, int* sigOut, Universe* universe, Window* window) {
    while (true) {
        window->_math.TickStart();
        if (*sigIn <= SUCCESS) {
            break;
        }
        window->_math.TickEndAndSleep();
    }
}
