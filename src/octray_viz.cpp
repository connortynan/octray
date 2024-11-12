#include "octray_node.hpp"

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

float radius = 3.0f;    // Distance from the origin
float pitch = 0.0f;     // Vertical angle
float yaw = -90.0f;     // Horizontal angle (start facing -Z direction)
bool firstMouse = true; // To prevent jumping the cursor at the start

float lastX = 400, lastY = 300; // Mouse starting position (center of window)
float mouseSensitivity = 1.f;   // Mouse sensitivity

GLuint createShaderProgramFromFile(const char *vertexShaderPath, const char *fragmentShaderPath);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
glm::vec3 getCameraPosition();

int main()
{
    if (!glfwInit())
    {
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(800, 600, "Octree Visualization", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    glfwSetCursorPosCallback(window, mouse_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        return -1;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    float s = 0.5f;

    float face_tris[] = {
        -s, -s, s, s, -s, s, s, s, s,
        -s, -s, s, s, s, s, -s, s, s,
        -s, -s, -s, s, -s, -s, s, s, -s,
        -s, -s, -s, s, s, -s, -s, s, -s,
        -s, -s, -s, -s, -s, s, -s, s, s,
        -s, -s, -s, -s, s, s, -s, s, -s,
        s, -s, -s, s, -s, s, s, s, s,
        s, -s, -s, s, s, s, s, s, -s,
        -s, s, -s, s, s, -s, s, s, s,
        -s, s, -s, s, s, s, -s, s, s,
        -s, -s, -s, s, -s, -s, s, -s, s,
        -s, -s, -s, s, -s, s, -s, -s, s};
    float line_vs[] = {
        s, s, s, -s, s, s,
        s, s, s, s, -s, s,
        s, s, s, s, s, -s,
        s, s, -s, -s, s, -s,
        s, s, -s, s, -s, -s,
        s, -s, s, -s, -s, s,
        s, -s, s, s, -s, -s,
        s, -s, -s, -s, -s, -s,
        -s, s, s, -s, -s, s,
        -s, s, s, -s, s, -s,
        -s, s, -s, -s, -s, -s,
        -s, -s, s, -s, -s, -s};

    std::vector<CubeInstance> solidInstanceTransforms;
    std::vector<CubeInstance> outlineInstanceTransforms;

    int maxDepth = 5;
    OctrayNode octray({0.f, 0.f, 0.f}, 1.f, maxDepth);

    Vec3f start{
        (float)(rand() % 20000) / 20000.f - 0.5f,
        (float)(rand() % 20000) / 20000.f - 0.5f,
        (float)(rand() % 20000) / 20000.f - 0.5f,
    };
    Vec3f end{
        (float)(rand() % 20000) / 20000.f - 0.5f,
        (float)(rand() % 20000) / 20000.f - 0.5f,
        (float)(rand() % 20000) / 20000.f - 0.5f,
    };

    std::cout << "start:\n x: " << start.x << "\n y: " << start.y << "\n z: " << start.z << "\nend:\n x: " << end.x << "\n y: " << end.y << "\n z: " << end.z << std::endl;

    octray.accumulate_ray(start, end, solidInstanceTransforms, outlineInstanceTransforms);

    std::cout << "solid: " << solidInstanceTransforms.size() << ", outline: " << outlineInstanceTransforms.size() << std::endl;

    GLuint VBO_solid, VBO_outline,
        VAO_solid, VAO_outline,
        instanceVBO_solid, instanceVBO_outline;

    glGenBuffers(1, &VBO_solid);
    glGenBuffers(1, &VBO_outline);
    glGenVertexArrays(1, &VAO_solid);
    glGenVertexArrays(1, &VAO_outline);
    glGenBuffers(1, &instanceVBO_solid);
    glGenBuffers(1, &instanceVBO_outline);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_solid);
    glBufferData(GL_ARRAY_BUFFER, sizeof(face_tris), face_tris, GL_STATIC_DRAW);
    glBindVertexArray(VAO_solid);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO_solid);
    glBufferData(GL_ARRAY_BUFFER, solidInstanceTransforms.size() * sizeof(CubeInstance), solidInstanceTransforms.data(), GL_STATIC_DRAW);
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

    glBindBuffer(GL_ARRAY_BUFFER, VBO_outline);
    glBufferData(GL_ARRAY_BUFFER, sizeof(line_vs), line_vs, GL_STATIC_DRAW);
    glBindVertexArray(VAO_outline);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO_outline);
    glBufferData(GL_ARRAY_BUFFER, outlineInstanceTransforms.size() * sizeof(CubeInstance), outlineInstanceTransforms.data(), GL_STATIC_DRAW);
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

    GLuint shaderProgram = createShaderProgramFromFile("../shaders/cube_vertex_shader.glsl", "../shaders/cube_fragment_shader.glsl");

    if (shaderProgram == -1)
        return -1;

    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);

        glm::vec3 cameraPos = getCameraPosition();
        glm::vec3 cameraUp(0.0f, 1.0f, 0.0f);
        glm::mat4 view = glm::lookAt(cameraPos, glm::vec3(0.0f, 0.0f, 0.0f), cameraUp);

        GLuint modelLoc = glGetUniformLocation(shaderProgram, "model");
        GLuint viewLoc = glGetUniformLocation(shaderProgram, "view");
        GLuint projectionLoc = glGetUniformLocation(shaderProgram, "projection");

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

        // [Render Octree and UI Here]
        if (solidInstanceTransforms.size() > 0)
        {
            glBindVertexArray(VAO_solid);
            glDrawArraysInstanced(GL_TRIANGLES, 0, 36, solidInstanceTransforms.size());
        }
        if (outlineInstanceTransforms.size() > 0)
        {
            glBindVertexArray(VAO_outline);
            glDrawArraysInstanced(GL_LINES, 0, 24, outlineInstanceTransforms.size());
        }
        // [------------------------]

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }
    glDeleteBuffers(1, &VBO_solid);
    glDeleteBuffers(1, &VBO_outline);
    glDeleteBuffers(1, &instanceVBO_solid);
    glDeleteBuffers(1, &instanceVBO_outline);
    glDeleteVertexArrays(1, &VAO_solid);
    glDeleteVertexArrays(1, &VAO_outline);

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

glm::vec3 getCameraPosition()
{
    float x = radius * cos(glm::radians(pitch)) * cos(glm::radians(yaw));
    float y = radius * sin(glm::radians(pitch));
    float z = radius * cos(glm::radians(pitch)) * sin(glm::radians(yaw));

    return glm::vec3(x, y, z);
}

void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xOffset = xpos - lastX;
    float yOffset = ypos - lastY;
    lastX = xpos;
    lastY = ypos;

    xOffset *= mouseSensitivity;
    yOffset *= mouseSensitivity;

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT))
    {
        yaw += xOffset;
        pitch += yOffset;

        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;
    }
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
