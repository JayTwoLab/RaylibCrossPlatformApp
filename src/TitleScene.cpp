#include "TitleScene.hpp"
#include <cassert>

TitleScene::TitleScene() {
}

void TitleScene::Init() {
    namespace ED = Engine::Display;
    using EDD = Engine::Display::Display;
    auto& display = EDD::Instance();

    const int virtualScreenWidth = display.VirtualWidth;
    assert(virtualScreenWidth >= 800);
    const int virtualScreenHeight = display.VirtualHeight;
    assert(virtualScreenHeight >= 600);

    nextScene = "";
    clickManager.Clear();

    // 드래그 박스 초기화
    dragBox = { 100, 100, 120, 80 }; 
    isDragging = false;

    clickManager.AddRegion("start_button", raylib::Rectangle{ 300, 180, 200, 50 }, [this]() {
        this->nextScene = "Gameplay";
        });

    clickManager.AddRegion("vol_down", raylib::Rectangle{ 300, 250, 40, 40 }, [this]() {
        if (soundVolume > 0) soundVolume -= 10;
        });

    clickManager.AddRegion("vol_up", raylib::Rectangle{ 460, 250, 40, 40 }, [this]() {
        if (soundVolume < 100) soundVolume += 10;
        });
}

void TitleScene::Update() {
    clickManager.Update();

    //-----------------------------

    // 멀티 터치 처리 기본 패턴
    // int touchCount = GetTouchPointCount();
    // if (touchCount >= 2) {
    //     // 두 손가락 이상 터치 시, 드래그 박스 초기화
	// }

    namespace ED = Engine::Display;
    using EDD = Engine::Display::Display;
    auto& display = EDD::Instance();

    raylib::Vector2 mousePos = display.GetVirtualMousePosition();

    // 1. 드래그 시작 (좌클릭을 누른 순간)
    if (raylib::Mouse::IsButtonPressed(MOUSE_BUTTON_LEFT)) {
        if (dragBox.CheckCollision(mousePos)) {
            isDragging = true;
            dragOffset.x = mousePos.x - dragBox.x;
            dragOffset.y = mousePos.y - dragBox.y;
        }

        lastClickedPos = mousePos;
        // hasClicked = true;

        // 콘솔 출력 (디버깅용)
        TraceLog(LOG_INFO, "L-Clicked Position: (%.1f, %.1f)", mousePos.x, mousePos.y);
    }

    // 2. 드래그 중 (좌클릭 유지 중)
    if (isDragging && raylib::Mouse::IsButtonDown(MOUSE_BUTTON_LEFT)) {
        dragBox.x = mousePos.x - dragOffset.x;
        dragBox.y = mousePos.y - dragOffset.y;

        // 콘솔 출력 (디버깅용)
        TraceLog(LOG_INFO, "DragBox Position: (%.1f, %.1f)", dragBox.x, dragBox.y);
    }

    // 3. 드래그 종료 (좌클릭 해제)
    if (raylib::Mouse::IsButtonReleased(MOUSE_BUTTON_LEFT)) {
        isDragging = false;

        // 콘솔 출력 (디버깅용)
        TraceLog(LOG_INFO, "L-Released Position: (%.1f, %.1f)", mousePos.x, mousePos.y);
    }

    // 우클릭 시 (안드로이드 터치 스크린에서는 우클릭 없음)   
    if (raylib::Mouse::IsButtonPressed(MOUSE_BUTTON_RIGHT)) {
        // hasClicked = false;

        // 콘솔 출력 (디버깅용)
        TraceLog(LOG_INFO, "R-Clicked Position: (%.1f, %.1f)", mousePos.x, mousePos.y);
    }

	// 우클릭 해제 시
    if (raylib::Mouse::IsButtonReleased(MOUSE_BUTTON_RIGHT)) {
        // 콘솔 출력 (디버깅용)
        TraceLog(LOG_INFO, "R-Released Position: (%.1f, %.1f)", mousePos.x, mousePos.y);
    }
}

void TitleScene::Draw() {
    raylib::Color::RayWhite().ClearBackground();
    raylib::DrawText("MAIN MENU", 320, 100, 32, raylib::Color::DarkBlue());

    // 드래그 박스 렌더링
    raylib::Color boxColor = isDragging ? raylib::Color::Orange() : raylib::Color::Purple();
    dragBox.Draw(boxColor);
    raylib::DrawText("Drag Me!", (int)dragBox.x + 20, (int)dragBox.y + 30, 20, raylib::Color::White());

    // 버튼 렌더링
    raylib::Rectangle(300, 180, 200, 50).Draw(raylib::Color::SkyBlue());
    raylib::DrawText("START GAME", 335, 195, 20, raylib::Color::DarkBlue());

    raylib::Rectangle(300, 250, 40, 40).Draw(raylib::Color::LightGray());
    raylib::DrawText("-", 315, 258, 24, raylib::Color::Black());

    raylib::DrawText(TextFormat("Volume: %d", soundVolume), 355, 260, 20, raylib::Color::DarkGray());

    raylib::Rectangle(460, 250, 40, 40).Draw(raylib::Color::LightGray());
    raylib::DrawText("+", 472, 258, 24, raylib::Color::Black());

    clickManager.DrawDebug();
}

void TitleScene::Unload() {
    clickManager.Clear();
}

std::string TitleScene::GetNextScene() const {
    return nextScene;
}
