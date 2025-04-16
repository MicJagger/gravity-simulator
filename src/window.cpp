#include "window.hpp"

#include <iostream>
#include <map>
#include <vector>

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
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    // tell opengl window size
    //SDL_GetWindowSize(_window, &w, &h);
    glViewport(0, 0, _horRes, _vertRes);
    glClearColor(0.0f, 0.5f, 1.0f, 0.0f);

    // get into the opengl stuff

    // vertex shader for compilation
    const char* vertexShaderSource = "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "void main()\n"
        "{\n"
        "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
        "}\0";//*/

    /*constexpr auto vertexShaderSource = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        layout (location = 1) in vec3 aColor;

        uniform mat4 modelMatrix;

        out vec3 vertexColor;

        void main() {
            gl_Position = modelMatrix * vec4(aPos.x, aPos.y, aPos.z, 1.0);
            vertexColor = aColor;
        }
    )";//*/

    // fragment shader for compilation
    constexpr auto fragmentShaderSource = R"(
        #version 330 core
        out vec4 FragColor;
        void main() {
           FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
        };
    )";//*/
    
    /*constexpr auto fragmentShaderSource = R"(
        #version 330 core
        out vec4 FragColor;

        in vec3 vertexColor;

        void main() {
            FragColor = vec4(vertexColor, 1.0);
        }
    )";//*/

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
    unsigned int shaderProgram;
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

    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);

    // set it to be for vertices
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // bind Vertex Array Object
    glBindVertexArray(VAO);

    // set vertex attributes pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    return SUCCESS;
}

std::vector<unsigned int> Window::PollEvent() {
    std::vector<unsigned int> events;
    while (SDL_PollEvent(&_windowEvent)) {
        events.push_back(_windowEvent.type);
    }
    return events;
}

#include <array>
static constexpr auto triangleVertexIndices = std::array{
    // front
    0, 1, 2, // first triangle
    2, 3, 0, // second triangle

    // top
    4, 5, 6, // first triangle
    6, 7, 4, // second triangle

    // left
    8, 9, 10,  // first triangle
    10, 11, 8, // second triangle

    // right
    14, 13, 12, // 12, 13, 14, // first triangle
    12, 15, 14, // 14, 15, 12, // second triangle

    // back
    18, 17, 16, // 16, 17, 18, // first triangle
    16, 19, 18, // 18, 19, 16, // second triangle

    // bottom
    20, 21, 22, // first triangle
    22, 23, 20  // second triangle
};

int Window::DrawFrame(Universe* universe) {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const std::map<long long, Body>* bodies = universe->GetBodies();

    for (auto iter = bodies->begin(); iter != bodies->end(); iter++) {

    }

    // test vertex data
    float vertices[] = {
        -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.0f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f
    };

    // copy vertex data into buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STREAM_DRAW);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    //glDrawElements(GL_TRIANGLES, 2, GL_UNSIGNED_INT, 0);
    //glDrawElements(GL_TRIANGLES, triangleVertexIndices.size(), GL_UNSIGNED_INT, 0);

    SDL_GL_SwapWindow(_window);
    return SUCCESS;
}
