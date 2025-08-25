#pragma once
#include <string>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

class Shader
{
public:
    unsigned int ID;

    Shader(const std::string& vertexPath, const std::string& fragmentPath);
    ~Shader();

    void Use() const;

    void SetBool(const std::string& name, bool value) const;
    void SetInt(const std::string& name, int value) const;
    void SetFloat(const std::string& name, float value) const;
    void SetMat4(const std::string& name, const glm::mat4& value) const;
    
    template<typename T>
    void SetArray(const std::string& name, const std::vector<T>& values) const;

private:
    std::string LoadShaderSource(const std::string& path);
    unsigned int CompileShader(unsigned int type, const std::string& source);
};
