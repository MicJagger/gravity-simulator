#include "window.hpp"

#include <iostream>
#include <map>
#include <vector>

#include "../glm/glm/glm.hpp"
#include "../glm/glm/gtc/matrix_transform.hpp"
#include "../glm/glm/gtc/type_ptr.hpp"
#include "external/include_glad.hpp"
#include "external/include_sdl.hpp"

#include "definitions.hpp"
#include "math.hpp"

Window::Window() {
    _horRes = 1280;
    _vertRes = 720;
}

int Window::OpenWindow() {
    // window creation
    _window = SDL_CreateWindow("gravity-simulator", 
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
        _horRes, _vertRes, 
        SDL_WINDOW_OPENGL);
    if (_window == NULL) {
        std::cout << "failed to create window\n";
        return FAIL;
    }
    return SUCCESS;
}

int Window::SetupOpenGL() {
    SDL_Init(SDL_INIT_EVERYTHING);
    // set some attributes
    SDL_GL_LoadLibrary(NULL);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3); // glad version 3.3
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE); // core
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
    if (!(_context = SDL_GL_CreateContext(_window))) {
        return FAIL;
    }
    if (SDL_GL_MakeCurrent(_window, _context) < 0) {
        return FAIL;
    }
    gladLoadGLLoader(SDL_GL_GetProcAddress);

    // Use v-sync
    // SDL_GL_SetSwapInterval(1);

    // Disable depth test and face culling.
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);

    // tell opengl window size
    //SDL_GetWindowSize(_window, &w, &h);
    glViewport(0, 0, _horRes, _vertRes);

    constexpr auto vertexShaderSource = R"(
        #version 330 core

        layout (location = 0) in vec3 aPos;
        layout (location = 1) in vec3 aColor;

        uniform mat4 modelMatrix;

        out vec3 vertexColor;

        void main() {
            gl_Position = modelMatrix * vec4(aPos.x, aPos.y, aPos.z, 1.0);
            vertexColor = aColor;
        }
    )";
    
    constexpr auto fragmentShaderSource = R"(
        #version 330 core

        in vec3 vertexColor;

        out vec4 FragColor;

        void main() {
            FragColor = vec4(vertexColor.r, vertexColor.g, vertexColor.b, 1.0f);
        }
    )";

    // create shader object
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);

    // attach source to shader object and compile
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // if compilation failed
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, sizeof(infoLog), NULL, infoLog);
        std::cout << "vertex compilation failed\n" << infoLog << std::endl;
        return FAIL;
    }

    // setup and compile fragment shader
    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // if compilation failed (again)
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, sizeof(infoLog), NULL, infoLog);
        std::cout << "fragment compilation failed\n" << infoLog << std::endl;
        return FAIL;
    }

    // create shader program to merge two pieces
    shaderProgram = glCreateProgram();

    // creates and links
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check for failure (even more)
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "shaderProgram compilation failed\n" << infoLog << std::endl;
        return FAIL;
    }

    // use this program
    glUseProgram(shaderProgram);

    // delete old objects
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // setup other stuffs

    glGenVertexArrays(1, &VAO);

    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    
    // bind Vertex Array Object
    glBindVertexArray(VAO);
    // set it to be for vertices
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // bind element buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    // set vertex attributes pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    return SUCCESS;
}

const Camera *Window::GetCamera() {
    return &_camera;
}

std::vector<SDL_Event> Window::PollEvent() {
    std::vector<SDL_Event> events;
    while (SDL_PollEvent(&_windowEvent)) {
        events.push_back(_windowEvent);
    }
    return events;
}

int Window::SetCameraPosition(double x, double y, double z) {
    _camera._x = x;
    _camera._y = y;
    _camera._z = z;
    return SUCCESS;
}

int Window::SetCameraVelocity(double xVel, double yVel, double zVel) {
    _camera._x = xVel;
    _camera._y = yVel;
    _camera._z = zVel;
    return SUCCESS;
}

int Window::SetCameraAngle(double theta, double phi) {
    _camera._theta = theta;
    _camera._phi = phi;
    return SUCCESS;
}

int Window::ChangeCameraPosition(double x, double y, double z) {
    _camera._x += x;
    _camera._y += y;
    _camera._z += z;
    return SUCCESS;
}

int Window::ChangeCameraVelocity(double xVel, double yVel, double zVel) {
    _camera._x += xVel;
    _camera._y += yVel;
    _camera._z += zVel;
    return SUCCESS;
}

int Window::ChangeCameraAngle(double theta, double phi) {
    _camera._theta += theta;
    _camera._phi += phi;
    return SUCCESS;
}

#include <cmath>

void DrawSphere(const Body* body, Camera* camera, std::vector<float>& vertexData) {

}

int Window::DrawFrame(Universe* universe) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    const std::map<long long, Body>* bodies = universe->GetBodies();
    std::vector<float> vertexData = {
        -0.75f, -0.75f, 0.2f, 1.0f, 0.0f, 0.0f,
        0.75f, -0.75f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.0f,  0.75f, 0.0f, 0.0f, 0.0f, 1.0f,
        0.5f,  0.0f, 0.5f, 0.5f, 0.0f, 0.0f,
        0.75f,  0.9f, 0.5f, 0.0f, 0.5f, 0.0f,
        0.5f,  0.95f, 0.5f, 0.0f, 0.0f, 0.5f
    };
    std::vector<unsigned int> elementData = {
        0,
        1,
        2,
        3,
        4,
        5
    };

    for (auto iter = bodies->begin(); iter != bodies->end(); iter++) {
        //DrawSphere(&iter->second, camera, vertexData);
    }

    auto modelMatrix = glm::mat4(1.0f);

    modelMatrix = glm::rotate(modelMatrix, glm::radians((float)_camera._theta), glm::vec3(0.0f, 0.0f, 1.0f));
    modelMatrix = glm::rotate(modelMatrix, glm::radians((float)_camera._phi), glm::vec3(1.0f, 0.0f, 0.0f));

    auto modelMatrixLocation = glGetUniformLocation(shaderProgram, "modelMatrix");
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    // copy vertex data into buffer
    glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), vertexData.data(), GL_STREAM_DRAW);
    // copy element data buffer
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, elementData.size() * sizeof(unsigned int), elementData.data(), GL_STREAM_DRAW);

    glBindVertexArray(VAO);

    glDrawElements(GL_TRIANGLES, elementData.size() * sizeof(unsigned int), GL_UNSIGNED_INT, 0);

    SDL_GL_SwapWindow(_window);
    return SUCCESS;
}

/* 
// test vertex data
float vertices[] = {
    -0.75f, -0.75f, 0.0f, 1.0f, 0.0f, 0.0f,
    0.75f, -0.75f, 0.0f, 0.0f, 1.0f, 0.0f,
    0.0f,  0.75f, 0.0f, 0.0f, 0.0f, 1.0f,
    0.7f,  0.75f, 0.0f, 0.0f, 0.0f, 1.0f,
    0.75f,  0.85f, 0.0f, 0.0f, 0.0f, 1.0f,
    0.9f,  0.95f, 0.0f, 0.0f, 0.0f, 1.0f
};
glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STREAM_DRAW);
glDrawArrays(GL_TRIANGLES, 0, 6);
*/

/*std::vector<unsigned int> elementData = {
    0, 1,
    1, 2,
    2, 0,
    3, 4,
    4, 5,
    5, 3
};//*/
//glDrawElements(GL_LINES, elementData.size() * sizeof(unsigned int), GL_UNSIGNED_INT, 0);
