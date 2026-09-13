#include <cassert>
#include "TitleScene.hpp"

TitleScene::TitleScene() {
}

void TitleScene::Init() {
    namespace ED = Engine::Display;
    namespace ES = Engine::Scene;
    namespace EU = Engine::UI;
    namespace ER = Engine::Resource;

    auto& resourceManager = ER::ResourceManager::Instance();

    nextScene_ = "";
    clickManager_.Clear();

    dragBox_ = { 150, 280, 160, 100 };
    isDragging_ = false;

    std::filesystem::path koreanFontDiskPath = resourceManager.GetResourcePath() / "NanumGothicBold.ttf";

    {
        // 씬의 글꼴 설정
        auto sceneFontSize = 32;
        SetSceneFont(koreanFontDiskPath, sceneFontSize, ES::Scene::GetKoreanCodePoints());
    }

    {
        // 클릭 가능한 영역의 기본 글꼴 설정
        auto clickFontSize = 20;
        clickManager_.SetDefaultFont(koreanFontDiskPath, clickFontSize, EU::ClickableAreaManager::GetKoreanCodePoints());
    }

    {
        {
            // 게임 시작 버튼
            auto start_button_name = "start_button";
            clickManager_.AddRegion(
                start_button_name, raylib::Rectangle{ 540, 320, 200, 60 },
                [this]() { this->nextScene_ = "Gameplay"; }
            );

            auto buttonSprite = std::make_shared<Engine::Graphics::RotatingSprite>();
            buttonSprite->RegisterClip("idle", "button_normal.png");
            buttonSprite->RegisterClip("pressed", "button_clicked.png");
            buttonSprite->SetClipState("idle");
            clickManager_.SetRegionSprite(start_button_name, buttonSprite);
        }

        {
            // 볼륨 조절 버튼
            auto vol_down_name = "vol_down";
            clickManager_.AddRegion(
                vol_down_name, raylib::Rectangle{ 510, 430, 50, 50 },
                [this]() {
                    if (soundVolume_ > 0.05f) {
                        soundVolume_ -= 0.1f;
                        if (soundVolume_ < 0.0f) soundVolume_ = 0.0f;
                        this->bgm_.SetVolume(soundVolume_);
                        this->sounds_["click"].Play();
                    }
                }
            );

            auto fontSize = 28;
            auto fontColor = raylib::Color::Black();
            auto backgroundColor = raylib::Color::LightGray();
            clickManager_.SetRegionText(vol_down_name, "음", fontColor, backgroundColor, fontSize);
        }

        {
            // 볼륨 증가 버튼
            auto vol_up_name = "vol_up";
            clickManager_.AddRegion(
                vol_up_name, raylib::Rectangle{ 720, 430, 50, 50 },
                [this]() {
                    if (soundVolume_ < 1.0f) {
                        soundVolume_ += 0.1f;
                        if (soundVolume_ > 1.0f) soundVolume_ = 1.0f;
                        this->bgm_.SetVolume(soundVolume_);
                        this->sounds_["click"].Play();
                    }
                }
            );

            auto fontSize = 28;
            auto fontColor = raylib::Color::Black();
            auto backgroundColor = raylib::Color::LightGray();
            clickManager_.SetRegionText(vol_up_name, "양", fontColor, backgroundColor, fontSize);
        }
    }

    {
        bgm_ = resourceManager.LoadMusic("background.mp3");
        soundVolume_ = 0.5f;
        bgm_.SetVolume(soundVolume_);
        bgm_.Play();

        sounds_.insert_or_assign("click", resourceManager.LoadSound("click.wav"));
        for (auto& snd : sounds_) {
            snd.second.SetVolume(1.0f);
        }
    }

}

void TitleScene::Update() {
    bgm_.Update();
    clickManager_.Update();

    // ImGui가 마우스를 잡고 있지 않을 때만 드래그 상자 인터랙션
    if (Engine::UI::WantCaptureMouse()) {
        isDragging_ = false;
        return; 
    }

    raylib::Vector2 mousePos = Engine::Display::Display::Instance().GetVirtualMousePosition();

    if (raylib::Mouse::IsButtonPressed(MOUSE_BUTTON_LEFT)) {
        if (dragBox_.CheckCollision(mousePos)) {
            isDragging_ = true;
            dragOffset_.x = mousePos.x - dragBox_.x;
            dragOffset_.y = mousePos.y - dragBox_.y;
        }
        lastClickedPos_ = mousePos;
    }

    if (isDragging_ && raylib::Mouse::IsButtonDown(MOUSE_BUTTON_LEFT)) {
        dragBox_.x = mousePos.x - dragOffset_.x;
        dragBox_.y = mousePos.y - dragOffset_.y;
    }

    if (raylib::Mouse::IsButtonReleased(MOUSE_BUTTON_LEFT)) {
        isDragging_ = false;
    }
 
}

void TitleScene::Draw() {
    raylib::Color::RayWhite().ClearBackground();

    {
        // 씬에 설정된 폰트가 있는 경우 해당 폰트로 렌더링
        auto mainTitleFontSize = 48; // 원하는 폰트 크기
        auto mainTitleX = 520.0f; // 원하는 X 좌표
        auto mainTitleY = 180.0f; // 원하는 Y 좌표
        auto mainTitlePosition = raylib::Vector2{ mainTitleX, mainTitleY }; // 원하는 위치
        auto koreanText = "메인 메뉴"; // 한글 텍스트
        auto engText = "MAIN MENU"; // 영어 텍스트
        auto mainTitleColor = raylib::Color::DarkBlue(); // 원하는 색상

        if (sceneFont_ && sceneFont_->texture.id > 0) {
            auto spacing = 1.0f; // 자간 설정 (필요에 따라 조정)
            sceneFont_->DrawText(koreanText, mainTitlePosition, mainTitleFontSize, spacing, mainTitleColor);
        }
        else {
            // 폰트가 없을 경우 기본 렌더링 방식 Fallback
            raylib::DrawText(engText, mainTitleX, mainTitleY, mainTitleFontSize, mainTitleColor);
        }
    }

    {
        // 드래그 상자 그리기
        raylib::Color boxColor = isDragging_ ? raylib::Color::Orange() : raylib::Color::Purple();
        dragBox_.Draw(boxColor);

        auto newX = static_cast<int>(dragBox_.x) + 30;
        auto newY = static_cast<int>(dragBox_.y) + 38;
        auto fontSize = 22;
        auto fontColor = raylib::Color::White();
        raylib::DrawText("Drag Me!", newX, newY, fontSize, fontColor);
    }

    {
        // 볼륨 표시
        auto volume_percent = static_cast<int>(soundVolume_ * 100.0f + 0.5f);
        raylib::DrawText(TextFormat("Volume: %d%%", volume_percent), 580, 442, 20, raylib::Color::DarkGray());
    }

    {
        // 클릭 가능한 영역 그리기
        clickManager_.Draw();
        clickManager_.DrawBoundary();
    }
}

void TitleScene::DrawImGui() {
    ImGui::Begin("Title Debugger 한글");
    {
        ImGui::Text("Set Background Music Volume 한글");

        if (ImGui::SliderFloat("Volume 볼륨", &soundVolume_, 0.0f, 1.0f, "%.2f")) {
            bgm_.SetVolume(soundVolume_);
        }

        if (ImGui::Button("Start Game 게임")) {
            nextScene_ = "Gameplay";
        }
    }
    ImGui::End();
}

void TitleScene::Unload() {
    bgm_.Stop();
    bgm_.Unload();
    for (auto& snd : sounds_) {
        snd.second.Stop();
        snd.second.Unload();
    }
    sounds_.clear();
    clickManager_.Clear();
}

std::string TitleScene::GetNextScene() {
    return nextScene_;
}
