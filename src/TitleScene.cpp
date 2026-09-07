#include <cassert>
#include "TitleScene.hpp"

TitleScene::TitleScene() {
}

void TitleScene::Init() {
    nextScene_ = "";
    clickManager_.Clear();

    dragBox_ = { 100, 100, 120, 80 };
    isDragging_ = false;

    // 1. 버튼 등록
    clickManager_.AddRegion(
        "start_button", raylib::Rectangle{ 300, 180, 200, 50 },
        [this]() { this->nextScene_ = "Gameplay"; }
    );

    clickManager_.AddRegion(
        "vol_down", raylib::Rectangle{ 300, 250, 40, 40 },
        [this]() {
            if (soundVolume_ > 0.1f) {
                soundVolume_ -= 0.1f;
                this->bgm_.SetVolume(soundVolume_);
                this->sounds_["click"].Play();
            }
        }
    );

    clickManager_.AddRegion(
        "vol_up", raylib::Rectangle{ 460, 250, 40, 40 },
        [this]() {
            if (soundVolume_ < 1.0f) {
                soundVolume_ += 0.1f;
                this->bgm_.SetVolume(soundVolume_);
                this->sounds_["click"].Play();
            }
        }
    );

    // 2. BGM 로드 (ResourceManager 경유)
    bgm_ = Engine::Resource::ResourceManager::Instance().LoadMusic("background.mp3");
    soundVolume_ = 0.5f;
    bgm_.SetVolume(soundVolume_);
    bgm_.Play();

    // 3. SFX 로드 (ResourceManager 경유)
    sounds_.insert_or_assign(
        "click",
        Engine::Resource::ResourceManager::Instance().LoadSound("click.wav")
    );

    for (auto& snd : sounds_) {
        snd.second.SetVolume(1.0f);
    }
}

void TitleScene::Update() {
    bgm_.Update();

    auto& display = Engine::Display::Display::Instance();
    clickManager_.Update();

    raylib::Vector2 mousePos = display.GetVirtualMousePosition();

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
    raylib::DrawText("MAIN MENU", 320, 100, 32, raylib::Color::DarkBlue());

    raylib::Color boxColor = isDragging_ ? raylib::Color::Orange() : raylib::Color::Purple();
    dragBox_.Draw(boxColor);
    raylib::DrawText("Drag Me!", (int)dragBox_.x + 20, (int)dragBox_.y + 30, 20, raylib::Color::White());

    raylib::Rectangle(300, 180, 200, 50).Draw(raylib::Color::SkyBlue());
    raylib::DrawText("START GAME", 335, 195, 20, raylib::Color::DarkBlue());

    raylib::Rectangle(300, 250, 40, 40).Draw(raylib::Color::LightGray());
    raylib::DrawText("-", 315, 258, 24, raylib::Color::Black());

    auto volume_percent = static_cast<int>(soundVolume_ * 100);
    raylib::DrawText(TextFormat("Volume: %d%%", volume_percent), 355, 260, 15, raylib::Color::DarkGray());

    raylib::Rectangle(460, 250, 40, 40).Draw(raylib::Color::LightGray());
    raylib::DrawText("+", 472, 258, 24, raylib::Color::Black());

    clickManager_.Draw();
    clickManager_.DrawBoundary();
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
