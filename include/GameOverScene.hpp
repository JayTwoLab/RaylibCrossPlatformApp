#pragma once

#include <string>
#include <memory>
#include "raylib-cpp.hpp"
#include "Engine/Engine.hpp"

class GameOverScene : public Engine::Scene::Scene {
public:
    GameOverScene();

    void Init() override;
    void Update() override;
    void Draw() override;
    void Unload() override;
    std::string GetNextScene() override;

private:
    Engine::UI::ClickableAreaManager clickManager_;

};
