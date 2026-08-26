#pragma once

#include <string>

#include "Engine/Engine.hpp"

class StageClearScene : public Engine::Scene::Scene {
private:
    Engine::UI::ClickableAreaManager clickManager_;

public:
    StageClearScene();
     
    void Init() override;
    void Update() override;
    void Draw() override;
    void Unload() override;

    std::string GetNextScene() override;
};
