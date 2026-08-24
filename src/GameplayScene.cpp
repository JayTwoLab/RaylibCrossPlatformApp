#include "GameplayScene.hpp"

GameplayScene::GameplayScene() {
    ResetFields();
}

void GameplayScene::ResetFields() {
    nextScene = "";
    playerHp = 100;
    score = 0;
    playerPos = { 400.0f, 225.0f };
}

void GameplayScene::Init() {
    ResetFields();

    clickManager.Clear();

    clickManager.AddRegion("hit_area", raylib::Rectangle{ 50, 350, 150, 40 }, [this]() {
        playerHp -= 50;
    });

    clickManager.AddRegion("score_area", raylib::Rectangle{ 220, 350, 150, 40 }, [this]() {
        score += 50;
    });
}

void GameplayScene::Update() {
    clickManager.Update();

    if (IsKeyDown(KEY_RIGHT)) playerPos.x += 4.0f;
    if (IsKeyDown(KEY_LEFT))  playerPos.x -= 4.0f;
    if (IsKeyDown(KEY_UP))    playerPos.y -= 4.0f;
    if (IsKeyDown(KEY_DOWN))  playerPos.y += 4.0f;

    if (playerHp <= 0) {
        nextScene = "GameOver";
        return;
    }

    if (score >= 100) {
        nextScene = "StageClear";
        return;
    }
}

void GameplayScene::Draw() {
    raylib::Color::LightGray().ClearBackground();
    playerPos.DrawCircle(25.0f, raylib::Color::Maroon());

    raylib::DrawText("GAMEPLAY SCENE", 30, 30, 24, raylib::Color::Black());
    raylib::DrawText(TextFormat("Player HP: %d", playerHp), 30, 80, 20, raylib::Color::Red());
    raylib::DrawText(TextFormat("Score: %d / 100", score), 30, 110, 20, raylib::Color::DarkBlue());

    raylib::Rectangle(50, 350, 150, 40).Draw(raylib::Color::Red());
    raylib::DrawText("Click: Take Damage", 60, 362, 14, raylib::Color::White());

    raylib::Rectangle(220, 350, 150, 40).Draw(raylib::Color::Green());
    raylib::DrawText("Click: Add Score", 240, 362, 14, raylib::Color::Black());

    clickManager.DrawDebug();
}

void GameplayScene::Unload() {
    clickManager.Clear();
}

std::string GameplayScene::GetNextScene() const {
    return nextScene;
}