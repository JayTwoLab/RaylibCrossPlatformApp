#include "TitleScene.hpp"
#include <cassert>

TitleScene::TitleScene() {
}

void TitleScene::Init() {
    namespace ED = Engine::Display;
    namespace ER = Engine::Resource;

    using EDD = Engine::Display::Display;
    auto& display = EDD::Instance();

    using ERM = ER::ResourceManager;
    auto& resourceManager = ERM::Instance();
    auto resPath = resourceManager.GetResourcePath();

    nextScene_ = "";
    clickManager_.Clear();

    // 드래그 박스 초기화
    dragBox_ = { 100, 100, 120, 80 };
    isDragging_ = false;

    // 버튼 클릭 영역 등록
    clickManager_.AddRegion("start_button", raylib::Rectangle{ 300, 180, 200, 50 }, [this]() {
        this->nextScene_ = "Gameplay";
    });

    clickManager_.AddRegion(
        "vol_down", // 이름 
        raylib::Rectangle{ 300, 250, 40, 40 }, // 영역 (x, y, width, height)
        [this]() { // 클릭 시 실행할 람다 함수
            if (soundVolume_ > 0.1f) {
                soundVolume_ -= 0.1f;
                this->bgm_.SetVolume(soundVolume_);
                this->sounds_["click"].Play();
            }
        }
    );

    clickManager_.AddRegion(
        "vol_up",
        raylib::Rectangle{ 460, 250, 40, 40 },
        [this]() {
            if (soundVolume_ < 1.0f) {
                soundVolume_ += 0.1f;
                this->bgm_.SetVolume(soundVolume_);
                this->sounds_["click"].Play();
            }
        }
    );

    // Background Music Load and Play
    auto bgmFileName = "background.mp3";
    bgm_.Load((resPath / bgmFileName).string());
    soundVolume_ = 0.5f; // 초기 볼륨 설정
    bgm_.SetVolume(soundVolume_);
    bgm_.Play();

    // Sound Effects Load
    sounds_.try_emplace("click", (resPath / "click.wav").string());
    // sounds_["click"].Play(); // 테스트용: 효과음 재생

    // sounds_.try_emplace("name", (resPath / "file.mp3").string());

    for (auto& snd : sounds_ ) {
        snd.second.SetVolume(1.0f);
    }

}

void TitleScene::Update() {

    // 배경음악 업데이트
    bgm_.Update(); // 누락 시 처음 몇 밀리초만 재생되거나 소리가 안 남

    namespace ED = Engine::Display;
    using EDD = Engine::Display::Display;
    auto& display = EDD::Instance();

    clickManager_.Update(); // UI 클릭 영역 업데이트 (마우스 좌표를 가상 좌표로 변환하여 처리)

    //-----------------------------
    // 멀티 터치 처리 기본 패턴 (안드로이드 등)
    // int touchCount = GetTouchPointCount();
    // if (touchCount >= 2) {
    //     // 두 손가락 이상 터치 시, 드래그 박스 초기화
	// }

    // 마우스 위치를 가상 좌표로 변환    
    raylib::Vector2 mousePos = display.GetVirtualMousePosition();

    // 좌 클릭
    if (raylib::Mouse::IsButtonPressed(MOUSE_BUTTON_LEFT)) {

        // 드래그 박스 안에서 클릭했는지 확인
        if (dragBox_.CheckCollision(mousePos)) {
            isDragging_ = true;
            dragOffset_.x = mousePos.x - dragBox_.x;
            dragOffset_.y = mousePos.y - dragBox_.y;
        }

        lastClickedPos_ = mousePos; // 마지막 클릭 좌표 저장

        // 콘솔 출력 (디버깅용)
        bool nonVirtualArea = false;
        if (mousePos.x <= 0 || mousePos.y <= 0) {
            nonVirtualArea = true;
        }
        using EDD = Engine::Display::Display;
        auto& display = EDD::Instance();
        const int virtualScreenWidth = display.VirtualWidth;
        const int virtualScreenHeight = display.VirtualHeight;
        if (mousePos.x >= virtualScreenWidth || mousePos.y >= virtualScreenHeight) {
            nonVirtualArea = true;
        }
        if (nonVirtualArea) {
            TraceLog(LOG_INFO, "L-Clicked Position: [Non-Virtual Area]");
        }
        else {
            TraceLog(LOG_INFO, "L-Clicked Position: (%.1f, %.1f)", mousePos.x, mousePos.y);
        }
    }

    // (드래그 박스의) 좌 클릭 유지 중
    if (isDragging_ && raylib::Mouse::IsButtonDown(MOUSE_BUTTON_LEFT)) {
        dragBox_.x = mousePos.x - dragOffset_.x;
        dragBox_.y = mousePos.y - dragOffset_.y;

        // 콘솔 출력 (디버깅용)
        TraceLog(LOG_INFO, "DragBox Position: (%.1f, %.1f)", dragBox_.x, dragBox_.y);
    }

    // 좌 클릭 해제
    if (raylib::Mouse::IsButtonReleased(MOUSE_BUTTON_LEFT)) {
        isDragging_ = false;

        // 콘솔 출력 (디버깅용)
        bool nonVirtualArea = false;
        if (mousePos.x <= 0 || mousePos.y <= 0) {
            nonVirtualArea = true;
        }
        using EDD = Engine::Display::Display;
        auto& display = EDD::Instance();
        const int virtualScreenWidth = display.VirtualWidth;
        const int virtualScreenHeight = display.VirtualHeight;
        if (mousePos.x >= virtualScreenWidth || mousePos.y >= virtualScreenHeight) {
            nonVirtualArea = true;
        }
        if (nonVirtualArea) {
            TraceLog(LOG_INFO, "L-Released Position: [Non-Virtual Area]");
        }
        else {
            TraceLog(LOG_INFO, "L-Released Position: (%.1f, %.1f)", mousePos.x, mousePos.y);
        }
    }

    // 우 클릭 시 (안드로이드 터치 스크린에서는 우클릭 없음)   
    if (raylib::Mouse::IsButtonPressed(MOUSE_BUTTON_RIGHT)) {
        // 콘솔 출력 (디버깅용)
        bool nonVirtualArea = false;
        if (mousePos.x <= 0 || mousePos.y <= 0) {
            nonVirtualArea = true;
        }
        using EDD = Engine::Display::Display;
        auto& display = EDD::Instance();
        const int virtualScreenWidth = display.VirtualWidth;
        const int virtualScreenHeight = display.VirtualHeight;
        if (mousePos.x >= virtualScreenWidth || mousePos.y >= virtualScreenHeight) {
            nonVirtualArea = true;
        }
        if (nonVirtualArea) {
            TraceLog(LOG_INFO, "R-Clicked Position: [Non-Virtual Area]");
        }
        else {
            TraceLog(LOG_INFO, "R-Clicked Position: (%.1f, %.1f)", mousePos.x, mousePos.y);
        }
    }

	// 우 클릭 해제 시 (안드로이드 터치 스크린에서는 우클릭 없음) 
    if (raylib::Mouse::IsButtonReleased(MOUSE_BUTTON_RIGHT)) {
        // 콘솔 출력 (디버깅용)
        bool nonVirtualArea = false;
        if (mousePos.x <= 0 || mousePos.y <= 0) {
            nonVirtualArea = true;
        }
        using EDD = Engine::Display::Display;
        auto& display = EDD::Instance();
        const int virtualScreenWidth = display.VirtualWidth;
        const int virtualScreenHeight = display.VirtualHeight;
        if (mousePos.x >= virtualScreenWidth || mousePos.y >= virtualScreenHeight) {
            nonVirtualArea = true;
        }
        if (nonVirtualArea) {
            TraceLog(LOG_INFO, "R-Released Position: [Non-Virtual Area]");
        }
        else {
            TraceLog(LOG_INFO, "R-Released Position: (%.1f, %.1f)", mousePos.x, mousePos.y);
        }
    }

}

void TitleScene::Draw() {
    raylib::Color::RayWhite().ClearBackground();
    raylib::DrawText("MAIN MENU", 320, 100, 32, raylib::Color::DarkBlue());

    // 드래그 박스 렌더링
    raylib::Color boxColor = isDragging_ ? raylib::Color::Orange() : raylib::Color::Purple();
    dragBox_.Draw(boxColor);
    raylib::DrawText("Drag Me!", (int)dragBox_.x + 20, (int)dragBox_.y + 30, 20, raylib::Color::White());

    // 버튼 렌더링
    raylib::Rectangle(300, 180, 200, 50).Draw(raylib::Color::SkyBlue());
    raylib::DrawText("START GAME", 335, 195, 20, raylib::Color::DarkBlue());

    raylib::Rectangle(300, 250, 40, 40).Draw(raylib::Color::LightGray());
    raylib::DrawText("-", 315, 258, 24, raylib::Color::Black());

    auto volume_percent = static_cast<int>(soundVolume_ * 100);
    raylib::DrawText(TextFormat("Volume: %d%%", volume_percent), 355, 260, 15, raylib::Color::DarkGray());

    raylib::Rectangle(460, 250, 40, 40).Draw(raylib::Color::LightGray());
    raylib::DrawText("+", 472, 258, 24, raylib::Color::Black());

    clickManager_.DrawDebug();
}

void TitleScene::Unload() {
    bgm_.Stop();
    bgm_.Unload();
    for (auto& snd : sounds_) {
        snd.second.Stop();
        snd.second.Unload();
    }
    clickManager_.Clear();
}

std::string TitleScene::GetNextScene() {
    return nextScene_;
}
