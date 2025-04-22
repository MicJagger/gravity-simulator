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
#include "body.hpp"
#include "math.hpp"

constexpr const char* vertexShaderSource = R"(
    #version 330 core

    layout (location = 0) in vec3 vPos;
    layout (location = 1) in vec3 vColor;
    layout (location = 2) in vec2 vTexCoords;

    uniform mat4 viewMatrix;
    uniform mat4 projectionMatrix;

    out vec3 vertexColor;
    out vec2 vertexTexCoords;

    void main() {
        gl_Position = projectionMatrix * viewMatrix * vec4(vPos.x, vPos.y, vPos.z, 1.0);
        vertexColor = vColor;
        vertexTexCoords = vec2(vTexCoords.x, vTexCoords.y * -1.0f);
    }
)";

constexpr const char* fragmentShaderSource = R"(
    #version 330 core

    in vec3 vertexColor;

    out vec4 FragColor;

    void main() {
        FragColor = vec4(vertexColor.r, vertexColor.g, vertexColor.b, 1.0f);
    }
)";

inline glm::vec3 AngleToVector(const float& theta, const float& phi, const float& psi) {
    float r_theta = glm::radians(theta);
    float r_phi = glm::radians(phi);
    float x = cos(r_theta) * sin(r_phi);
    float y = sin(r_theta) * sin(r_phi);
    float z = cos(r_phi);
    return glm::vec3(x, y, z);
}


// window functions

Window::Window() {
    _horRes = 1280;
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

const Camera& Window::GetCamera() {
    return _camera;
}

std::vector<SDL_Event> Window::PollEvent() {
    std::vector<SDL_Event> events;
    while (SDL_PollEvent(&_windowEvent)) {
        events.push_back(_windowEvent);
    }
    return events;
}

int Window::SetCameraPosition(const double& x, const double& y, const double& z) {
    _camera._x = x;
    _camera._y = y;
    _camera._z = z;
    return SUCCESS;
}

int Window::SetCameraAngle(const float& theta, const float& phi, const float& psi) {
    _camera._theta = fmod(theta, 360.0f);
    _camera._phi = fmod(phi, 360.0f);
    _camera._psi = fmod(psi, 360.0f);
    return SUCCESS;
}

int Window::ChangeCameraPosition(const double& x, const double& y, const double& z) {
    _camera._x += x;
    _camera._y += y;
    _camera._z += z;
    return SUCCESS;
}

int Window::ChangeCameraAngle(const float& theta, const float& phi, const float& psi) {
    _camera._theta = fmod(_camera._theta + theta, 360.0f);
    float newPhi = _camera._phi + phi;
    if (newPhi > 180) {
        _camera._phi = 179.999f;
    }
    else if (newPhi < 0) {
        _camera._phi = 0.001f;
    }
    else {
        _camera._phi = newPhi;
    }
    _camera._psi = fmod(_camera._psi + psi, 360.0f);
    return SUCCESS;
}

int Window::MoveCamera(const double& forward, const double& right, const double& up) {
    float theta = glm::radians(_camera._theta);
    float x = cos(theta);
    float y = sin(theta);
    _camera._x += (forward * x) + (right * y);
    _camera._y += (forward * y) - (right * x);
    _camera._z += up;
    return SUCCESS;
}

void DrawSphere(const Body& body, const Camera& camera, std::vector<float>& vertexData, std::vector<unsigned int>& elementData) {

}

int Window::DrawFrame(const Universe& universe) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    const std::map<long long, Body> bodies = universe.GetBodies();
    std::vector<float> vertexData = {
        // forward
        -2.0f, 5.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        2.0f, 5.0f, 0.0f, 1.0f, 1.0f, 0.0f,
        0.0f, 5.0f, 5.0f, 1.0f, 1.0f, 1.0f,

        // right
        5.0f, -2.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        5.0f, 2.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        5.0f, 0.0f, -5.0f, 1.0f, 1.0f, 1.0f,

        // back
        -2.0f, -5.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        2.0f, -5.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        0.0f, -5.0f, 5.0f, 0.0f, 0.0f, 1.0f,

        // left
        -5.0f, -2.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        -5.0f, 2.0f, 0.0f, 1.0f, 1.0f, 0.0f,
        -5.0f, 0.0f, -5.0f, 1.0f, 0.0f, 0.0f
    };
    std::vector<unsigned int> elementData = {
        // top
        0, 1, 2,

        // right
        3, 4, 5,

        // back
        6, 7, 8,

        // left
        9, 10, 11
    };

    for (const auto& [id, data]: bodies) {
        DrawSphere(data, _camera, vertexData, elementData);
    }

    float nearPlane = 0.1f;
    float farPlane = 1000.0f;
    _mtx.lock();
    glm::vec3 camPosition(_camera._x, _camera._y, _camera._z);
    glm::vec3 camFront(AngleToVector(_camera._theta, _camera._phi, _camera._psi));
    _mtx.unlock();

    glm::mat4 viewMatrix(1.0f);
    viewMatrix = glm::lookAt(camPosition, camPosition + camFront, glm::vec3(0.0f, 0.0f, 1.0f));
    auto viewMatrixLocation = glGetUniformLocation(shaderProgram, "viewMatrix");
    glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));

    glm::mat4 projectionMatrix(1.0f);
    projectionMatrix = glm::perspective(glm::radians(_fov), (float)_horRes / (float)_vertRes, nearPlane, farPlane);
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
