#pragma once
#include <string>
#include <glad/glad.h>

class Texture
{
public:
    unsigned int ID;
    Texture(const std::string &path, bool flipVertically = true);
    ~Texture();

    void Bind(unsigned int unit = 0) const;
    void Unbind() const;

    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }

private:
    int m_width, m_height, m_channels;
};
