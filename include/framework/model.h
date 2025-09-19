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

    Model(const std::string &i, const std::string &path);

    void SetID(const std::string &id) { this->id = id; }
    std::string GetID() const { return id; }

    void DrawObjects();
    void DrawHitboxes();

private:
    std::string id;

    void LoadModel(const std::string &path);
    void ProcessNode(aiNode *node, const aiScene *scene, const glm::mat4 &parentTransform = glm::mat4(1.0f));
    Mesh ProcessMesh(aiMesh *mesh, const aiScene *scene, bool isHitbox, const glm::mat4 &nodeTransform = glm::mat4(1.0f));

    void ExtractBoneWeightForVertices(Mesh &mesh, aiMesh *aimesh, const aiScene *scene);
    void CalculateBoundingBox();
};
