#include "GameplayScene.hpp"
#include <algorithm>

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

    // 1. 플레이어 스프라이트 설정 (Z: 10)
    playerSprite_.RegisterClip("idle", "resources/character_idle.png");
    playerSprite_.RegisterClip("hurt", "resources/character_hurt.png");
    playerSprite_.RegisterClipSheet("walk", "resources/character_walk_sheet.png", 4, 0.12f);
    playerSprite_.SetState("idle");
    playerSprite_.SetPosition(400.0f, 300.0f);
    playerSprite_.SetScale(1.0f);
    playerSprite_.SetRotationSpeed(0.0f);
    playerSprite_.SetZOrder(10);

    playerSprite_.SetOnClick([this]() {
        TraceLog(LOG_INFO, "Player Sprite Clicked!");
        playerHp_ -= 10;
        playerSprite_.SetState("hurt");
    });

    // 2. 바닥 배경 오브젝트 설정 (Z: 0)
    backgroundProp_.RegisterClip("prop", "resources/back_char.png");
    backgroundProp_.SetPosition(400.0f, 300.0f);
    backgroundProp_.SetScale(2.5f);
    backgroundProp_.SetRotationSpeed(0.0f);
    backgroundProp_.SetZOrder(0);
    backgroundProp_.SetOnClick([this]() {
        TraceLog(LOG_INFO, "Background Prop Clicked!");
        score_ += 10;
    });
     
    // 3. 상단 회전 이펙트 설정 (Z: 20)
    floatingEffect_.RegisterClip("effect", "resources/rotate_char.png");
    floatingEffect_.SetPosition(400.0f, 270.0f);
    floatingEffect_.SetScale(0.6f);
    floatingEffect_.SetRotationSpeed(180.0f);
    floatingEffect_.SetZOrder(20);
    floatingEffect_.SetOnClick([this]() {
        TraceLog(LOG_INFO, "Floating Effect Clicked!");
        score_ += 50;
    });

    // 4. 렌더 목록 등록
    renderList_.push_back(&playerSprite_);
    renderList_.push_back(&backgroundProp_);
    renderList_.push_back(&floatingEffect_);

    // 5. 버튼 등록
    clickManager_.AddRegion("hit_area", raylib::Rectangle{ 50, 500, 150, 40 }, [this]() {
        playerHp_ -= 50;
        if (playerHp_ <= 50 && playerHp_ > 0) {
            playerSprite_.SetState("hurt");
        }
        });

    clickManager_.AddRegion("score_area", raylib::Rectangle{ 220, 500, 150, 40 }, [this]() {
        score_ += 50;
        });
}

void GameplayScene::Update() {
    clickManager_.Update();

    // 스프라이트 클릭 판정 (Z-Order 내림차순 검사)
    if (raylib::Mouse::IsButtonPressed(MOUSE_BUTTON_LEFT)) {
        raylib::Vector2 mousePos = Engine::Display::Display::Instance().GetVirtualMousePosition();

        auto sortedList = renderList_;
        std::sort(sortedList.begin(), sortedList.end(), [](const auto* a, const auto* b) {
            return a->GetZOrder() > b->GetZOrder();
            });

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
    if (IsKeyDown(KEY_LEFT)) { pos.x -= 4.0f; isMoving = true; }
    if (IsKeyDown(KEY_UP)) { pos.y -= 4.0f; isMoving = true; }
    if (IsKeyDown(KEY_DOWN)) { pos.y += 4.0f; isMoving = true; }

    pos.x = std::clamp(pos.x, 30.0f, 770.0f);
    pos.y = std::clamp(pos.y, 30.0f, 570.0f);
    playerSprite_.SetPosition(pos);

    floatingEffect_.SetPosition(pos.x, pos.y - 30.0f);

    if (playerHp_ > 50) {
        if (isMoving) {
            playerSprite_.SetState("walk");
        }
        else {
            playerSprite_.SetState("idle");
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

    std::stable_sort(renderList_.begin(), renderList_.end(), [](const auto* a, const auto* b) {
        return a->GetZOrder() < b->GetZOrder();
        });

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
