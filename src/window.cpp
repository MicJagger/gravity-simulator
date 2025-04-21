#include "window.hpp"

#include <cmath>
#include <iostream>
#include <map>
#include <vector>

#include "external/glm/glm/glm.hpp"
#include "external/glm/glm/gtc/matrix_transform.hpp"
#include "external/glm/glm/gtc/type_ptr.hpp"
#include "external/include_glad.hpp"
#include "external/include_sdl.hpp"

#include "definitions.hpp"
#include "math.hpp"

Window::Window() {
    _horRes = 720;
    _vertRes = 720;
    _fov = 90;
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

        layout (location = 0) in vec3 vPos;
        layout (location = 1) in vec3 vColor;
        layout (location = 2) in vec2 vTexCoords;

        uniform mat4 modelMatrix;
        uniform mat4 viewMatrix;
        uniform mat4 projectionMatrix;

        out vec3 vertexColor;
        out vec2 vertexTexCoords;

        void main() {
            gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(vPos.x, vPos.y, vPos.z, 1.0);
            vertexColor = vColor;
            vertexTexCoords = vec2(vTexCoords.x, vTexCoords.y * -1.0f);
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

int Window::SetCameraAngle(float theta, float phi, float psi) {
    _camera._theta = fmod(theta, 360.0f);
    _camera._phi = fmod(phi, 360.0f);
    _camera._psi = fmod(psi, 360.0f);
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

int Window::ChangeCameraAngle(float theta, float phi, float psi) {
    _camera._theta = fmod(_camera._theta + theta, 360.0f);
    _camera._phi = fmod(_camera._phi + phi, 360.0f);
    _camera._psi = fmod(_camera._psi + psi, 360.0f);
    return SUCCESS;
}

void DrawSphere(const Body* body, Camera* camera, std::vector<float>& vertexData) {

}

int Window::DrawFrame(Universe* universe) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    const std::map<long long, Body>* bodies = universe->GetBodies();
    std::vector<float> vertexData = {
        // front
        -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
        // left
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
        // right
        0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f,
        0.5f, 0.5f, -0.5f, 0.5f, 0.5f, 0.5f,
        0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f,
        0.5f, -0.5f, 0.5f, 0.5f, 0.5f, 0.5f,
        // back
        -0.5f, -0.5f, 0.5f, 0.8f, 0.8f, 0.8f,
        0.5f, -0.5f, 0.5f, 0.8f, 0.8f, 0.8f,
        0.5f, 0.5f, 0.5f, 0.8f, 0.8f, 0.8f,
        -0.5f, 0.5f, 0.5f, 0.8f, 0.8f, 0.8f,
        // top
        -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 0.0f, 0.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
        // bottom
        -0.5f, -0.5f, 0.5f, 0.2f, 0.2f, 0.2f,
        0.5f, -0.5f, 0.5f, 0.2f, 0.2f, 0.2f,
        0.5f, -0.5f, -0.5f, 0.2f, 0.2f, 0.2f,
        -0.5f, -0.5f, -0.5f, 0.2f, 0.2f, 0.2f
    };
    std::vector<unsigned int> elementData = {
        // front
        0, 1, 2,
        2, 3, 0,
        // left
        4, 5, 6,
        6, 7, 4,
        // right
        8, 9, 10,
        10, 11, 8,
        // back
        14, 13, 12,
        12, 15, 14,
        // top
        16, 17, 18,
        18, 19, 16,
        // bottom
        20, 21, 22,
        22, 23, 20
    };

    for (auto iter = bodies->begin(); iter != bodies->end(); iter++) {
        //DrawSphere(&iter->second, camera, vertexData);
    }

    auto modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::rotate(modelMatrix, glm::radians((float)_camera._phi), glm::vec3(1.0f, 0.0f, 0.0f)); // w / s
    modelMatrix = glm::rotate(modelMatrix, glm::radians((float)_camera._psi), glm::vec3(0.0f, 1.0f, 0.0f)); // q / e
    modelMatrix = glm::rotate(modelMatrix, glm::radians((float)_camera._theta), glm::vec3(0.0f, 0.0f, 1.0f)); // a / d
    auto modelMatrixLocation = glGetUniformLocation(shaderProgram, "modelMatrix");
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    float nearPlane = 0.1f;
    float farPlane = 1000.0f;
    glm::vec3 camPosition(0.0f, 0.0f, 1.5f);
    glm::vec3 camFront(0.0f, 0.0f, -1.0f);

    glm::mat4 viewMatrix(1.0f);
    viewMatrix = glm::lookAt(camPosition, camPosition + camFront, glm::vec3(0.0f, 1.0f, 0.0f));
    auto viewMatrixLocation = glGetUniformLocation(shaderProgram, "viewMatrix");
    glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));

    glm::mat4 projectionMatrix(1.0f);
    projectionMatrix = glm::perspective(glm::radians(_fov), 1.0f, nearPlane, farPlane);
    auto projectionMatrixLocation = glGetUniformLocation(shaderProgram, "projectionMatrix");
    glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    // copy vertex data into buffer
    glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), vertexData.data(), GL_STREAM_DRAW);
    // copy element data buffer
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, elementData.size() * sizeof(unsigned int), elementData.data(), GL_STREAM_DRAW);

    glBindVertexArray(VAO);

    glDrawElements(GL_TRIANGLES, elementData.size() * sizeof(unsigned int), GL_UNSIGNED_INT, 0);

    SDL_GL_SwapWindow(_window);
    return SUCCESS;
}
