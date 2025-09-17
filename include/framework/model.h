#pragma once
#include <string>
#include <vector>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <iostream>
#include "mesh.h"

struct AABB
{
    glm::vec3 min;
    glm::vec3 max;
};

class Model
{
public:
    std::vector<Mesh> renderMeshes;
    std::vector<Mesh> hitboxMeshes;
    AABB boundingBox;

    Model(const std::string &path)
    {
        LoadModel(path);
        CalculateBoundingBox();

        for (const auto &mesh : hitboxMeshes)
        {
            for (const auto &vertex : mesh.vertices)
            {
                std::cout << "Hitbox Vertex: (" << vertex.Position.x << ", " << vertex.Position.y << ", " << vertex.Position.z << ")\n";
            }
        }
    }

    void DrawObjects();
    void DrawHitboxes();

private:
    void LoadModel(const std::string &path);
    void ProcessNode(aiNode *node, const aiScene *scene, const glm::mat4 &parentTransform = glm::mat4(1.0f));
    Mesh ProcessMesh(aiMesh *mesh, const aiScene *scene, bool isHitbox, const glm::mat4 &nodeTransform = glm::mat4(1.0f));

    void ExtractBoneWeightForVertices(Mesh &mesh, aiMesh *aimesh, const aiScene *scene);
    void CalculateBoundingBox();
};
