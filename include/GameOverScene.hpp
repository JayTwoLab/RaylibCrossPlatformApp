#pragma once

#include <string>

#include "Engine/Engine.hpp"

class GameOverScene : public Engine::Scene::Scene {
private:
    Engine::UI::ClickableAreaManager clickManager_;

public:
    GameOverScene();

    void Init() override;
    void Update() override;
    void Draw() override;
    void Unload() override;

    std::string GetNextScene() override;
};
