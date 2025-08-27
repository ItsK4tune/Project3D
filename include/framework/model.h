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
    float radiusSquared;;
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

        for (size_t meshIdx = 0; meshIdx < renderMeshes.size(); ++meshIdx) {
            const auto &mesh = renderMeshes[meshIdx];
            std::cout << "[Model] Render mesh " << meshIdx << " has " << mesh.vertices.size() << " vertices and " << mesh.indices.size() / 3 << " triangles." << std::endl;
            for (size_t v = 0; v < mesh.vertices.size(); ++v) {
            const auto &vert = mesh.vertices[v];
            std::cout << "  Vertex " << v << ": Position(" 
                  << vert.Position.x << ", " << vert.Position.y << ", " << vert.Position.z << ")" << std::endl;
            }
        }
        for (size_t meshIdx = 0; meshIdx < hitboxMeshes.size(); ++meshIdx) {
            const auto &mesh = hitboxMeshes[meshIdx];
            std::cout << "[Model] Hitbox mesh " << meshIdx << " has " << mesh.vertices.size() << " vertices and " << mesh.indices.size() / 3 << " triangles." << std::endl;
            for (size_t v = 0; v < mesh.vertices.size(); ++v) {
            const auto &vert = mesh.vertices[v];
            std::cout << "  Vertex " << v << ": Position(" 
                  << vert.Position.x << ", " << vert.Position.y << ", " << vert.Position.z << ")" << std::endl;
            }
        }
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
