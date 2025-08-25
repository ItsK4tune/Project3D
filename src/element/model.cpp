#include "model.h"
#include <iostream>

void Model::LoadModel(const std::string &path)
{
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(path,
                                             aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cerr << "[Model::loadModel] ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        return;
    }

    ProcessNode(scene->mRootNode, scene);
}

void Model::ProcessNode(aiNode *node, const aiScene *scene)
{
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(ProcessMesh(mesh, scene));
    }
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        ProcessNode(node->mChildren[i], scene);
    }
}

Mesh Model::ProcessMesh(aiMesh *mesh, const aiScene *scene)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;
        
        vertex.Position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
        vertex.Normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);

        if (mesh->mTextureCoords[0])
        vertex.TexCoords = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
        else
        vertex.TexCoords = glm::vec2(0.0f, 0.0f);

        vertex.Tangent = glm::vec3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);
        vertex.Bitangent = glm::vec3(mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z);

        vertices.push_back(vertex);
    }
    
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
        indices.push_back(face.mIndices[j]);
    }
    
    Mesh newMesh(vertices, indices);
    ExtractBoneWeightForVertices(newMesh, mesh, scene);
    
    return newMesh;
}

void Model::ExtractBoneWeightForVertices(Mesh &mesh, aiMesh *aimesh, const aiScene *scene)
{
    for (unsigned int boneIndex = 0; boneIndex < aimesh->mNumBones; ++boneIndex)
    {
        int boneID = boneIndex;
        aiBone *bone = aimesh->mBones[boneIndex];
        
        for (unsigned int weightIndex = 0; weightIndex < bone->mNumWeights; ++weightIndex)
        {
            unsigned int vertexId = bone->mWeights[weightIndex].mVertexId;
            float weight = bone->mWeights[weightIndex].mWeight;
            
            for (int i = 0; i < MAX_BONE_INFLUENCE; i++)
            {
                if (mesh.vertices[vertexId].BoneIDs[i] < 0)
                {
                    mesh.vertices[vertexId].BoneIDs[i] = boneID;
                    mesh.vertices[vertexId].Weights[i] = weight;
                    break;
                }
            }
        }
    }
}

void Model::Draw()
{
    for (auto &mesh : meshes)
    {
        mesh.Draw();
    }
}