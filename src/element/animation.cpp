#include "animation.h"
#include <iostream>

Animation::Animation(aiAnimation* animation, Model* model, const aiNode* root)
{
    name = animation->mName.C_Str();
    duration = (float)animation->mDuration;
    ticksPerSecond = (float)(animation->mTicksPerSecond != 0 ? animation->mTicksPerSecond : 25.0f);
    rootNode = root;

    ReadMissingBones(animation, *model);
}

Bone* Animation::FindBone(const std::string &name)
{
    for (auto &bone : bones)
    {
        if (bone.GetBoneName() == name)
            return &bone;
    }
    return nullptr;
}

void Animation::ReadMissingBones(const aiAnimation *animation, Model &model)
{
    int size = animation->mNumChannels;

    for (int i = 0; i < size; i++)
    {
        auto channel = animation->mChannels[i];
        std::string boneName = channel->mNodeName.data;

        if (model.boneIDMap.find(boneName) == model.boneIDMap.end())
        {
            model.boneIDMap[boneName] = model.boneCount;

            BoneInfo bi;
            bi.id = model.boneCount;
            bi.offset = glm::mat4(1.0f);

            model.boneInfoMap[boneName] = bi;

            model.boneCount++;
        }

        bones.push_back(Bone(boneName, model.boneIDMap[boneName], channel));
    }

    boneInfoMap = model.boneInfoMap;
}
