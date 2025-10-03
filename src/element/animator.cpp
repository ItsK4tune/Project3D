#include "animator.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cmath>

Animator::Animator(const std::map<std::string, std::shared_ptr<Animation>> &animations)
    : animations(animations), currentAnimation(nullptr), currentTime(0.0f)
{
    finalBoneMatrices.reserve(100);
    for (int i = 0; i < 100; i++)
        finalBoneMatrices.push_back(glm::mat4(1.0f));

    if (!animations.empty())
    {
        currentAnimation = animations.begin()->second.get();
    }
}

void Animator::UpdateAnimation(float dt)
{
    if (currentAnimation)
    {
        float ticksPerSecond = currentAnimation->GetTicksPerSecond();
        ticksPerSecond = ticksPerSecond != 0 ? ticksPerSecond : 25.0f;

        currentTime += ticksPerSecond * dt;
        currentTime = fmod(currentTime, currentAnimation->GetDuration());

        CalculateBoneTransform(currentAnimation->GetRootNode(), glm::mat4(1.0f));
    }
}

void Animator::PlayAnimation(Animation *animation)
{
    currentAnimation = animation;
    currentTime = 0.0f;
}

void Animator::PlayAnimation(const std::string &name)
{
    auto it = animations.find(name);
    if (it != animations.end())
    {
        currentAnimation = it->second.get();
        currentTime = 0.0f;
    }
    else
    {
        std::cerr << "Animation not found: " << name << std::endl;
    }
}

std::vector<glm::mat4> Animator::GetFinalBoneMatrices()
{
    return finalBoneMatrices;
}

void Animator::CalculateBoneTransform(const aiNode *node, glm::mat4 parentTransform)
{
    std::string nodeName(node->mName.data);
    glm::mat4 nodeTransform = glm::transpose(glm::make_mat4(&node->mTransformation.a1));

    Bone *bone = currentAnimation->FindBone(nodeName);
    if (bone)
    {
        bone->Update(currentTime);
        nodeTransform = bone->GetLocalTransform();
    }

    glm::mat4 globalTransform = parentTransform * nodeTransform;

    const auto &boneInfoMap = currentAnimation->GetBoneInfoMap();
    if (boneInfoMap.find(nodeName) != boneInfoMap.end())
    {
        int index = boneInfoMap.at(nodeName).id;
        glm::mat4 offset = boneInfoMap.at(nodeName).offset;
        finalBoneMatrices[index] = globalTransform * offset;
    }

    for (int i = 0; i < node->mNumChildren; i++)
    {
        CalculateBoneTransform(node->mChildren[i], globalTransform);
    }
}
