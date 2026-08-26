#include "GameOverScene.hpp"

GameOverScene::GameOverScene() {
    nextScene_ = "";
}

void GameOverScene::Init() {
    nextScene_ = "";
    clickManager_.Clear();

    clickManager_.AddRegion("retry_button", raylib::Rectangle{ 250, 250, 130, 50 }, [this]() {
        this->nextScene_ = "Gameplay";
        });

    clickManager_.AddRegion("title_button", raylib::Rectangle{ 420, 250, 130, 50 }, [this]() {
        this->nextScene_ = "Title";
        });
}

void GameOverScene::Update() { 
    clickManager_.Update();
}

void GameOverScene::Draw() {
    raylib::Color::Maroon().ClearBackground();
    raylib::DrawText("GAME OVER", 280, 150, 40, raylib::Color::White());

    raylib::Rectangle(250, 250, 130, 50).Draw(raylib::Color::DarkGray());
    raylib::DrawText("RETRY", 285, 265, 20, raylib::Color::White());

    raylib::Rectangle(420, 250, 130, 50).Draw(raylib::Color::DarkGray());
    raylib::DrawText("TITLE", 460, 265, 20, raylib::Color::White());

    clickManager_.DrawDebug();
}

void GameOverScene::Unload() {
    clickManager_.Clear();
}

std::string GameOverScene::GetNextScene() {
    return nextScene_;
}
