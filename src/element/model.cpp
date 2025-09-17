#include "model.h"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>

void Model::LoadModel(const std::string &path)
{
    Assimp::Importer importer;

    importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, false); 
    importer.SetPropertyInteger(AI_CONFIG_IMPORT_FBX_READ_ALL_GEOMETRY_LAYERS, 1);
    importer.SetPropertyInteger(AI_CONFIG_IMPORT_FBX_READ_ALL_MATERIALS, 1);
    importer.SetPropertyInteger(AI_CONFIG_IMPORT_FBX_READ_MATERIALS, 1);
    importer.SetPropertyInteger(AI_CONFIG_IMPORT_FBX_READ_CAMERAS, 0);
    importer.SetPropertyInteger(AI_CONFIG_IMPORT_FBX_READ_LIGHTS, 0);

    const aiScene *scene = importer.ReadFile(path,
                                             aiProcess_Triangulate |
                                             aiProcess_FlipUVs |
                                             aiProcess_CalcTangentSpace |
                                             aiProcess_GenNormals);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cerr << "[Model::loadModel] ERROR::ASSIMP::"
                  << importer.GetErrorString() << std::endl;
        return;
    }

    ProcessNode(scene->mRootNode, scene);
}

static glm::mat4 aiMat4ToGlm(const aiMatrix4x4 &m)
{
    return glm::mat4(
        m.a1, m.b1, m.c1, m.d1,
        m.a2, m.b2, m.c2, m.d2,
        m.a3, m.b3, m.c3, m.d3,
        m.a4, m.b4, m.c4, m.d4
    );
}

void Model::ProcessNode(aiNode *node, const aiScene *scene, const glm::mat4 &parentTransform)
{
    std::string nodeName(node->mName.C_Str());
    bool isHitboxCollection = (nodeName.find("Hitbox") != std::string::npos);
    aiMatrix4x4 aiMat = node->mTransformation;
    
    glm::mat4 glmTransform = parentTransform * aiMat4ToGlm(aiMat);

    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];

        if (isHitboxCollection)
            hitboxMeshes.push_back(ProcessMesh(mesh, scene, true, glmTransform));
        else
            renderMeshes.push_back(ProcessMesh(mesh, scene, false, glmTransform));
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        ProcessNode(node->mChildren[i], scene, glmTransform);
    }
}

Mesh Model::ProcessMesh(aiMesh *mesh, const aiScene *scene, bool isHitbox, const glm::mat4 &nodeTransform)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(nodeTransform)));

    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;

        glm::vec4 pos = nodeTransform * glm::vec4(mesh->mVertices[i].x,
                                                  mesh->mVertices[i].y,
                                                  mesh->mVertices[i].z,
                                                  1.0f);
        vertex.Position = glm::vec3(pos);

        if (!isHitbox)
        {
            glm::vec3 normal(mesh->mNormals[i].x,
                             mesh->mNormals[i].y,
                             mesh->mNormals[i].z);
            vertex.Normal = glm::normalize(normalMatrix * normal);

            if (mesh->mTextureCoords[0])
                vertex.TexCoords = glm::vec2(mesh->mTextureCoords[0][i].x,
                                             mesh->mTextureCoords[0][i].y);
            else
                vertex.TexCoords = glm::vec2(0.0f, 0.0f);

            glm::vec3 tangent(mesh->mTangents[i].x,
                              mesh->mTangents[i].y,
                              mesh->mTangents[i].z);
            glm::vec3 bitangent(mesh->mBitangents[i].x,
                                mesh->mBitangents[i].y,
                                mesh->mBitangents[i].z);

            vertex.Tangent = glm::normalize(normalMatrix * tangent);
            vertex.Bitangent = glm::normalize(normalMatrix * bitangent);
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
    if (!isHitbox)
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
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    for (auto &mesh : hitboxMeshes)
        mesh.Draw();
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
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

void Model::CalculateBoundingBox()
{
    if (hitboxMeshes.empty())
    {
        boundingBox.min = glm::vec3(0.0f);
        boundingBox.max = glm::vec3(0.0f);
        return;
    }

    glm::vec3 minPoint(FLT_MAX);
    glm::vec3 maxPoint(-FLT_MAX);

    for (const auto &mesh : hitboxMeshes)
    {
        for (const auto &vertex : mesh.vertices)
        {
            minPoint.x = std::min(minPoint.x, vertex.Position.x);
            minPoint.y = std::min(minPoint.y, vertex.Position.y);
            minPoint.z = std::min(minPoint.z, vertex.Position.z);

            maxPoint.x = std::max(maxPoint.x, vertex.Position.x);
            maxPoint.y = std::max(maxPoint.y, vertex.Position.y);
            maxPoint.z = std::max(maxPoint.z, vertex.Position.z);
        }
    }

    boundingBox.min = minPoint;
    boundingBox.max = maxPoint;

    const float epsilon = 0.001f;
    for (int i = 0; i < 3; i++)
    {
        if (boundingBox.min[i] == boundingBox.max[i])
        {
            boundingBox.min[i] -= epsilon;
            boundingBox.max[i] += epsilon;
        }
    }
}