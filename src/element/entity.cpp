#define GLM_ENABLE_EXPERIMENTAL
#include "entity.h"
#include <algorithm>
#include <array>
#include <glm/glm.hpp>
#include <glm/geometric.hpp>
#include <glm/gtx/norm.hpp>

Entity::Entity(std::shared_ptr<Model> m, std::shared_ptr<Shader> s, std::shared_ptr<Texture> t,
               const glm::vec3 &pos, const glm::vec3 &rot, const glm::vec3 &scl)
    : Object(m, s, t, pos, rot, scl), accel(0.0f), velocity(0.0f), maxSpeed(10.0f), friction(0.05f)
{
}

inline glm::vec3 transformVec3(const glm::vec3 &v, const glm::mat4 &m)
{
    return glm::vec3(m * glm::vec4(v, 1.0f));
}

inline glm::vec3 SlideAlongPlane(const glm::vec3 &vel, const glm::vec3 &normal)
{
    float vn = glm::dot(vel, normal);
    glm::vec3 vNormal = vn * normal;
    glm::vec3 vTangent = vel - vNormal;

    // giữ nguyên tangential, scale lại normal
    return vTangent - 0.5f * vNormal;
}

inline bool RayIntersectPlane(const glm::vec3 &p, const glm::vec3 &dir, const glm::vec3 &n, float d, float tMax, float &outT, float eps = 1e-6f)
{
    float denom = glm::dot(n, dir);
    if (glm::abs(denom) < eps)
        return false;
    float t = (d - glm::dot(n, p)) / denom;
    if (t >= -eps && t <= tMax + eps)
    {
        outT = t;
        return true;
    }
    return false;
}

inline bool PointInTriangle(const glm::vec3 &p, const glm::vec3 &a, const glm::vec3 &b, const glm::vec3 &c)
{
    glm::vec3 v0 = c - a;
    glm::vec3 v1 = b - a;
    glm::vec3 v2 = p - a;

    glm::vec3 n = glm::cross(v1, v0);
    float area2 = glm::length(n);
    if (area2 < 1e-9f)
        return false;

    glm::vec3 c0 = glm::cross(b - p, c - p);
    glm::vec3 c1 = glm::cross(c - p, a - p);
    glm::vec3 c2 = glm::cross(a - p, b - p);

    if (glm::dot(c0, n) >= -1e-6f && glm::dot(c1, n) >= -1e-6f && glm::dot(c2, n) >= -1e-6f)
        return true;
    return false;
}

inline std::pair<glm::vec3, glm::vec3> computeAABB(const AABB &bb, const glm::mat4 &world)
{
    glm::vec3 corners[8] = {
        {bb.min.x, bb.min.y, bb.min.z},
        {bb.max.x, bb.min.y, bb.min.z},
        {bb.min.x, bb.max.y, bb.min.z},
        {bb.max.x, bb.max.y, bb.min.z},
        {bb.min.x, bb.min.y, bb.max.z},
        {bb.max.x, bb.min.y, bb.max.z},
        {bb.min.x, bb.max.y, bb.max.z},
        {bb.max.x, bb.max.y, bb.max.z}};

    glm::vec3 minPt(std::numeric_limits<float>::max());
    glm::vec3 maxPt(-std::numeric_limits<float>::max());

    for (int i = 0; i < 8; i++)
    {
        glm::vec3 t = transformVec3(corners[i], world);
        minPt = glm::min(minPt, t);
        maxPt = glm::max(maxPt, t);
    }

    return std::make_pair(minPt, maxPt);
}

bool Entity::BroadPhaseCheck(const Entity &other) const
{
    auto [minA, maxA] = computeAABB(GetModel()->boundingBox, GetWorldMatrix());
    auto [minB, maxB] = computeAABB(other.GetModel()->boundingBox, other.GetWorldMatrix());

    return (minA.x <= maxB.x && maxA.x >= minB.x) &&

           (minA.y <= maxB.y && maxA.y >= minB.y) &&
           (minA.z <= maxB.z && maxA.z >= minB.z);
}

bool Entity::NarrowPhaseCheck(const Entity &other, float deltaTime)
{
    if (&other == this) return false;

    // --- build trisB như code cũ ---
    struct Tri {
        glm::vec3 a, b, c;
        glm::vec3 normal;
        float d;
    };
    std::vector<Tri> trisB;
    trisB.reserve(256);

    for (const auto &meshB : other.GetModel()->hitboxMeshes) {
        for (size_t j = 0; j + 2 < meshB.indices.size(); j += 3) {
            glm::vec3 b0 = transformVec3(meshB.vertices[meshB.indices[j]].Position, other.GetWorldMatrix());
            glm::vec3 b1 = transformVec3(meshB.vertices[meshB.indices[j+1]].Position, other.GetWorldMatrix());
            glm::vec3 b2 = transformVec3(meshB.vertices[meshB.indices[j+2]].Position, other.GetWorldMatrix());

            glm::vec3 n = glm::cross(b1 - b0, b2 - b0);
            float ln = glm::length(n);
            if (ln < 1e-8f) continue;
            n /= ln;
            float d = glm::dot(n, b0);
            trisB.push_back({b0,b1,b2,n,d});
        }
    }
    if (trisB.empty()) return false;

    // --- substep loop ---
    glm::vec3 vel = GetVelocity();
    glm::vec3 pos = GetPosition();

    // số substep: phụ thuộc vào vận tốc và dt
    float maxStep = 0.1f;
    float moveLen = glm::length(vel * deltaTime);
    int numSteps = std::max(1, (int)std::ceil(moveLen / maxStep));

    float dtStep = deltaTime / (float)numSteps;

    bool collided = false;

    for (int step = 0; step < numSteps; ++step) {
        glm::vec3 moveTotal = vel * dtStep;

        // broad phase
        auto [minA_ws, maxA_ws] = computeAABB(GetModel()->boundingBox, GetWorldMatrix());
        auto [minB_ws, maxB_ws] = computeAABB(other.GetModel()->boundingBox, other.GetWorldMatrix());

        glm::vec3 sweptMin = glm::min(minA_ws, minA_ws + moveTotal);
        glm::vec3 sweptMax = glm::max(maxA_ws, maxA_ws + moveTotal);

        if (!(sweptMin.x <= maxB_ws.x && sweptMax.x >= minB_ws.x &&
              sweptMin.y <= maxB_ws.y && sweptMax.y >= minB_ws.y &&
              sweptMin.z <= maxB_ws.z && sweptMax.z >= minB_ws.z)) {
            // không va chạm trong step này → di chuyển bình thường
            pos += moveTotal;
            SetPosition(pos);
            continue;
        }

        // narrow phase (như code cũ, nhưng chỉ cho step này)
        std::array<glm::vec3,8> corners = {
            glm::vec3(minA_ws.x, minA_ws.y, minA_ws.z),
            glm::vec3(maxA_ws.x, minA_ws.y, minA_ws.z),
            glm::vec3(minA_ws.x, maxA_ws.y, minA_ws.z),
            glm::vec3(maxA_ws.x, maxA_ws.y, minA_ws.z),
            glm::vec3(minA_ws.x, minA_ws.y, maxA_ws.z),
            glm::vec3(maxA_ws.x, minA_ws.y, maxA_ws.z),
            glm::vec3(minA_ws.x, maxA_ws.y, maxA_ws.z),
            glm::vec3(maxA_ws.x, maxA_ws.y, maxA_ws.z)
        };

        float globalEarliestT = 1.0f + 1e-6f;
        const Tri* globalHitTri = nullptr;

        for (const auto &pt : corners) {
            glm::vec3 start = pt;
            for (const auto &tri : trisB) {
                float tOnRay;
                if (!RayIntersectPlane(start, moveTotal, tri.normal, tri.d, 1.0f, tOnRay))
                    continue;

                glm::vec3 ip = start + moveTotal * tOnRay;
                if (!PointInTriangle(ip, tri.a, tri.b, tri.c)) continue;

                if (glm::dot(tri.normal, moveTotal) >= 0.0f) continue;

                if (tOnRay < globalEarliestT) {
                    globalEarliestT = tOnRay;
                    globalHitTri = &tri;
                }
            }
        }

        if (!globalHitTri) {
            pos += moveTotal;
            SetPosition(pos);
            continue;
        }

        // xử lý va chạm trong step này
        float moveT = std::max(0.0f, globalEarliestT - 1e-5f);
        glm::vec3 newCenter = pos + moveTotal * moveT;

        // đẩy ra ngoài mặt phẳng
        float minSigned = std::numeric_limits<float>::max();
        for (const auto &c : corners) {
            glm::vec3 movedCorner = c + moveTotal * moveT;
            float signedDist = glm::dot(globalHitTri->normal, movedCorner) - globalHitTri->d;
            minSigned = std::min(minSigned, signedDist);
        }
        if (minSigned < 0.0f)
            newCenter += globalHitTri->normal * ((-minSigned) + 0.01f);

        pos = newCenter;
        SetPosition(pos);

        // tính velocity mới
        glm::vec3 slid = SlideAlongPlane(vel, globalHitTri->normal);
        if (glm::length2(slid) < 1e-6f) slid = glm::vec3(0.0f);
        vel = slid;
        SetVelocity(vel);

        collided = true;
    }

    return collided;
}
