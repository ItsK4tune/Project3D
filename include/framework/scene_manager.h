#pragma once
#include <vector>
#include <string>
#include <memory>
#include "entity.h"
#include "hud.h"
#include "camera.h"

class SceneManager
{
public:
    static SceneManager &Instance();

    SceneManager(const SceneManager &) = delete;
    SceneManager &operator=(const SceneManager &) = delete;

    void LoadFromFile(const std::string& filePath);
    void Cleanup();
    
    std::shared_ptr<Camera> GetCamera() const { return camera; }
    std::vector<std::shared_ptr<Entity>> GetEntities() const { return entities; }
    std::vector<std::shared_ptr<HUD>> GetHUDs() const { return huds; }
    std::shared_ptr<Entity> GetEntity (const std::string &name) const;
    std::shared_ptr<HUD> GetHUD(const std::string &name) const;
    
    void DeactivateAll();
    void ActiveAll();
    void DeactivateEntity(const std::string& id);
    void ActivateEntity(const std::string& id);

    void Update(float deltaTime);
    void Draw();
    void DrawHUD();

private:
    std::shared_ptr<Camera> camera;

    std::vector<std::shared_ptr<Entity>> entities;
    std::vector<std::shared_ptr<HUD>> huds; 

    SceneManager() = default;
    ~SceneManager();
};
