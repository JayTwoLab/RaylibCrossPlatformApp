#include <cassert>
#include "GameOverScene.hpp"

GameOverScene::GameOverScene() {
    nextScene_ = "";
}

void GameOverScene::Init() {
    nextScene_ = "";
    clickManager_.Clear();

    {
        // 재시도 버튼
        auto retry_button_name = "retry_button";
        clickManager_.AddRegion(
            retry_button_name, raylib::Rectangle{ 480, 360, 140, 50 },
            [this]() { this->nextScene_ = "Gameplay"; }
        );

        auto sprite = std::make_shared<Engine::Graphics::RotatingSprite>();
        sprite->RegisterClip("backchar", "back_char.png");
        sprite->SetClipState("backchar");
        sprite->SetRotationSpeed(180.0f);
        clickManager_.SetRegionSprite(retry_button_name, sprite);
    }

    {
        // 타이틀 버튼
        auto title_button_name = "title_button";
        clickManager_.AddRegion(
            title_button_name, raylib::Rectangle{ 660, 360, 140, 50 },
            [this]() { this->nextScene_ = "Title"; }
        );

        clickManager_.SetRegionText(
            title_button_name,
            "To Title",
            raylib::Color::White(),
            raylib::Color::DarkGray(),
            22
        );
    }

}

void GameOverScene::Update() {
    clickManager_.Update();
}

void GameOverScene::Draw() {
    raylib::Color::Maroon().ClearBackground();

    {
        raylib::DrawText("GAME OVER", 500, 240, 50, raylib::Color::White());
    }

    {
        clickManager_.Draw();
        clickManager_.DrawBoundary();
    }
}

void GameOverScene::Unload() {
    clickManager_.Clear();
}

std::string GameOverScene::GetNextScene() {
    return nextScene_;
}
