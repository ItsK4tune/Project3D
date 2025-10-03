#include "model.h"
#include <cfloat>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include "animation.h"

Model::Model(const std::string &i, const std::string &path)
    : id(i)
{
    LoadModel(path);
}

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

    for (int i = 0; i < scene->mNumAnimations; i++)
    {
        aiAnimation *anim = scene->mAnimations[i];
        std::shared_ptr<Animation> newAnim = std::make_shared<Animation>(anim, this, scene->mRootNode);
        animations[newAnim->GetName()] = std::move(newAnim);
    }
}

static glm::mat4 aiMat4ToGlm(const aiMatrix4x4 &m)
{
    return glm::mat4(
        m.a1, m.b1, m.c1, m.d1,
        m.a2, m.b2, m.c2, m.d2,
        m.a3, m.b3, m.c3, m.d3,
        m.a4, m.b4, m.c4, m.d4);
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
    const float EPS = 1e-6f;

    if (isHitbox)
    {
        // 1) compute min/max in mesh LOCAL space (don't multiply by normalMatrix!)
        glm::vec3 minLocal(FLT_MAX);
        glm::vec3 maxLocal(-FLT_MAX);

        for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
        {
            glm::vec3 vLocal(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
            minLocal = glm::min(minLocal, vLocal);
            maxLocal = glm::max(maxLocal, vLocal);
        }

        glm::vec3 centerLocal = (minLocal + maxLocal) * 0.5f;
        glm::vec3 halfSizeLocal = (maxLocal - minLocal) * 0.5f;

        // 2) extract columns from nodeTransform (model-space transform for this node)
        glm::vec3 col0 = glm::vec3(nodeTransform[0]); // column 0
        glm::vec3 col1 = glm::vec3(nodeTransform[1]);
        glm::vec3 col2 = glm::vec3(nodeTransform[2]);

        // 3) scale per axis = length of columns (handles non-uniform scale)
        glm::vec3 scaleVec(glm::length(col0), glm::length(col1), glm::length(col2));
        if (scaleVec.x < EPS)
            scaleVec.x = 1.0f;
        if (scaleVec.y < EPS)
            scaleVec.y = 1.0f;
        if (scaleVec.z < EPS)
            scaleVec.z = 1.0f;

        // 4) normalize columns to get a basis (remove scale)
        glm::vec3 c0n = col0 / scaleVec.x;
        glm::vec3 c1n = col1 / scaleVec.y;
        glm::vec3 c2n = col2 / scaleVec.z;

        // Gram-Schmidt to ensure orthonormal axes
        glm::vec3 x = glm::normalize(c0n);
        glm::vec3 y = c1n - glm::dot(c1n, x) * x;
        float ylen = glm::length(y);
        if (ylen < EPS)
        {
            // pick arbitrary perpendicular
            if (fabs(x.x) < 0.9f)
                y = glm::normalize(glm::cross(x, glm::vec3(1, 0, 0)));
            else
                y = glm::normalize(glm::cross(x, glm::vec3(0, 1, 0)));
        }
        else
        {
            y = glm::normalize(y);
        }
        glm::vec3 z = glm::cross(x, y);

        // keep handedness consistent with original c2n
        if (glm::dot(z, c2n) < 0.0f)
        {
            z = -z;
            y = glm::cross(z, x); // re-orthonormalize y
        }

        glm::mat3 orientation;
        // glm::mat3 constructor takes columns as arguments
        orientation = glm::mat3(x, y, z);

        // 5) half size in world/model space = local halfSize * scaleVec (component-wise)
        glm::vec3 halfSizeWorld = halfSizeLocal * scaleVec;

        // 6) center in model space (apply full nodeTransform to local center)
        glm::vec3 centerWorld = glm::vec3(nodeTransform * glm::vec4(centerLocal, 1.0f));

        OBB obb;
        obb.center = centerWorld;
        obb.halfSize = halfSizeWorld;
        obb.orientation = orientation;

        boundingBoxs.push_back(obb);
    }

    // --- build mesh vertices (baked to model-space) ---
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
            if (mesh->HasNormals())
            {
                glm::vec3 normal(mesh->mNormals[i].x,
                                 mesh->mNormals[i].y,
                                 mesh->mNormals[i].z);
                vertex.Normal = glm::normalize(normalMatrix * normal);
            }
            else
                vertex.Normal = glm::vec3(0.0f, 1.0f, 0.0f); // fallback

            if (mesh->mTextureCoords[0])
                vertex.TexCoords = glm::vec2(mesh->mTextureCoords[0][i].x,
                                             mesh->mTextureCoords[0][i].y);
            else
                vertex.TexCoords = glm::vec2(0.0f, 0.0f);

            if (mesh->HasTangentsAndBitangents())
            {
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
                vertex.Tangent = glm::vec3(0.0f);
                vertex.Bitangent = glm::vec3(0.0f);
            }
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

    // indices
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
    for (unsigned int i = 0; i < aimesh->mNumBones; i++)
    {
        int boneID = -1;
        std::string boneName = aimesh->mBones[i]->mName.C_Str();

        if (boneIDMap.find(boneName) == boneIDMap.end())
        {
            BoneInfo bi;
            bi.id = boneCount;
            bi.offset = glm::transpose(glm::make_mat4(&aimesh->mBones[i]->mOffsetMatrix.a1));
            boneInfoMap[boneName] = bi;
            boneID = boneCount;
            boneIDMap[boneName] = boneCount;
            boneCount++;
        }
        else
        {
            boneID = boneIDMap[boneName];
        }

        assert(boneID != -1);

        auto weights = aimesh->mBones[i]->mWeights;
        int numWeights = aimesh->mBones[i]->mNumWeights;

        for (int j = 0; j < numWeights; j++)
        {
            int vertexID = weights[j].mVertexId;
            float weight = weights[j].mWeight;

            for (int k = 0; k < MAX_BONE_INFLUENCE; k++)
            {
                if (mesh.vertices[vertexID].Weights[k] == 0.0f)
                {
                    mesh.vertices[vertexID].BoneIDs[k] = boneID;
                    mesh.vertices[vertexID].Weights[k] = weight;
                    break;
                }
            }
        }
    }
}
