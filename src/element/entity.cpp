#define GLM_ENABLE_EXPERIMENTAL
#include "entity.h"
#include <algorithm>
#include <array>
#include <glm/glm.hpp>
#include <glm/geometric.hpp>
#include <glm/gtx/norm.hpp>

Entity::Entity(std::shared_ptr<Model> m, std::shared_ptr<Shader> s, std::shared_ptr<Texture> t,
               const glm::vec3 &pos, const glm::vec3 &rot, const glm::vec3 &scl)
    : Object(m, s, t, pos, rot, scl), accel(0.0f), velocity(0.0f), maxSpeed(0.1f), friction(0.05f)
{
}

// bool Entity::BroadPhaseCheck(const Entity &other) const
// {
//     auto transformVec3 = [&](const glm::vec3 &v, const glm::mat4 &m)
//     {
//         return glm::vec3(m * glm::vec4(v, 1.0f));
//     };

//     glm::vec3 centerA = transformVec3(GetModel()->boundingSphere.center, GetWorldMatrix());
//     glm::vec3 centerB = transformVec3(other.GetModel()->boundingSphere.center, other.GetWorldMatrix());
//     float dist = glm::length(centerA - centerB);
//     return dist <= (GetModel()->boundingSphere.radius + other.GetModel()->boundingSphere.radius);
// }

bool Entity::BroadPhaseCheck(const Entity &other) const
{
    auto transformVec3 = [](const glm::vec3 &v, const glm::mat4 &m)
    {
        return glm::vec3(m * glm::vec4(v, 1.0f));
    };

    auto computeAABB = [&](const AABB &bb, const glm::mat4 &world)
    {
        // 8 corner points of the bounding box
        glm::vec3 corners[8] = {
            {bb.min.x, bb.min.y, bb.min.z},
            {bb.max.x, bb.min.y, bb.min.z},
            {bb.min.x, bb.max.y, bb.min.z},
            {bb.max.x, bb.max.y, bb.min.z},
            {bb.min.x, bb.min.y, bb.max.z},
            {bb.max.x, bb.min.y, bb.max.z},
            {bb.min.x, bb.max.y, bb.max.z},
            {bb.max.x, bb.max.y, bb.max.z}
        };

        glm::vec3 minPt( std::numeric_limits<float>::max());
        glm::vec3 maxPt(-std::numeric_limits<float>::max());

        for (int i = 0; i < 8; i++)
        {
            glm::vec3 t = transformVec3(corners[i], world);
            minPt = glm::min(minPt, t);
            maxPt = glm::max(maxPt, t);
        }

        return std::make_pair(minPt, maxPt);
    };

    auto [minA, maxA] = computeAABB(GetModel()->boundingBox, GetWorldMatrix());
    auto [minB, maxB] = computeAABB(other.GetModel()->boundingBox, other.GetWorldMatrix());

    // AABB overlap test
    return (minA.x <= maxB.x && maxA.x >= minB.x) &&
           (minA.y <= maxB.y && maxA.y >= minB.y) &&
           (minA.z <= maxB.z && maxA.z >= minB.z);
}


bool Entity::NarrowPhaseCheck(const Entity &other) const
{
    // std::cout << "Entity::NarrowPhaseCheck" << std::endl;
    if (!BroadPhaseCheck(other))
    {
        return false;
    }


    auto transformVec3 = [&](const glm::vec3 &v, const glm::mat4 &m)
    {
        return glm::vec3(m * glm::vec4(v, 1.0f));
    };

    for (const auto &meshA : GetModel()->hitboxMeshes)
    {
        for (size_t i = 0; i < meshA.indices.size(); i += 3)
        {
            glm::vec3 a0 = transformVec3(meshA.vertices[meshA.indices[i]].Position, GetWorldMatrix());
            glm::vec3 a1 = transformVec3(meshA.vertices[meshA.indices[i + 1]].Position, GetWorldMatrix());
            glm::vec3 a2 = transformVec3(meshA.vertices[meshA.indices[i + 2]].Position, GetWorldMatrix());

            for (const auto &meshB : other.GetModel()->hitboxMeshes)
            {
                for (size_t j = 0; j < meshB.indices.size(); j += 3)
                {
                    glm::vec3 b0 = transformVec3(meshB.vertices[meshB.indices[j]].Position, other.GetWorldMatrix());
                    glm::vec3 b1 = transformVec3(meshB.vertices[meshB.indices[j + 1]].Position, other.GetWorldMatrix());
                    glm::vec3 b2 = transformVec3(meshB.vertices[meshB.indices[j + 2]].Position, other.GetWorldMatrix());

                    if (TriangleIntersect(a0, a1, a2, b0, b1, b2))
                        return true;
                }
            }
        }
    }
    return false;
}

bool Entity::TriangleIntersect(const glm::vec3 &v0, const glm::vec3 &v1, const glm::vec3 &v2,
                               const glm::vec3 &u0, const glm::vec3 &u1, const glm::vec3 &u2) const
{
    auto projectOntoAxis = [](const std::array<glm::vec3, 3> &tri, const glm::vec3 &axis, float &minProj, float &maxProj)
    {
        minProj = maxProj = glm::dot(tri[0], axis);
        for (int i = 1; i < 3; i++)
        {
            float p = glm::dot(tri[i], axis);
            minProj = std::min(minProj, p);
            maxProj = std::max(maxProj, p);
        }
    };

    auto overlapOnAxis = [&](const std::array<glm::vec3, 3> &t1, const std::array<glm::vec3, 3> &t2, const glm::vec3 &axis)
    {
        if (glm::length2(axis) < 1e-8f)
            return true;
        glm::vec3 nAxis = glm::normalize(axis);

        float min1, max1, min2, max2;
        projectOntoAxis(t1, nAxis, min1, max1);
        projectOntoAxis(t2, nAxis, min2, max2);

        return !(max1 < min2 || max2 < min1);
    };

    std::array<glm::vec3, 3> T1 = {v0, v1, v2};
    std::array<glm::vec3, 3> T2 = {u0, u1, u2};

    glm::vec3 e1[3] = {v1 - v0, v2 - v1, v0 - v2};
    glm::vec3 e2[3] = {u1 - u0, u2 - u1, u0 - u2};

    glm::vec3 n1 = glm::cross(e1[0], e1[1]);
    glm::vec3 n2 = glm::cross(e2[0], e2[1]);

    std::vector<glm::vec3> axes;

    // pháp tuyến
    axes.push_back(n1);
    axes.push_back(n2);

    // 9 cross products giữa cạnh
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            axes.push_back(glm::cross(e1[i], e2[j]));

    // kiểm tra từng axis
    for (auto &axis : axes)
    {
        if (!overlapOnAxis(T1, T2, axis))
            return false; // tách được => không giao nhau
    }

    return true; // không tìm được separating axis => giao nhau
}