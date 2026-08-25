#pragma once

#include <string>
#include <vector>
#include "Engine/Engine.hpp"
#include "Engine/Graphics/RotatingSprite.hpp"

class GameplayScene : public Engine::Scene::Scene {
private:
    std::string nextScene;
    Engine::UI::ClickableAreaManager clickManager;

    std::vector<Engine::Graphics::RotatingSprite*> renderList;

    Engine::Graphics::RotatingSprite backgroundProp;
    Engine::Graphics::RotatingSprite playerSprite;
    Engine::Graphics::RotatingSprite floatingEffect;

    int playerHp;
    int score;

    void ResetFields();

public:
    GameplayScene();

    void Init() override;
    void Update() override;
    void Draw() override;
    void Unload() override;

    std::string GetNextScene() const override;
};
