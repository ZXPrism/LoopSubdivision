#pragma once

#include <string>

#include <glm/glm.hpp>

class Shader
{
public:
    Shader(const std::string &vertShaderPath, const std::string &fragShaderPath);

    void setMat4(const std::string &name, const glm::mat4 &matrix);
    void setVec3(const std::string &name, const glm::vec3 &vec);
    void setInt1(const std::string &name, const int &val);
    void setFloat1(const std::string &name, const float &val);

    void Activate();

private:
    unsigned _Program;
};
