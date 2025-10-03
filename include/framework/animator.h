#pragma once
#include <vector>
#include <map>
#include <memory>
#include <glm/glm.hpp>
#include "animation.h"

class Animator {
public:
    Animator(const std::map<std::string, std::shared_ptr<Animation>>& animations);

    void UpdateAnimation(float dt);
    void PlayAnimation(Animation* animation);
    void PlayAnimation(const std::string& name); // thêm hàm play theo tên

    std::vector<glm::mat4> GetFinalBoneMatrices();

private:
    void CalculateBoneTransform(const aiNode* node, glm::mat4 parentTransform);

    std::map<std::string, std::shared_ptr<Animation>> animations;
    std::vector<glm::mat4> finalBoneMatrices;
    Animation* currentAnimation;
    float currentTime;
};
