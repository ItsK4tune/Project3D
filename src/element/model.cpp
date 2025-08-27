#define GLM_ENABLE_EXPERIMENTAL
#include "model.h"
#include <iostream>
#include <glm/gtx/norm.hpp>

void Model::LoadModel(const std::string &path)
{
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(path,
                                             aiProcess_Triangulate |
                                                 aiProcess_FlipUVs |
                                                 aiProcess_CalcTangentSpace |
                                                 aiProcess_GenNormals);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cerr << "[Model::loadModel] ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        return;
    }

    ProcessNode(scene->mRootNode, scene);
}

void Model::ProcessNode(aiNode *node, const aiScene *scene)
{
    std::string nodeName(node->mName.C_Str());
    bool isHitboxCollection = (nodeName.find("Hitbox") != std::string::npos);

    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];

        if (isHitboxCollection)
            hitboxMeshes.push_back(ProcessMesh(mesh, scene, true));
        else
            renderMeshes.push_back(ProcessMesh(mesh, scene, false));
    }

    // recursive
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        ProcessNode(node->mChildren[i], scene);
    }
}

Mesh Model::ProcessMesh(aiMesh *mesh, const aiScene *scene, bool isHitbox)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;
        vertex.Position = glm::vec3(mesh->mVertices[i].x,
                                    mesh->mVertices[i].y,
                                    mesh->mVertices[i].z);

        if (!isHitbox) // hitbox không cần normal/uv
        {
            vertex.Normal = glm::vec3(mesh->mNormals[i].x,
                                      mesh->mNormals[i].y,
                                      mesh->mNormals[i].z);

            if (mesh->mTextureCoords[0])
                vertex.TexCoords = glm::vec2(mesh->mTextureCoords[0][i].x,
                                             mesh->mTextureCoords[0][i].y);
            else
                vertex.TexCoords = glm::vec2(0.0f, 0.0f);

            vertex.Tangent = glm::vec3(mesh->mTangents[i].x,
                                       mesh->mTangents[i].y,
                                       mesh->mTangents[i].z);
            vertex.Bitangent = glm::vec3(mesh->mBitangents[i].x,
                                         mesh->mBitangents[i].y,
                                         mesh->mBitangents[i].z);
        }
        else
        {
            vertex.Normal = glm::vec3(0, 0, 0);
            vertex.TexCoords = glm::vec2(0, 0);
            vertex.Tangent = glm::vec3(0, 0, 0);
            vertex.Bitangent = glm::vec3(0, 0, 0);
        }

        vertices.push_back(vertex);
    }

    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    Mesh newMesh(vertices, indices);
    if (!isHitbox) // hitbox thường không cần xương
        ExtractBoneWeightForVertices(newMesh, mesh, scene);

    return newMesh;
}

void Model::DrawObjects()
{
    for (auto &mesh : renderMeshes)
        mesh.Draw();
}

void Model::DrawHitboxes()
{
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // vẽ wireframe
    for (auto &mesh : hitboxMeshes)
        mesh.Draw();
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // reset lại
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

void Model::CalculateBoundingSphere()
{
    if (renderMeshes.empty())
    {
        boundingSphere.center = glm::vec3(0.0f);
        boundingSphere.radiusSquared = 0.0f;
        return;
    }

    // Gom tất cả các đỉnh từ meshes
    std::vector<glm::vec3> points;
    for (const auto &mesh : renderMeshes)
    {
        for (const auto &vertex : mesh.vertices)
            points.push_back(vertex.Position);
    }

    if (points.empty())
    {
        boundingSphere.center = glm::vec3(0.0f);
        boundingSphere.radiusSquared = 0.0f;
        return;
    }

    // Step 1: chọn p bất kỳ
    glm::vec3 p = points[0];

    // Step 2: tìm q xa nhất từ p
    glm::vec3 q = p;
    float maxDist2 = 0.0f;
    for (const auto &pt : points)
    {
        float dist2 = glm::length2(pt - p);
        if (dist2 > maxDist2)
        {
            maxDist2 = dist2;
            q = pt;
        }
    }

    // Step 3: tìm r xa nhất từ q
    glm::vec3 r = q;
    maxDist2 = 0.0f;
    for (const auto &pt : points)
    {
        float dist2 = glm::length2(pt - q);
        if (dist2 > maxDist2)
        {
            maxDist2 = dist2;
            r = pt;
        }
    }

    // Step 4: khởi tạo sphere
    glm::vec3 center = (q + r) * 0.5f;
    float radius2 = glm::length2(r - center); // bán kính bình phương

    // Step 5: mở rộng sphere nếu cần
    for (const auto &pt : points)
    {
        float dist2 = glm::length2(pt - center); // khoảng cách bình phương
        if (dist2 > radius2)
        {
            float dist = sqrt(dist2);     // khoảng cách thật
            float radius = sqrt(radius2); // bán kính thật
            float newRadius = (radius + dist) * 0.5f;
            float k = (newRadius - radius) / dist;
            center += (pt - center) * k;
            radius2 = newRadius * newRadius; // cập nhật bán kính bình phương
        }
    }

    boundingSphere.center = center;
    boundingSphere.radiusSquared = radius2;
}