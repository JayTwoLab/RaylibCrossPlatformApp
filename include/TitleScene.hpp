#pragma once

#include <string>
#include "Engine/Engine.hpp"

class TitleScene : public Engine::Scene::Scene {
private:
    std::string nextScene = "";
    Engine::UI::ClickableAreaManager clickManager;
    int soundVolume = 50;

    // 클릭 좌표 저장 변수
    raylib::Vector2 lastClickedPos = { -1.0f, -1.0f };
    // bool hasClicked = false;

    // 드래그 관련 변수
    raylib::Rectangle dragBox = { 100, 100, 120, 80 }; // 드래그할 박스
    bool isDragging = false;                           // 드래그 상태 플래그
    raylib::Vector2 dragOffset = { 0.0f, 0.0f };       // 마우스 클릭 오프셋

public:
    TitleScene();

    void Init() override;
    void Update() override;
    void Draw() override;
    void Unload() override;

    std::string GetNextScene() const override;
};