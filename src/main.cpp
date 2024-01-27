#include "GLFW/glfw3.h"
#include "Model.h"
#include "Renderer.h"
#include "Shader.h"

#include <iostream>

#include <glm/gtc/matrix_transform.hpp>

bool subdivFlag = false;
int subdivNum;

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cout << "Usage: lsdemo <obj file path>" << std::endl;
        return 0;
    }

    Renderer renderer;
    renderer.Init();

    Model m(argv[1]);

    Shader shader("shader/shader.vs", "shader/shader.fs");

    glm::mat4 model(1.0f);
    glm::mat4 projection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, -10.0f, 10.0f);
    glm::mat4 view = glm::lookAt(glm::vec3({1, 1, 1}), {0, 0, 0}, {0, 1, 0});

    model = glm::scale(model, glm::vec3(3.5f));

    while (renderer.Active())
    {
        renderer.BeginFrame();

        if (subdivFlag)
        {
            m.LoopSubdivision();
            ++subdivNum;
            renderer.SetWindowTitle(WINDOW_TITLE " [iteration: %d]", subdivNum);
            subdivFlag = false;
        }

        shader.Activate();
        shader.setMat4("mvp", projection * view * model);
        renderer.Render(m);

        renderer.EndFrame();
    }

    renderer.Shutdown();
    return 0;
}

void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS)
    {
        if (key == GLFW_KEY_ESCAPE)
            glfwSetWindowShouldClose(window, true);
        else if (key == 'x' || key == 'X')
            subdivFlag = true;
    }
}
