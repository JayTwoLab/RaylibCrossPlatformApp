#include <cassert>
#include "TitleScene.hpp"

TitleScene::TitleScene() {
}

void TitleScene::Init() {
    namespace ER = Engine::Resource;
    auto& resourceManager = ER::ResourceManager::Instance();

    nextScene_ = "";
    clickManager_.Clear();

    dragBox_ = { 150, 280, 160, 100 };
    isDragging_ = false;

    // 게임 시작 버튼
    clickManager_.AddRegion(
        "start_button", raylib::Rectangle{ 540, 320, 200, 60 },
        [this]() { this->nextScene_ = "Gameplay"; }
    );

    auto buttonSprite = std::make_shared<Engine::Graphics::RotatingSprite>();
    buttonSprite->RegisterClip("idle", "button_normal.png");
    buttonSprite->RegisterClip("pressed", "button_clicked.png");
    buttonSprite->SetClipState("idle");
    clickManager_.SetRegionSprite("start_button", buttonSprite);

    // 볼륨 조절 버튼
    clickManager_.AddRegion(
        "vol_down", raylib::Rectangle{ 510, 430, 50, 50 },
        [this]() {
            if (soundVolume_ > 0.05f) {
                soundVolume_ -= 0.1f;
                if (soundVolume_ < 0.0f) soundVolume_ = 0.0f;
                this->bgm_.SetVolume(soundVolume_);
                this->sounds_["click"].Play();
            }
        }
    );
    clickManager_.SetRegionText("vol_down", "-", raylib::Color::Black(), raylib::Color::LightGray(), 28);

    clickManager_.AddRegion(
        "vol_up", raylib::Rectangle{ 720, 430, 50, 50 },
        [this]() {
            if (soundVolume_ < 1.0f) {
                soundVolume_ += 0.1f;
                if (soundVolume_ > 1.0f) soundVolume_ = 1.0f;
                this->bgm_.SetVolume(soundVolume_);
                this->sounds_["click"].Play();
            }
        }
    );
    clickManager_.SetRegionText("vol_up", "+", raylib::Color::Black(), raylib::Color::LightGray(), 28);

    bgm_ = resourceManager.LoadMusic("background.mp3");
    soundVolume_ = 0.5f;
    bgm_.SetVolume(soundVolume_);
    bgm_.Play();

    sounds_.insert_or_assign("click", resourceManager.LoadSound("click.wav"));
    for (auto& snd : sounds_) {
        snd.second.SetVolume(1.0f);
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
    raylib::DrawText("MAIN MENU", 520, 180, 48, raylib::Color::DarkBlue());

    raylib::Color boxColor = isDragging_ ? raylib::Color::Orange() : raylib::Color::Purple();
    dragBox_.Draw(boxColor);
    raylib::DrawText("Drag Me!", static_cast<int>(dragBox_.x) + 30, static_cast<int>(dragBox_.y) + 38, 22, raylib::Color::White());

    auto volume_percent = static_cast<int>(soundVolume_ * 100.0f + 0.5f);
    raylib::DrawText(TextFormat("Volume: %d%%", volume_percent), 580, 442, 20, raylib::Color::DarkGray());

    clickManager_.Draw();
    clickManager_.DrawBoundary();
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
