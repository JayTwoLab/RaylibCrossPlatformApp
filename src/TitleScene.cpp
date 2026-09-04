#include <cassert>
#include "TitleScene.hpp"

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

    // Initialize drag box
    dragBox_ = { 100, 100, 120, 80 };
    isDragging_ = false;

    // Register button click regions
    clickManager_.AddRegion(
        "start_button", raylib::Rectangle{ 300, 180, 200, 50 },
        [this]() { this->nextScene_ = "Gameplay"; });

    clickManager_.AddRegion(
        "vol_down", // name
        raylib::Rectangle{ 300, 250, 40, 40 }, // region (x, y, width, height)
        [this]() { // lambda executed on click
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
    soundVolume_ = 0.5f; // initial volume setting
    bgm_.SetVolume(soundVolume_);
    bgm_.Play();

    // Sound Effects Load
    sounds_.try_emplace("click", (resPath / "click.wav").string());
    // sounds_["click"].Play(); // test: play effect sound

    // sounds_.try_emplace("name", (resPath / "file.mp3").string());

    for (auto& snd : sounds_ ) {
        snd.second.SetVolume(1.0f);
    }

}

void TitleScene::Update() {

    // Update background music
    // If omitted, it may play only the first few milliseconds or produce no sound
    bgm_.Update();

    namespace ED = Engine::Display;
    using EDD = Engine::Display::Display;
    auto& display = EDD::Instance();

    clickManager_.Update(); // Update UI click regions (converts mouse coords to virtual coords)

    //-----------------------------
    // Multi-touch handling basic pattern (Android, etc.)
    // int touchCount = GetTouchPointCount();
    // if (touchCount >= 2) {
    //     // Reset drag box when two or more fingers touch
    // }

    // Convert mouse position to virtual coordinates
    raylib::Vector2 mousePos = display.GetVirtualMousePosition();

    // Left click
    if (raylib::Mouse::IsButtonPressed(MOUSE_BUTTON_LEFT)) {

        // Check if clicked inside drag box
        if (dragBox_.CheckCollision(mousePos)) {
            isDragging_ = true;
            dragOffset_.x = mousePos.x - dragBox_.x;
            dragOffset_.y = mousePos.y - dragBox_.y;
        }

        lastClickedPos_ = mousePos; // Store last clicked position

        // Console output (for debugging)
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

    // While left click held on (the drag box)
    if (isDragging_ && raylib::Mouse::IsButtonDown(MOUSE_BUTTON_LEFT)) {
        dragBox_.x = mousePos.x - dragOffset_.x;
        dragBox_.y = mousePos.y - dragOffset_.y;

        // Console output (for debugging)
        TraceLog(LOG_INFO, "DragBox Position: (%.1f, %.1f)", dragBox_.x, dragBox_.y);
    }

    // Left click released
    if (raylib::Mouse::IsButtonReleased(MOUSE_BUTTON_LEFT)) {
        isDragging_ = false;

        // Console output (for debugging)
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

    // Right click (no right-click on Android touch screens)
    if (raylib::Mouse::IsButtonPressed(MOUSE_BUTTON_RIGHT)) {
        // Console output (for debugging)
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

    // Right-click release (no right-click on Android touch screens)
    if (raylib::Mouse::IsButtonReleased(MOUSE_BUTTON_RIGHT)) {
        // Console output (for debugging)
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

    // Right arrow key pressed (for testing)
    if (raylib::Keyboard::IsKeyPressed(KEY_RIGHT)) {
        TraceLog(LOG_INFO, "[TitleScene] Right Arrow Key is down.");
    }
}

void TitleScene::Draw() {
    raylib::Color::RayWhite().ClearBackground();
    raylib::DrawText("MAIN MENU", 320, 100, 32, raylib::Color::DarkBlue());

    // Render drag box
    raylib::Color boxColor = isDragging_ ? raylib::Color::Orange() : raylib::Color::Purple();
    dragBox_.Draw(boxColor);
    raylib::DrawText("Drag Me!", (int)dragBox_.x + 20, (int)dragBox_.y + 30, 20, raylib::Color::White());

    // Button rendering
    raylib::Rectangle(300, 180, 200, 50).Draw(raylib::Color::SkyBlue());
    raylib::DrawText("START GAME", 335, 195, 20, raylib::Color::DarkBlue());

    raylib::Rectangle(300, 250, 40, 40).Draw(raylib::Color::LightGray());
    raylib::DrawText("-", 315, 258, 24, raylib::Color::Black());

    auto volume_percent = static_cast<int>(soundVolume_ * 100);
    raylib::DrawText(TextFormat("Volume: %d%%", volume_percent), 355, 260, 15, raylib::Color::DarkGray());

    raylib::Rectangle(460, 250, 40, 40).Draw(raylib::Color::LightGray());
    raylib::DrawText("+", 472, 258, 24, raylib::Color::Black());

    clickManager_.Draw();
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
