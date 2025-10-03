#pragma once
#include <map>
#include <string>
#include <vector>
#include <assimp/scene.h>
#include "bone.h"
#include "model.h"

class Animation {
public:
    Animation(aiAnimation* animation, Model* model, const aiNode* root);

    Bone* FindBone(const std::string& name);
    inline float GetTicksPerSecond() const { return ticksPerSecond; }
    inline float GetDuration() const { return duration; }
    inline const aiNode* GetRootNode() const { return rootNode; }
    inline const std::map<std::string, BoneInfo>& GetBoneInfoMap() const { return boneInfoMap; }
    inline const std::string& GetName() const { return name; }

private:
    std::string name;
    float duration;
    float ticksPerSecond;
    std::map<std::string, BoneInfo> boneInfoMap;
    std::vector<Bone> bones;
    const aiNode* rootNode;

    void ReadMissingBones(const aiAnimation* animation, Model& model);
};
