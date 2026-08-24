#pragma once

#include <string>

#include "Engine/Engine.hpp"

class GameplayScene : public Engine::Scene::Scene {
private:
    std::string nextScene;
    Engine::UI::ClickableAreaManager clickManager;
    int playerHp;
    int score;
    raylib::Vector2 playerPos;

    void ResetFields();

public:
    GameplayScene(); 

    void Init() override;
    void Update() override;
    void Draw() override;
    void Unload() override;

    std::string GetNextScene() const override;
};