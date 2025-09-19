#pragma once
#include <string>
#include <glad/glad.h>

class Texture
{
public:
    unsigned int ID;
    Texture(const std::string &i, const std::string &path, bool flipVertically = true);
    ~Texture();

    void SetID(const std::string &id) { m_id = id; }
    std::string GetID() const { return m_id; }

    void Bind(unsigned int unit = 0) const;
    void Unbind() const;

    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }

private:
    std::string m_id;
    int m_width, m_height, m_channels;
};
