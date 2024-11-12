#include "octray_node.hpp"
#include "visualization_util.hpp"

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
#include <chrono>

#define WIDTH 1280
#define HEIGHT 720

float radius = 3.0f;
float pitch = 0.0f;
float yaw = -90.0f;
bool firstMouse = true;

float lastX = WIDTH / 2.f, lastY = HEIGHT / 2.f;
float mouseSensitivity = 0.5f;

glm::vec3 focal_point{0.f, 0.f, 0.f};

void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xpos, double ypos);
glm::vec3 getCameraPosition();

int main()
{
    GLFWwindow *window = window_init(WIDTH, HEIGHT, "Octray");
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    std::vector<CubeInstance> solidInstances;
    std::vector<CubeInstance> outlineInstances;

    int maxDepth = 12;
    Octray octray({0.f, 0.f, 0.f}, 1.f, maxDepth);

    srand(time(NULL));

    Vec3f ray_start{
        (float)(rand()) / RAND_MAX - 0.5f,
        (float)(rand()) / RAND_MAX - 0.5f,
        (float)(rand()) / RAND_MAX - 0.5f,
    };
    Vec3f ray_end{
        (float)(rand()) / RAND_MAX - 0.5f,
        (float)(rand()) / RAND_MAX - 0.5f,
        (float)(rand()) / RAND_MAX - 0.5f,
    };

    std::cout << "start:\n x: " << ray_start.x << "\n y: " << ray_start.y << "\n z: " << ray_start.z
              << "\nend:\n x: " << ray_end.x << "\n y: " << ray_end.y << "\n z: " << ray_end.z << std::endl;

    {
        using namespace std::chrono;

        time_point start_time = high_resolution_clock::now();
        octray.accumulate_ray(ray_start, ray_end, solidInstances, outlineInstances);
        time_point end_time = high_resolution_clock::now();

        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
        std::cout << "one ray at " << maxDepth << " depth in " << (float)(duration.count()) / 1000.f << "ms" << std::endl;
    }
    std::cout << "solid: " << solidInstances.size() << ", outline: " << outlineInstances.size() << std::endl;

    GLuint VBO_solid, VBO_outline,
        VAO_solid, VAO_outline,
        instanceVBO_solid, instanceVBO_outline;

    generate_buffers(&VBO_solid, &VBO_outline, &VAO_solid, &VAO_outline, &instanceVBO_solid, &instanceVBO_outline, solidInstances, outlineInstances);

    GLuint shaderProgram = createShaderProgramFromFile("../shaders/cube_vertex_shader.glsl", "../shaders/cube_fragment_shader.glsl");
    if (shaderProgram == -1)
        return -1;

    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        if (glfwGetKey(window, GLFW_KEY_R))
        {
            focal_point = {0.f, 0.f, 0.f};
            radius = 3.0f;
            pitch = 0.0f;
            yaw = -90.0f;
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);

        glm::vec3 cameraPos = getCameraPosition();
        glm::vec3 cameraUp(0.0f, 1.0f, 0.0f);
        glm::mat4 view = glm::lookAt(cameraPos, focal_point, cameraUp);

        GLuint modelLoc = glGetUniformLocation(shaderProgram, "model");
        GLuint viewLoc = glGetUniformLocation(shaderProgram, "view");
        GLuint projectionLoc = glGetUniformLocation(shaderProgram, "projection");

        GLuint cameraPosLoc = glGetUniformLocation(shaderProgram, "cameraPos");
        glUniform3fv(cameraPosLoc, 1, glm::value_ptr(cameraPos));

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

        // [Render Octree and UI Here]
        if (solidInstances.size() > 0)
        {
            glBindVertexArray(VAO_solid);
            glDrawArraysInstanced(GL_TRIANGLES, 0, 36, solidInstances.size());
        }
        if (outlineInstances.size() > 0)
        {
            glLineWidth(0.5f);
            glBindVertexArray(VAO_outline);
            glDrawArraysInstanced(GL_LINES, 0, 24, outlineInstances.size());
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

    return focal_point + glm::vec3(x, y, z);
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
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS)
        {
            glm::vec3 cameraDirection = glm::normalize(focal_point - getCameraPosition());
            glm::vec3 cameraRight = glm::normalize(glm::cross(cameraDirection, glm::vec3(0.0f, 1.0f, 0.0f)));
            glm::vec3 cameraUp = glm::cross(cameraRight, cameraDirection);

            focal_point += cameraRight * (-xOffset * 0.001f * radius);
            focal_point += cameraUp * (yOffset * 0.001f * radius);
        }
        else
        {
            yaw += xOffset;
            pitch += yOffset;

            if (pitch > 89.0f)
                pitch = 89.0f;
            if (pitch < -89.0f)
                pitch = -89.0f;
        }
    }
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    radius -= static_cast<float>(yoffset) * ((glfwGetKey(window, GLFW_KEY_LEFT_SHIFT)) ? 0.01f : 0.5f);

    radius = std::clamp(radius, 0.1f, 3.0f);
}
