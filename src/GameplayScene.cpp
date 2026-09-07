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
    ResetFields();
    clickManager_.Clear();
    renderList_.clear();

    // 1. Player 스프라이트
    playerSprite_.RegisterClip("idle", "character_idle.png");
    playerSprite_.RegisterClip("hurt", "character_hurt.png");
    playerSprite_.RegisterClipSheet("walk", "character_walk_sheet.png", 4, 0.12f);
    playerSprite_.SetClipState("idle");
    playerSprite_.SetPosition(400.0f, 300.0f);
    playerSprite_.SetScale(1.0f);
    playerSprite_.SetRotationSpeed(0.0f);
    playerSprite_.SetZOrder(10);
    playerSprite_.SetOnClick([this]() {
        playerHp_ -= 10;
        TraceLog(LOG_INFO, "Player Sprite Clicked! HP: %d", playerHp_);
        playerSprite_.SetClipState("hurt");
        });

    // 2. 배경 오브젝트
    backgroundProp_.RegisterClip("prop", "back_char.png");
    backgroundProp_.SetPosition(400.0f, 300.0f);
    backgroundProp_.SetScale(2.5f);
    backgroundProp_.SetRotationSpeed(0.0f);
    backgroundProp_.SetZOrder(0);
    backgroundProp_.SetOnClick([this]() {
        score_ += 10;
        TraceLog(LOG_INFO, "Background Prop Clicked! Score: %d", score_);
        });

    // 3. 회전 이펙트
    floatingEffect_.RegisterClip("effect", "rotate_char.png");
    floatingEffect_.SetPosition(400.0f, 270.0f);
    floatingEffect_.SetScale(0.6f);
    floatingEffect_.SetRotationSpeed(180.0f);
    floatingEffect_.SetZOrder(20);
    floatingEffect_.SetOnClick([this]() {
        score_ += 50;
        TraceLog(LOG_INFO, "Floating Effect Clicked! Score: %d", score_);
        });

    // 4. 렌더 리스트 등록 및 정렬
    renderList_.push_back(&playerSprite_);
    renderList_.push_back(&backgroundProp_);
    renderList_.push_back(&floatingEffect_);

    std::stable_sort(renderList_.begin(), renderList_.end(), [](const auto* a, const auto* b) {
        return a->GetZOrder() < b->GetZOrder();
        });

    // 5. 버튼 등록
    clickManager_.AddRegion("hit_area", raylib::Rectangle{ 50, 500, 150, 40 }, [this]() {
        playerHp_ -= 50;
        if (playerHp_ <= 50 && playerHp_ > 0) {
            playerSprite_.SetClipState("hurt");
        }
        });

    clickManager_.AddRegion("score_area", raylib::Rectangle{ 220, 500, 150, 40 }, [this]() {
        score_ += 50;
        });
}

// Update, Draw, Unload, GetNextScene 은 기존과 동일
void GameplayScene::Update() {
    clickManager_.Update();

    if (raylib::Mouse::IsButtonPressed(MOUSE_BUTTON_LEFT)) {
        raylib::Vector2 mousePos = Engine::Display::Display::Instance().GetVirtualMousePosition();
        auto sortedList = renderList_;
        for (auto* sprite : sortedList) {
            if (sprite->GetBounds().CheckCollision(mousePos)) {
                if (sprite->TriggerClick()) break;
            }
        }
    }

    for (auto* sprite : renderList_) sprite->Update();

    raylib::Vector2 pos = playerSprite_.GetPosition();
    bool isMoving = false;
    if (IsKeyDown(KEY_RIGHT)) { pos.x += 4.0f; isMoving = true; }
    if (IsKeyDown(KEY_LEFT)) { pos.x -= 4.0f; isMoving = true; }
    if (IsKeyDown(KEY_UP)) { pos.y -= 4.0f; isMoving = true; }
    if (IsKeyDown(KEY_DOWN)) { pos.y += 4.0f; isMoving = true; }

    pos.x = std::clamp(pos.x, 30.0f, 770.0f);
    pos.y = std::clamp(pos.y, 30.0f, 570.0f);
    playerSprite_.SetPosition(pos);
    floatingEffect_.SetPosition(pos.x, pos.y - 30.0f);

    if (playerHp_ > 50) {
        playerSprite_.SetClipState(isMoving ? "walk" : "idle");
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
    for (const auto* sprite : renderList_) sprite->Draw();

    raylib::DrawText("GAMEPLAY SCENE", 30, 30, 24, raylib::Color::Black());
    raylib::DrawText(TextFormat("Player HP: %d", playerHp_), 30, 80, 20, raylib::Color::Red());
    raylib::DrawText(TextFormat("Score: %d / 100", score_), 30, 110, 20, raylib::Color::DarkBlue());
    raylib::DrawText(TextFormat("Current State: %s", playerSprite_.GetCurrentState().c_str()), 30, 140, 20, raylib::Color::DarkGreen());

    raylib::Rectangle(50, 500, 150, 40).Draw(raylib::Color::Red());
    raylib::DrawText("Click: Take Damage", 60, 512, 14, raylib::Color::White());

    raylib::Rectangle(220, 500, 150, 40).Draw(raylib::Color::Green());
    raylib::DrawText("Click: Add Score", 240, 512, 14, raylib::Color::Black());

    clickManager_.Draw();
    clickManager_.DrawBoundary();
}

void GameplayScene::Unload() {
    clickManager_.Clear();
    for (auto* sprite : renderList_) sprite->Unload();
    renderList_.clear();
}

std::string GameplayScene::GetNextScene() {
    return nextScene_;
}
