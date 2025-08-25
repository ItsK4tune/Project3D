#pragma once
#include <string>
#include <vector>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include "mesh.h"

class Model
{
public:
    std::vector<Mesh> meshes;

    Model(const std::string& path)
    {
        LoadModel(path);
    }

    void Draw();

private:
    void LoadModel(const std::string& path);
    void ProcessNode(aiNode* node, const aiScene* scene);
    Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);

    void ExtractBoneWeightForVertices(Mesh& mesh, aiMesh* aimesh, const aiScene* scene);
};
