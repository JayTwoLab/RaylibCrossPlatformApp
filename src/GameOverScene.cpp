#include "GameOverScene.hpp"

GameOverScene::GameOverScene() : nextScene("") {}

void GameOverScene::Init() {
    nextScene = "";
    clickManager.Clear();

    clickManager.AddRegion("retry_button", raylib::Rectangle{ 250, 250, 130, 50 }, [this]() {
        this->nextScene = "Gameplay";
        });

    clickManager.AddRegion("title_button", raylib::Rectangle{ 420, 250, 130, 50 }, [this]() {
        this->nextScene = "Title";
        });
}

void GameOverScene::Update() {
    clickManager.Update();
}

void GameOverScene::Draw() {
    raylib::Color::Maroon().ClearBackground();
    raylib::DrawText("GAME OVER", 280, 150, 40, raylib::Color::White());

    raylib::Rectangle(250, 250, 130, 50).Draw(raylib::Color::DarkGray());
    raylib::DrawText("RETRY", 285, 265, 20, raylib::Color::White());

    raylib::Rectangle(420, 250, 130, 50).Draw(raylib::Color::DarkGray());
    raylib::DrawText("TITLE", 460, 265, 20, raylib::Color::White());

    clickManager.DrawDebug();
}

void GameOverScene::Unload() {
    clickManager.Clear();
}

std::string GameOverScene::GetNextScene() const {
    return nextScene;
}