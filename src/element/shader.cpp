#include "shader.h"
#include <fstream>
#include <sstream>
#include <iostream>

Shader::Shader(const std::string &vertexPath, const std::string &fragmentPath)
{
    std::string vertexCode = LoadShaderSource(vertexPath);
    std::string fragmentCode = LoadShaderSource(fragmentPath);

    unsigned int vertex = CompileShader(GL_VERTEX_SHADER, vertexCode);
    unsigned int fragment = CompileShader(GL_FRAGMENT_SHADER, fragmentCode);

    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);

    int success;
    char infoLog[512];
    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(ID, 512, nullptr, infoLog);
        std::cerr << "[Shader::Shader] ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
                  << infoLog << std::endl;
    }

    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

Shader::~Shader()
{
    glDeleteProgram(ID);
}

void Shader::Use() const
{
    glUseProgram(ID);
}

void Shader::SetBool(const std::string &name, bool value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void Shader::SetInt(const std::string &name, int value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::SetFloat(const std::string &name, float value) const
{
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::SetMat4(const std::string &name, const glm::mat4 &value) const
{
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()),
                       1,
                       GL_FALSE,
                       &value[0][0]);
}

template<>
void Shader::SetArray<int>(const std::string& name, const std::vector<int>& values) const
{
    glUniform1iv(glGetUniformLocation(ID, name.c_str()), values.size(), values.data());
}

template<>
void Shader::SetArray<float>(const std::string& name, const std::vector<float>& values) const
{
    glUniform1fv(glGetUniformLocation(ID, name.c_str()), values.size(), values.data());
}

template<>
void Shader::SetArray<glm::vec3>(const std::string& name, const std::vector<glm::vec3>& values) const
{
    glUniform3fv(glGetUniformLocation(ID, name.c_str()), values.size(), (const float*)values.data());
}

template<>
void Shader::SetArray<glm::mat4>(const std::string& name, const std::vector<glm::mat4>& values) const
{
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), values.size(), GL_FALSE, (const float*)values.data());
}

std::string Shader::LoadShaderSource(const std::string &path)
{
    std::ifstream file;
    std::stringstream buffer;

    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        file.open(path);
        buffer << file.rdbuf();
        file.close();
    }
    catch (std::ifstream::failure &e)
    {
        std::cerr << "[Shader::LoadShaderSource] ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: "
                  << path << "\n"
                  << "Exception: " << e.what() << std::endl;
    }
    return buffer.str();
}

unsigned int Shader::CompileShader(unsigned int type, const std::string &source)
{
    unsigned int shader = glCreateShader(type);
    const char *src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    // Check lỗi compile
    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "[Shader::LoadShaderSource] ERROR::SHADER::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }

    return shader;
}
