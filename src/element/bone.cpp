#define GLM_ENABLE_EXPERIMENTAL
#include "bone.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

Bone::Bone(const std::string& name, int ID, const aiNodeAnim* channel)
    : name(name), id(ID), localTransform(1.0f)
{
    // positions
    for (unsigned int i = 0; i < channel->mNumPositionKeys; i++) {
        KeyPosition data;
        data.position = glm::vec3(channel->mPositionKeys[i].mValue.x,
                                  channel->mPositionKeys[i].mValue.y,
                                  channel->mPositionKeys[i].mValue.z);
        data.timeStamp = (float)channel->mPositionKeys[i].mTime;
        positions.push_back(data);
    }
    numPositions = positions.size();

    // rotations
    for (unsigned int i = 0; i < channel->mNumRotationKeys; i++) {
        KeyRotation data;
        auto q = channel->mRotationKeys[i].mValue;
        data.orientation = glm::quat(q.w, q.x, q.y, q.z);
        data.timeStamp = (float)channel->mRotationKeys[i].mTime;
        rotations.push_back(data);
    }
    numRotations = rotations.size();

    // scales
    for (unsigned int i = 0; i < channel->mNumScalingKeys; i++) {
        KeyScale data;
        data.scale = glm::vec3(channel->mScalingKeys[i].mValue.x,
                               channel->mScalingKeys[i].mValue.y,
                               channel->mScalingKeys[i].mValue.z);
        data.timeStamp = (float)channel->mScalingKeys[i].mTime;
        scales.push_back(data);
    }
    numScalings = scales.size();
}

void Bone::Update(float animationTime) {
    glm::mat4 translation = InterpolatePosition(animationTime);
    glm::mat4 rotation = InterpolateRotation(animationTime);
    glm::mat4 scale = InterpolateScaling(animationTime);
    localTransform = translation * rotation * scale;
}

int Bone::GetPositionIndex(float animationTime) {
    for (int i = 0; i < numPositions - 1; i++) {
        if (animationTime < positions[i + 1].timeStamp)
            return i;
    }
    return numPositions - 1;
}

int Bone::GetRotationIndex(float animationTime) {
    for (int i = 0; i < numRotations - 1; i++) {
        if (animationTime < rotations[i + 1].timeStamp)
            return i;
    }
    return numRotations - 1;
}

int Bone::GetScaleIndex(float animationTime) {
    for (int i = 0; i < numScalings - 1; i++) {
        if (animationTime < scales[i + 1].timeStamp)
            return i;
    }
    return numScalings - 1;
}

float Bone::GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime) {
    float scaleFactor = (animationTime - lastTimeStamp) / (nextTimeStamp - lastTimeStamp);
    return glm::clamp(scaleFactor, 0.0f, 1.0f);
}

glm::mat4 Bone::InterpolatePosition(float animationTime) {
    if (numPositions == 1) {
        return glm::translate(glm::mat4(1.0f), positions[0].position);
    }
    int p0Index = GetPositionIndex(animationTime);
    int p1Index = p0Index + 1;
    float scaleFactor = GetScaleFactor(positions[p0Index].timeStamp,
                                       positions[p1Index].timeStamp, animationTime);
    glm::vec3 finalPos = glm::mix(positions[p0Index].position,
                                  positions[p1Index].position, scaleFactor);
    return glm::translate(glm::mat4(1.0f), finalPos);
}

glm::mat4 Bone::InterpolateRotation(float animationTime) {
    if (numRotations == 1) {
        auto q = glm::normalize(rotations[0].orientation);
        return glm::toMat4(q);
    }
    int p0Index = GetRotationIndex(animationTime);
    int p1Index = p0Index + 1;
    float scaleFactor = GetScaleFactor(rotations[p0Index].timeStamp,
                                       rotations[p1Index].timeStamp, animationTime);
    glm::quat finalRot = glm::slerp(rotations[p0Index].orientation,
                                    rotations[p1Index].orientation, scaleFactor);
    finalRot = glm::normalize(finalRot);
    return glm::toMat4(finalRot);
}

glm::mat4 Bone::InterpolateScaling(float animationTime) {
    if (numScalings == 1) {
        return glm::scale(glm::mat4(1.0f), scales[0].scale);
    }
    int p0Index = GetScaleIndex(animationTime);
    int p1Index = p0Index + 1;
    float scaleFactor = GetScaleFactor(scales[p0Index].timeStamp,
                                       scales[p1Index].timeStamp, animationTime);
    glm::vec3 finalScale = glm::mix(scales[p0Index].scale,
                                    scales[p1Index].scale, scaleFactor);
    return glm::scale(glm::mat4(1.0f), finalScale);
}
