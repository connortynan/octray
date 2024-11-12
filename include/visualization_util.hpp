#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

constexpr float cube_tri_verts[] = {
    -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f, 0.5f, 0.5f, 0.5f,
    -0.5f, -0.5f, 0.5f, 0.5f, 0.5f, 0.5f, -0.5f, 0.5f, 0.5f,
    -0.5f, -0.5f, -0.5f, 0.5f, -0.5f, -0.5f, 0.5f, 0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, -0.5f, 0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f, -0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f,
    -0.5f, -0.5f, -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f, -0.5f,
    0.5f, -0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f, 0.5f, 0.5f,
    0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, -0.5f,
    -0.5f, 0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f, 0.5f, 0.5f,
    -0.5f, 0.5f, -0.5f, 0.5f, 0.5f, 0.5f, -0.5f, 0.5f, 0.5f,
    -0.5f, -0.5f, -0.5f, 0.5f, -0.5f, -0.5f, 0.5f, -0.5f, 0.5f,
    -0.5f, -0.5f, -0.5f, 0.5f, -0.5f, 0.5f, -0.5f, -0.5f, 0.5f};

constexpr float cube_line_verts[] = {
    0.5f, 0.5f, 0.5f, -0.5f, 0.5f, 0.5f,
    0.5f, 0.5f, 0.5f, 0.5f, -0.5f, 0.5f,
    0.5f, 0.5f, 0.5f, 0.5f, 0.5f, -0.5f,
    0.5f, 0.5f, -0.5f, -0.5f, 0.5f, -0.5f,
    0.5f, 0.5f, -0.5f, 0.5f, -0.5f, -0.5f,
    0.5f, -0.5f, 0.5f, -0.5f, -0.5f, 0.5f,
    0.5f, -0.5f, 0.5f, 0.5f, -0.5f, -0.5f,
    0.5f, -0.5f, -0.5f, -0.5f, -0.5f, -0.5f,
    -0.5f, 0.5f, 0.5f, -0.5f, -0.5f, 0.5f,
    -0.5f, 0.5f, 0.5f, -0.5f, 0.5f, -0.5f,
    -0.5f, 0.5f, -0.5f, -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f, 0.5f, -0.5f, -0.5f, -0.5f};

void generate_buffers(GLuint *VBO_solid, GLuint *VBO_outline,
                      GLuint *VAO_solid, GLuint *VAO_outline,
                      GLuint *instanceVBO_solid, GLuint *instanceVBO_outline,
                      std::vector<CubeInstance> &solidInstances, std::vector<CubeInstance> &outlineInstances)
{
    glGenBuffers(1, VBO_solid);
    glGenBuffers(1, VBO_outline);
    glGenVertexArrays(1, VAO_solid);
    glGenVertexArrays(1, VAO_outline);
    glGenBuffers(1, instanceVBO_solid);
    glGenBuffers(1, instanceVBO_outline);

    glBindBuffer(GL_ARRAY_BUFFER, *VBO_solid);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_tri_verts), cube_tri_verts, GL_STATIC_DRAW);
    glBindVertexArray(*VAO_solid);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, *instanceVBO_solid);
    glBufferData(GL_ARRAY_BUFFER, solidInstances.size() * sizeof(CubeInstance), solidInstances.data(), GL_STATIC_DRAW);
    for (int i = 0; i < 4; ++i)
    {
        glVertexAttribPointer(
            i + 1, 4, GL_FLOAT, GL_FALSE, sizeof(CubeInstance),
            (void *)(offsetof(CubeInstance, model) + i * sizeof(glm::vec4)));
        glEnableVertexAttribArray(i + 1);
        glVertexAttribDivisor(i + 1, 1);
    }
    glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, sizeof(CubeInstance),
                          (void *)(offsetof(CubeInstance, color)));
    glEnableVertexAttribArray(5);
    glVertexAttribDivisor(5, 1);
    glBindVertexArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, *VBO_outline);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_line_verts), cube_line_verts, GL_STATIC_DRAW);
    glBindVertexArray(*VAO_outline);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, *instanceVBO_outline);
    glBufferData(GL_ARRAY_BUFFER, outlineInstances.size() * sizeof(CubeInstance), outlineInstances.data(), GL_STATIC_DRAW);
    for (int i = 0; i < 4; ++i)
    {
        glVertexAttribPointer(
            i + 1, 4, GL_FLOAT, GL_FALSE, sizeof(CubeInstance),
            (void *)(offsetof(CubeInstance, model) + i * sizeof(glm::vec4)));
        glEnableVertexAttribArray(i + 1);
        glVertexAttribDivisor(i + 1, 1);
    }
    glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, sizeof(CubeInstance),
                          (void *)(offsetof(CubeInstance, color)));
    glEnableVertexAttribArray(5);
    glVertexAttribDivisor(5, 1);
    glBindVertexArray(0);
}

GLFWwindow *window_init(int width, int height, const char *title)
{
    if (!glfwInit())
    {
        return nullptr;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        return nullptr;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    return window;
}

std::string readFile(const char *filePath)
{
    std::ifstream file(filePath);
    if (!file)
    {
        std::cerr << "Error opening file: " << filePath << std::endl;
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

GLuint loadShader(const char *shaderSource, GLenum shaderType)
{
    GLuint shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &shaderSource, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        GLint logLength;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
        char *log = new char[logLength];
        glGetShaderInfoLog(shader, logLength, nullptr, log);
        std::cerr << "Shader compilation failed:\n"
                  << log << std::endl;
        delete[] log;
        return -1;
    }

    return shader;
}

GLuint createShaderProgramFromSource(const char *vertexShaderSource, const char *fragmentShaderSource)
{
    GLuint vertexShader = loadShader(vertexShaderSource, GL_VERTEX_SHADER);
    GLuint fragmentShader = loadShader(fragmentShaderSource, GL_FRAGMENT_SHADER);

    if (vertexShader == -1 || fragmentShader == -1)
        return -1;

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    GLint success;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        GLint logLength;
        glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &logLength);
        char *log = new char[logLength];
        glGetProgramInfoLog(shaderProgram, logLength, nullptr, log);
        std::cerr << "Shader program linking failed:\n"
                  << log << std::endl;
        delete[] log;
        return -1;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

GLuint createShaderProgramFromFile(const char *vertexShaderPath, const char *fragmentShaderPath)
{
    std::string vertSource = readFile(vertexShaderPath);
    std::string fragSource = readFile(fragmentShaderPath);
    if (vertSource.empty() || fragSource.empty())
    {
        std::cerr << "Failed to load shader source code!" << std::endl;
        return 0;
    }

    return createShaderProgramFromSource(vertSource.c_str(), fragSource.c_str());
}
