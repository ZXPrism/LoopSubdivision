#include "Renderer.h"

#include "Model.h"

#include <cstdio>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <cstdarg>

void Renderer::Init()
{
    // GLFW
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    auto vidMode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    auto height = vidMode->height / 2, width = height;
    _Window = glfwCreateWindow(width, height, WINDOW_TITLE, nullptr, nullptr);
    glfwSetWindowPos(_Window, vidMode->width / 2 - width / 2, height / 2); // screen center

    glfwMakeContextCurrent(_Window);
    glfwSwapInterval(1);

    glfwSetKeyCallback(_Window, KeyCallback);

    // GLAD
    gladLoadGL();
    glViewport(0, 0, width, height);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // Buffer Setup
    glGenBuffers(1, &_VBO);
    glGenBuffers(1, &_EBO);
    glGenVertexArrays(1, &_VAO);
    glBindVertexArray(_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, _VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _EBO);
}

void Renderer::Shutdown()
{
    glfwDestroyWindow(_Window);
    glfwTerminate();
}

void Renderer::BeginFrame()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::EndFrame()
{
    glfwSwapBuffers(_Window);
    glfwPollEvents();
}

bool Renderer::Active()
{
    return !glfwWindowShouldClose(_Window);
}

void Renderer::Render(Model &model)
{
    auto &vertices = model._vertices;
    auto &edges = model._edges;
    auto &faces = model._faces;

    std::vector<unsigned> vertIndexes;

    if (model.isUpdated())
    {
        for (int i = 0; i < faces->size(); i++)
        {
            auto &edge = (*edges)[(*faces)[i]._edgeIndex[0]];
            vertIndexes.push_back(edge._vertIndex[0]);
            vertIndexes.push_back(edge._vertIndex[1]);
            if (edge._faceIndex[0] == i)
                vertIndexes.push_back(edge._vertOppositeIndex[0]);
            else
                vertIndexes.push_back(edge._vertOppositeIndex[1]);
        }

        glBindBuffer(GL_ARRAY_BUFFER, _VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, vertIndexes.size() * sizeof(unsigned), &vertIndexes[0],
                     GL_STATIC_DRAW);

        model.ClearUpdFlag();
    }

    glBindVertexArray(_VAO);
    glDrawElements(GL_TRIANGLES, model._faces->size() * 3, GL_UNSIGNED_INT, 0);
}

void Renderer::SetWindowTitle(const char *newTitle, ...)
{
    static char titleBuffer[128];

    va_list args;
    va_start(args, newTitle);
    vsprintf(titleBuffer, newTitle, args);
    va_end(args);

    glfwSetWindowTitle(_Window, titleBuffer);
}
