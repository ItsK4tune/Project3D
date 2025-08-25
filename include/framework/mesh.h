#pragma once
#include <vector>
#include <glad/glad.h>
#include "Vertex.h"

class Mesh
{
public:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices)
        : vertices(vertices), indices(indices)
    {
        SetupMesh();
    }

    void Draw();

private:
    unsigned int VAO, VBO, EBO;
    void SetupMesh();
};
