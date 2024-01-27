#include "Shader.h"

#include <fstream>
#include <sstream>

#include <glad/glad.h>

Shader::Shader(const std::string &VertShaderPath, const std::string &FragShaderPath)
{
    std::string vshaderStr;
    std::string fshaderStr;
    std::ifstream vshaderFin;
    std::ifstream fshaderFin;
    vshaderFin.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fshaderFin.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    vshaderFin.open(VertShaderPath);
    fshaderFin.open(FragShaderPath);
    std::stringstream vshaderSsm, fshaderSsm;
    vshaderSsm << vshaderFin.rdbuf();
    fshaderSsm << fshaderFin.rdbuf();
    vshaderFin.close();
    fshaderFin.close();
    vshaderStr = vshaderSsm.str();
    fshaderStr = fshaderSsm.str();

    const char *vshaderData = vshaderStr.c_str();
    const char *fshaderData = fshaderStr.c_str();

    unsigned vertex, fragment;

    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vshaderData, NULL);
    glCompileShader(vertex);

    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fshaderData, NULL);
    glCompileShader(fragment);

    _Program = glCreateProgram();
    glAttachShader(_Program, vertex);
    glAttachShader(_Program, fragment);
    glLinkProgram(_Program);

    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

void Shader::Activate()
{
    glUseProgram(_Program);
}

void Shader::setMat4(const std::string &name, const glm::mat4 &matrix)
{
    glUniformMatrix4fv(glGetUniformLocation(_Program, name.c_str()), 1, GL_FALSE, &matrix[0][0]);
}

void Shader::setVec3(const std::string &name, const glm::vec3 &vec)
{
    glUniform3f(glGetUniformLocation(_Program, name.c_str()), vec[0], vec[1], vec[2]);
}

void Shader::setInt1(const std::string &name, const int &val)
{
    glUniform1i(glGetUniformLocation(_Program, name.c_str()), val);
}

void Shader::setFloat1(const std::string &name, const float &val)
{
    glUniform1f(glGetUniformLocation(_Program, name.c_str()), val);
}
