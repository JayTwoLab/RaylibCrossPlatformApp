#pragma once

#include <string>
#include "Engine/Engine.hpp"

class TitleScene : public Engine::Scene::Scene {
private:
    Engine::UI::ClickableAreaManager clickManager_; // 클릭 관리 객체

    raylib::Music bgm_;      // 씬이 직접 소유
    raylib::Sound clickSfx_; // 씬이 직접 소유
    float soundVolume_ = 0.5;
    
    raylib::Vector2 lastClickedPos_ = { -1.0f, -1.0f }; // 클릭 좌표 저장 변수

    // 드래그 관련 변수
    raylib::Rectangle dragBox_ = { 100, 100, 120, 80 }; // 드래그할 박스
    bool isDragging_ = false;                           // 드래그 상태 플래그
    raylib::Vector2 dragOffset_ = { 0.0f, 0.0f };       // 마우스 클릭 오프셋

public:
    TitleScene(); 

    void Init() override;
    void Update() override;
    void Draw() override;
    void Unload() override;

    std::string GetNextScene() override;
};
