#pragma once
#include <unordered_map>
#include <vector>
#include <string>
#include <memory>
#include "object.h"
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
    std::unordered_map<std::string, std::shared_ptr<Object>> GetObjects() const { return objects; }
    std::unordered_map<std::string, std::shared_ptr<HUD>> GetHUDs() const { return huds; }
    
    void DeactivateAll();
    void ActiveAll();
    void DeactivateObject(const std::string& id);
    void ActivateObject(const std::string& id);

    void Update(float deltaTime);
    void Draw();
    void DrawHUD();

private:
    std::shared_ptr<Camera> camera;

    std::unordered_map<std::string, std::shared_ptr<Object>> objects;
    std::unordered_map<std::string, std::shared_ptr<HUD>> huds; 

    SceneManager() = default;
    ~SceneManager();
};
