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

// POS.X, POS.Y, POS.Z, COLOR.R, COLOR.G, COLOR.B, TEX.X, TEX.Y, NORMAL.X, NORMAL.Y, NORMAL.Z
constexpr int vertexFloatWidth = 12;

constexpr const char* vertexShaderSource = R"(
    #version 330 core

    layout (location = 0) in vec3 vPos;
    layout (location = 1) in vec3 vColor;
    layout (location = 2) in vec2 vTexCoords;
    layout (location = 3) in float vMinBrightness;
    layout (location = 4) in vec3 vNormals;

    uniform mat4 projectionMatrix;
    uniform mat4 viewMatrix;

    out vec3 vertexPos;
    out vec3 vertexColor;
    out vec2 vertexTexCoords;
    out float vertexMinBrightness;
    out vec3 vertexNormal;

    void main() {
        gl_Position = projectionMatrix * viewMatrix * vec4(vPos.x, vPos.y, vPos.z, 1.0f);
        vertexPos = vPos;
        vertexColor = vColor;
        //vertexTexCoords = vec2(vTexCoords.x, vTexCoords.y * -1.0f);
        vertexMinBrightness = vMinBrightness;
        vertexNormal = vNormals;
    }
)";

constexpr const char* fragmentShaderSource = R"(
    #version 330 core

    vec3 lightPos = vec3(0.0f, 0.0f, 0.0f);

    in vec3 vertexPos;
    in vec3 vertexColor;
    in float vertexMinBrightness;
    in vec3 vertexNormal;

    out vec4 FragColor;

    void main() {
        vec3 vNorm = normalize(vertexNormal);
        vec3 lightDirection = normalize(lightPos - vertexPos);
        float diff = max(dot(vNorm, lightDirection), 0.0f);
        float lightIntensity = min(diff + vertexMinBrightness, 1.0f);
        FragColor = vec4(vertexColor * lightIntensity, 1.0f);
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
    _horRes = 1600;
    _vertRes = 900;
    _fov = 75;
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

    // tell opengl window size
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
    _shaderProgram = glCreateProgram();

    // creates and links
    glAttachShader(_shaderProgram, vertexShader);
    glAttachShader(_shaderProgram, fragmentShader);
    glLinkProgram(_shaderProgram);
    // check for failure (even more)
    glGetProgramiv(_shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(_shaderProgram, 512, NULL, infoLog);
        std::cout << "shaderProgram compilation failed\n" << infoLog << std::endl;
        return FAIL;
    }

    // use this program
    glUseProgram(_shaderProgram);

    // delete old objects
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // setup other stuffs

    glGenVertexArrays(1, &_VAO);
    glGenBuffers(1, &_VBO);
    glGenBuffers(1, &_EBO);

    glBindVertexArray(_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, _VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _EBO);

    // set vertex attributes pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertexFloatWidth * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, vertexFloatWidth * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // texture
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, vertexFloatWidth * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // minBrightness
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, vertexFloatWidth * sizeof(float), (void*)(8 * sizeof(float)));
    glEnableVertexAttribArray(3);

    // normals
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, vertexFloatWidth * sizeof(float), (void*)(9 * sizeof(float)));
    glEnableVertexAttribArray(4);

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
    _camera.x = x;
    _camera.y = y;
    _camera.z = z;
    return SUCCESS;
}

int Window::SetCameraAngle(const float& theta, const float& phi, const float& psi) {
    _camera.theta = fmod(theta, 360.0f);
    _camera.phi = fmod(phi, 360.0f);
    _camera.psi = fmod(psi, 360.0f);
    return SUCCESS;
}

int Window::ChangeCameraPosition(const double& x, const double& y, const double& z) {
    _camera.x += x;
    _camera.y += y;
    _camera.z += z;
    return SUCCESS;
}

int Window::ChangeCameraAngle(const float& theta, const float& phi, const float& psi) {
    _camera.theta = fmod(_camera.theta + theta, 360.0f);
    float newPhi = _camera.phi + phi;
    if (newPhi > 180) {
        _camera.phi = 179.999f;
    }
    else if (newPhi < 0) {
        _camera.phi = 0.001f;
    }
    else {
        _camera.phi = newPhi;
    }
    _camera.psi = fmod(_camera.psi + psi, 360.0f);
    return SUCCESS;
}

int Window::MoveCamera(const double& forward, const double& right, const double& up) {
    float theta = glm::radians(_camera.theta);
    float x = cos(theta);
    float y = sin(theta);
    _camera.x += (forward * x) + (right * y);
    _camera.y += (forward * y) - (right * x);
    _camera.z += up;
    return SUCCESS;
}

inline void AddValues(std::vector<float>& vertexData, const float& f0, const float& f1, const float& f2) {
    vertexData.push_back(f0);
    vertexData.push_back(f1);
    vertexData.push_back(f2);
}

inline void AddValues(std::vector<unsigned int>& elementData, const unsigned int& f0, const unsigned int& f1, const unsigned int& f2) {
    elementData.push_back(f0);
    elementData.push_back(f1);
    elementData.push_back(f2);
}

void DrawSphere(const Body& body, const Camera& camera, std::vector<float>& vertexData, std::vector<unsigned int>& elementData, const float& red, const float& green, const float& blue) {
    // tracks initial vertexData size to offset indices
    int elementStart = vertexData.size() / vertexFloatWidth;
    int elementIndexStart = elementData.size();

    const int stackCount = 45;
    const int sectorCount = 45;
    const float stackAngle = 180.0 / stackCount;
    const float sectorAngle = 360.0 / sectorCount;

    const double x = body.x, y = body.y, z = body.z;
    // delta
    double dx = 0, dy = 0, dz = 0;
    // delta normalized
    double dxn = 0, dyn = 0, dzn = 0;
    const double radius = body.radius;

    // vertexData
    // top
    AddValues(vertexData, x, y, z + radius); // position
    AddValues(vertexData, 1.0 - red, 1.0f - green, 1.0f - blue); // color (inverted)
    vertexData.push_back(0.0f); // tex.x
    vertexData.push_back(0.0f); // tex.y
    vertexData.push_back((float)body.luminosity); // minBrightness
    AddValues(vertexData, 0.0f, 0.0f, 1.0f); // normal
    // all other points
    for (int i = 1; i < stackCount; i++) {
        dzn = cos(glm::radians(i * stackAngle));
        dz = radius * dzn;
        for (int j = 0; j < sectorCount; j++) {
            dxn = sin(glm::radians(i * stackAngle)) * cos(glm::radians(j * sectorAngle));
            dyn = sin(glm::radians(i * stackAngle)) * sin(glm::radians(j * sectorAngle));
            dx = radius * dxn;
            dy = radius * dyn;
            AddValues(vertexData, x + dx, y + dy, z + dz); // position
            AddValues(vertexData, red, green, blue); // color
            vertexData.push_back(0.0f); // tex.x
            vertexData.push_back(0.0f); // tex.y
            vertexData.push_back((float)body.luminosity); // minBrightness
            AddValues(vertexData, dxn, dyn, dzn); // normals
        }
    }
    // bottom
    AddValues(vertexData, x, y, z - radius); // position
    AddValues(vertexData, 1.0 - red, 1.0f - green, 1.0f - blue); // color (inverted)
    vertexData.push_back(0.0f); // tex.x
    vertexData.push_back(0.0f); // tex.y
    vertexData.push_back((float)body.luminosity); // minBrightness
    AddValues(vertexData, 0.0f, 0.0f, -1.0f); // normal

    // elementData
    // top triangles
    for (int j = 1; j <= sectorCount; j++) {
        AddValues(elementData, 0, j, (j % sectorCount) + 1);
    }
    // middle squares
    for (int i = 0; i < (stackCount - 1); i++) {
        if (i > 0) {
            int rowIndexStart = sectorCount * i;
            for (int j = 1; j <= sectorCount; j++) {
                int vertex1 = j + rowIndexStart;
                int vertex2 = (j % sectorCount) + 1 + rowIndexStart;
                int vertex3 = (j % sectorCount) + 1 + rowIndexStart - sectorCount;
                AddValues(elementData, vertex1, vertex2, vertex3);
            }
        }
        if (i < (stackCount - 2)) {
            int rowIndexStart = sectorCount * i;
            for (int j = 1; j <= sectorCount; j++) {
                int vertex1 = j + rowIndexStart;
                int vertex2 = (j % sectorCount) + 1 + rowIndexStart;
                int vertex3 = j + sectorCount + rowIndexStart;
                AddValues(elementData, vertex1, vertex2, vertex3);
            }
        }
    }
    // bottom triangles
    int start = (stackCount - 2) * sectorCount + 1;
    int end = (stackCount - 1) * sectorCount;
    for (int j = start; j <= end; j++) {
        AddValues(elementData, end + 1, j, start + (j % sectorCount));
    }
    // offset indices by element start value
    // this could maybe be factored into all the above calculations, but this has benefits too
    for (int i = elementIndexStart; i < elementData.size(); i++) {
        elementData[i] += elementStart;
    }
}

int Window::DrawFrame(const Universe& universe) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    const std::map<long long, Body> bodies = universe.GetBodies();
    std::vector<float> vertexData;
    std::vector<unsigned int> elementData;

    _mtx.lock();
    for (const auto& [id, data]: bodies) {
        DrawSphere(data, _camera, vertexData, elementData, 1.0f, 1.0f, 1.0f);
    }

    glm::vec3 camPosition(_camera.x, _camera.y, _camera.z);
    glm::vec3 camFront(AngleToVector(_camera.theta, _camera.phi, _camera.psi));
    _mtx.unlock();

    float nearPlane = 0.1f;
    // set this back to <= 1000 once properly scaling distances
    // it is set this high to test the solar system
    float farPlane = 10000.0f;

    glm::mat4 viewMatrix(1.0f);
    viewMatrix = glm::lookAt(camPosition, camPosition + camFront, glm::vec3(0.0f, 0.0f, 1.0f));
    auto viewMatrixLocation = glGetUniformLocation(_shaderProgram, "viewMatrix");
    glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));

    glm::mat4 projectionMatrix(1.0f);
    projectionMatrix = glm::perspective(glm::radians(_fov), (float)_horRes / (float)_vertRes, nearPlane, farPlane);
    auto projectionMatrixLocation = glGetUniformLocation(_shaderProgram, "projectionMatrix");
    glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    // copy vertex data into buffer
    glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), vertexData.data(), GL_STREAM_DRAW);
    // copy element data buffer
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, elementData.size() * sizeof(unsigned int), elementData.data(), GL_STREAM_DRAW);

    glDrawElements(GL_TRIANGLES, elementData.size() * sizeof(unsigned int), GL_UNSIGNED_INT, 0);

    SDL_GL_SwapWindow(_window);
    return SUCCESS;
}
