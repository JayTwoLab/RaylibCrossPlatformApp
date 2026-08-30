#pragma once

#include <string>
#include "raylib-cpp.hpp"
#include "Engine/Engine.hpp"

class GameOverScene : public Engine::Scene::Scene {
private:
    Engine::UI::ClickableAreaManager clickManager_;

    std::shared_ptr<raylib::Font> nanumBoldFont_;

public:
    GameOverScene();

    void Init() override;
    void Update() override;
    void Draw() override;
    void Unload() override;

    std::string GetNextScene() override;
};
