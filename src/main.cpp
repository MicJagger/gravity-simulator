#include <iostream>
#include <string>
#include <thread>
#include <vector>

#include "body.hpp"
#include "console.hpp"
#include "definitions.hpp"
#include "math.hpp"
#include "universe.hpp"
#include "window.hpp"

//int HandleKeypress();
//int HandleKeyunpress();

// handles the physics of all the objects
void PhysicsThread(int* sigIn, int* sigOut, Universe* universe);
// handles drawing of the frames
void RenderThread(int* sigIn, int* sigOut, Universe* universe, Window* window);

// handles user inputs
int main(int argc, char* argv[]) {
    int failVal = 0;

    Universe universe;
    Window window;
    window.OpenWindow();
    int physIn = 1, physOut = 1;
    std::thread physicsThread = std::thread(PhysicsThread, &physIn, &physOut, &universe);
    int renderIn = 1, renderOut = 1;
    std::thread renderThread = std::thread(RenderThread, &renderIn, &renderOut, &universe, &window);
    int consoleIn = 1, consoleOut = 1;
    std::thread consoleThread = std::thread(ConsoleThread, &consoleIn, &consoleOut, &universe, &window);
    int returnVal = 0;
    
    Body first;
    first._radius = 0.5;
    universe.AddBody(first);

    unsigned int key;
    //bool keys[322];
    //std::fill_n(keys, 322, false);
    bool running = true;
    while (running) {
        std::vector<SDL_Event> event = window.PollEvent();
        for (int i = 0; i < event.size(); i++) {
            switch (event[i].type) {
                // Press Window X
                case SDL_QUIT:
                    physIn = 0;
                    renderIn = 0;
                    consoleIn = 0;
                    returnVal = 0;
                    running = false;
                    std::cout << "main called quit\n";
                    break;
                case SDL_KEYDOWN:
                    key = event[i].key.keysym.sym;
                    //keys[key] = true;
                    if (key == 'w') {
                        window.MoveCamera(1.0, 0.0, 0.0);
                    }
                    else if (key == 's') {
                        window.MoveCamera(-1.0, 0.0, 0.0);
                    }
                    else if (key == 'a') {
                        window.MoveCamera(0.0, -1.0, 0.0);
                    }
                    else if (key == 'd') {
                        window.MoveCamera(0.0, 1.0, 0.0);
                    }
                    else if (key == SDLK_SPACE) {
                        window.MoveCamera(0.0, 0.0, 1.0);
                    }
                    else if (key == SDLK_LCTRL) {
                        window.MoveCamera(0.0, 0.0, -1.0);
                    }
                    /*else if (key == 'q') {
                        window.ChangeCameraAngle(0.0f, 0.0f, 5.0f);
                    }
                    else if (key == 'e') {
                        window.ChangeCameraAngle(0.0f, 0.0f, -5.0f);
                    }*/
                    else if (key == SDLK_UP) {
                        window.ChangeCameraAngle(0.0f, -5.0f, 0.0f);
                    }
                    else if (key == SDLK_DOWN) {
                        window.ChangeCameraAngle(0.0f, 5.0f, 0.0f);
                    }
                    else if (key == SDLK_LEFT) {
                        window.ChangeCameraAngle(5.0f, 0.0f, 0.0f);
                    }
                    else if (key == SDLK_RIGHT) {
                        window.ChangeCameraAngle(-5.0f, 0.0f, 0.0f);
                    }
                    //HandleKeypress(key, keys, universe, window, camera);
                    break;
                case SDL_KEYUP:
                    key = event[i].key.keysym.sym;
                    //keys[key] = false;
                    //HandleKeyunpress(key, keys, universe, window, camera);
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    break;
                case SDL_MOUSEBUTTONUP:
                    break;
            }
        }

        if (physOut <= SUCCESS) {
            std::cout << "physics called quit\n";
            renderIn = physOut;
            consoleIn = physOut;
            returnVal = physOut;
            break;
        }
        if (renderOut <= SUCCESS) {
            std::cout << "render called quit\n";
            physIn = renderOut;
            consoleIn = renderOut;
            returnVal = renderOut;
            break;
        }
        if (consoleOut <= SUCCESS) {
            std::cout << "console called quit\n";
            physIn = consoleOut;
            renderIn = consoleOut;
            returnVal = consoleOut;
            break;
        }
        Math::sleep(0.010);
    }
    physicsThread.join();
    renderThread.join();
    consoleThread.join();

    return returnVal;
}

//int HandleKeypress() {}

//int HandleKeyunpress() {}

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
    int failVal = 0;
    if ((failVal = window->SetupOpenGL()) < SUCCESS) {
        *sigOut = failVal;
        return;
    }
    while (true) {
        window->_math.TickStart();
        window->DrawFrame(universe);
        if (*sigIn <= SUCCESS) {
            break;
        }
        window->_math.TickEndAndSleep();
    }
}
