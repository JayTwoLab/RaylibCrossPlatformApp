#include <algorithm>
#include "Engine/Engine.hpp"
#include "GameplayScene.hpp"

GameplayScene::GameplayScene() {
    ResetFields();
}

void GameplayScene::ResetFields() {
    nextScene_ = "";
    playerHp_ = 100;
    score_ = 0;
}

void GameplayScene::Init() {

    namespace ER = Engine::Resource;
    using ERM = ER::ResourceManager;
    auto& resourceManager = ERM::Instance();

    auto resPath = resourceManager.GetResourcePath(); // resource path

    ResetFields();
    clickManager_.Clear();
    renderList_.clear();

    // 1. Player sprite setup (Z: 10)
    playerSprite_.RegisterClip("idle", (resPath / "character_idle.png").string());
    playerSprite_.RegisterClip("hurt", (resPath / "character_hurt.png").string());
    playerSprite_.RegisterClipSheet("walk", (resPath / "character_walk_sheet.png").string(), 4, 0.12f);
    playerSprite_.SetClipState("idle"); // set initial clip state
    playerSprite_.SetPosition(400.0f, 300.0f);
    playerSprite_.SetScale(1.0f);
    playerSprite_.SetRotationSpeed(0.0f);
    playerSprite_.SetZOrder(10); // Lower Z-order renders first
    playerSprite_.SetOnClick([this]() {
        playerHp_ -= 10;
        TraceLog(LOG_INFO, "Player Sprite Clicked! HP: %d", playerHp_);
        playerSprite_.SetClipState("hurt");
    });

    // 2. Ground/background prop setup (Z: 0)
    backgroundProp_.RegisterClip("prop", (resPath / "back_char.png").string());
    backgroundProp_.SetPosition(400.0f, 300.0f);
    backgroundProp_.SetScale(2.5f);
    backgroundProp_.SetRotationSpeed(0.0f);
    backgroundProp_.SetZOrder(0);
    backgroundProp_.SetOnClick([this]() {
        score_ += 10;
        TraceLog(LOG_INFO, "Background Prop Clicked! Score: %d", score_);
    });
     
    // 3. Top rotating effect setup (Z: 20)
    floatingEffect_.RegisterClip("effect", (resPath / "rotate_char.png").string());
    floatingEffect_.SetPosition(400.0f, 270.0f);
    floatingEffect_.SetScale(0.6f);
    floatingEffect_.SetRotationSpeed(180.0f);
    floatingEffect_.SetZOrder(20);
    floatingEffect_.SetOnClick([this]() {
        score_ += 50;
        TraceLog(LOG_INFO, "Floating Effect Clicked! Score: %d", score_);
    });

    // 4. Register to render list
    renderList_.push_back(&playerSprite_);
    renderList_.push_back(&backgroundProp_);
    renderList_.push_back(&floatingEffect_);

    // Stable sort by Z-order (lower Z renders first)
    std::stable_sort(renderList_.begin(), renderList_.end(), [](const auto* a, const auto* b) {
        auto ret = a->GetZOrder() < b->GetZOrder();
        return ret;
    });

    // 5. Register buttons
    clickManager_.AddRegion(
        "hit_area",
        raylib::Rectangle{ 50, 500, 150, 40 },
        [this]() {
            playerHp_ -= 50;
            if (playerHp_ <= 50 && playerHp_ > 0) {
                playerSprite_.SetClipState("hurt");
            }
        }
    );

    clickManager_.AddRegion(
        "score_area",
        raylib::Rectangle{ 220, 500, 150, 40 },
        [this]() {
            score_ += 50;
        }
    );

}

void GameplayScene::Update() {
    clickManager_.Update();

    // Sprite click detection (check in descending Z-order)
    if (raylib::Mouse::IsButtonPressed(MOUSE_BUTTON_LEFT)) {
        namespace ED = Engine::Display;
        namespace EG = Engine::Graphics;

        using EDD = Engine::Display::Display;
        using RotatingSprite = EG::RotatingSprite;
        auto& display = EDD::Instance();

        raylib::Vector2 mousePos = display.GetVirtualMousePosition();

        auto sortedList = renderList_;

        for (auto* sprite : sortedList) {
            if (sprite->GetBounds().CheckCollision(mousePos)) {
                if (sprite->TriggerClick()) {
                    break;
                }
            }
        }
    }

    for (auto* sprite : renderList_) {
        sprite->Update();
    }

    raylib::Vector2 pos = playerSprite_.GetPosition();
    bool isMoving = false;

    if (IsKeyDown(KEY_RIGHT)) { pos.x += 4.0f; isMoving = true; }
    if (IsKeyDown(KEY_LEFT))  { pos.x -= 4.0f; isMoving = true; }
    if (IsKeyDown(KEY_UP))    { pos.y -= 4.0f; isMoving = true; }
    if (IsKeyDown(KEY_DOWN))  { pos.y += 4.0f; isMoving = true; }

    pos.x = std::clamp(pos.x, 30.0f, 770.0f);
    pos.y = std::clamp(pos.y, 30.0f, 570.0f);
    playerSprite_.SetPosition(pos);

    floatingEffect_.SetPosition(pos.x, pos.y - 30.0f);

    if (playerHp_ > 50) {
        if (isMoving) {
            playerSprite_.SetClipState("walk");
        }
        else {
            playerSprite_.SetClipState("idle");
        }
    }

    if (playerHp_ <= 0) {
        nextScene_ = "GameOver";
        return;
    }

    if (score_ >= 100) {
        nextScene_ = "StageClear";
        return;
    }
}

void GameplayScene::Draw() {
    raylib::Color::LightGray().ClearBackground();
 
    for (const auto* sprite : renderList_) {
        sprite->Draw();
    }

    raylib::DrawText("GAMEPLAY SCENE", 30, 30, 24, raylib::Color::Black());
    raylib::DrawText(TextFormat("Player HP: %d", playerHp_), 30, 80, 20, raylib::Color::Red());
    raylib::DrawText(TextFormat("Score: %d / 100", score_), 30, 110, 20, raylib::Color::DarkBlue());
    raylib::DrawText(TextFormat("Current State: %s", playerSprite_.GetCurrentState().c_str()), 30, 140, 20, raylib::Color::DarkGreen());

    raylib::Rectangle(50, 500, 150, 40).Draw(raylib::Color::Red());
    raylib::DrawText("Click: Take Damage", 60, 512, 14, raylib::Color::White());

    raylib::Rectangle(220, 500, 150, 40).Draw(raylib::Color::Green());
    raylib::DrawText("Click: Add Score", 240, 512, 14, raylib::Color::Black());

    clickManager_.Draw();
    clickManager_.DrawDebug();
}

void GameplayScene::Unload() {
    clickManager_.Clear();
    for (auto* sprite : renderList_) {
        sprite->Unload();
    }
    renderList_.clear();
}

std::string GameplayScene::GetNextScene() {
    return nextScene_;
}
