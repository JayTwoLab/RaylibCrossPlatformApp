#pragma once

#include <string>
#include <vector>
#include "Engine/Engine.hpp"
#include "Engine/Graphics/RotatingSprite.hpp"

class GameplayScene : public Engine::Scene::Scene {
public:
    GameplayScene();

    void Init() override;
    void Update() override;
    void Draw() override;
    void Unload() override;
    std::string GetNextScene() override;

private:
    Engine::UI::ClickableAreaManager clickManager_;

    std::vector<Engine::Graphics::RotatingSprite*> renderList_;

    Engine::Graphics::RotatingSprite backgroundProp_;
    Engine::Graphics::RotatingSprite playerSprite_;
    Engine::Graphics::RotatingSprite floatingEffect_;

    int playerHp_;
    int score_;

    void ResetFields();
};
