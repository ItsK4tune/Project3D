#pragma once
#include <string>
#include <vector>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <iostream>
#include "mesh.h"

struct BoundingSphere
{
    glm::vec3 center;
    float radius;
};

class Model
{
public:
    std::vector<Mesh> renderMeshes;
    std::vector<Mesh> hitboxMeshes;
    BoundingSphere boundingSphere;

    Model(const std::string &path)
    {
        LoadModel(path);
        CalculateBoundingSphere();
    }

    void DrawObjects();
    void DrawHitboxes();

private:
    void LoadModel(const std::string &path);
    void ProcessNode(aiNode *node, const aiScene *scene);
    Mesh ProcessMesh(aiMesh *mesh, const aiScene *scene, bool isHitbox);

    void ExtractBoneWeightForVertices(Mesh &mesh, aiMesh *aimesh, const aiScene *scene);
    void CalculateBoundingSphere();
};
