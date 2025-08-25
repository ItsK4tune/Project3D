#pragma once
#include <unordered_map>
#include <vector>
#include <string>
#include <memory>
#include "object.h"
#include "camera.h"

class SceneManager
{
public:
    static SceneManager &Instance();

    SceneManager(const SceneManager &) = delete;
    SceneManager &operator=(const SceneManager &) = delete;

    bool LoadFromFile(const std::string& filePath);
    bool AddFromFile(const std::string& filePath);
    bool AddObject(const std::shared_ptr<Object>& object);
    bool AddHUD(const std::shared_ptr<Object>& hud);
    bool SetCamera(const std::shared_ptr<Camera>& camera);
    void Cleanup();
    
    std::shared_ptr<Camera> GetCamera() const { return camera; }
    std::vector<std::shared_ptr<Object>> GetObjects() const { return objects; }
    std::unordered_map<std::string, std::shared_ptr<Object>> GetHUDs() const { return huds; }
    
    void Update(float deltaTime);
    
    void Draw();
    void DrawHUD();

private:
    std::shared_ptr<Camera> camera;

    std::vector<std::shared_ptr<Object>> objects;
    std::unordered_map<std::string, std::shared_ptr<Object>> huds; 

    SceneManager() = default;
    ~SceneManager();
};
