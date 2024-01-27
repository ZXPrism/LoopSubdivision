#pragma once

#define WINDOW_TITLE "Loop Subdivision Demo"

struct GLFWwindow;
class Model;

class Renderer
{
public:
    void Init();
    void Shutdown();

    void BeginFrame();
    void EndFrame();

    bool Active();

    void Render(Model &model);

    void SetWindowTitle(const char *newTitle, ...);

private:
    GLFWwindow *_Window;
    unsigned _VAO, _VBO, _EBO;
};

void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
