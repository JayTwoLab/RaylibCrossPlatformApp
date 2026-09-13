#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include "raylib-cpp.hpp"
#include "Engine/Engine.hpp"

class TitleScene : public Engine::Scene::Scene {
public:
    TitleScene();

    void Init() override;
    void Update() override;
    void Draw() override;
    void DrawImGui() override;
    void Unload() override;
    std::string GetNextScene() override;

private:
    raylib::Music bgm_;
    float soundVolume_{ 0.5f };
    std::unordered_map<std::string, raylib::Sound> sounds_;

    Engine::UI::ClickableAreaManager clickManager_;

    raylib::Vector2 lastClickedPos_{ -1.0f, -1.0f };
    raylib::Rectangle dragBox_{ 150.0f, 280.0f, 160.0f, 100.0f };
    bool isDragging_{ false };
    raylib::Vector2 dragOffset_{ 0.0f, 0.0f };
};
