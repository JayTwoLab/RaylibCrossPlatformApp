#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <iostream>
#include <map>
#include "Engine/Engine.hpp"

class TitleScene : public Engine::Scene::Scene {
private:
    raylib::Music bgm_; // background music
    float soundVolume_ = 0.5; // sound volume (0.0 ~ 1.0)
    std::unordered_map<std::string, raylib::Sound> sounds_; // list of sound effects    

    Engine::UI::ClickableAreaManager clickManager_; // click manager object

    raylib::Vector2 lastClickedPos_ = { -1.0f, -1.0f }; // variable storing click coordinates

    // drag-related variables
    raylib::Rectangle dragBox_ = { 100, 100, 120, 80 }; // box to drag
    bool isDragging_ = false;                           // dragging state flag
    raylib::Vector2 dragOffset_ = { 0.0f, 0.0f };       // mouse click offset

public:
    TitleScene(); 

    void Init() override;
    void Update() override;
    void Draw() override;
    void Unload() override;

    std::string GetNextScene() override;
};
