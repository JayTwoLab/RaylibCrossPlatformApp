#include "GameplayScene.hpp"
#include <algorithm>

GameplayScene::GameplayScene() {
    ResetFields();
}

void GameplayScene::ResetFields() {
    nextScene = "";
    playerHp = 100;
    score = 0;
}

void GameplayScene::Init() {
    ResetFields();
    clickManager.Clear();
    renderList.clear();

    // 1. 플레이어 스프라이트 설정 (Z: 10)
    playerSprite.RegisterClip("idle", "resources/character_idle.png");
    playerSprite.RegisterClip("hurt", "resources/character_hurt.png");
    playerSprite.RegisterClipSheet("walk", "resources/character_walk_sheet.png", 4, 0.12f);
    playerSprite.SetState("idle");
    playerSprite.SetPosition(400.0f, 300.0f);
    playerSprite.SetScale(1.0f);
    playerSprite.SetRotationSpeed(0.0f);
    playerSprite.SetZOrder(10);

    playerSprite.SetOnClick([this]() {
        TraceLog(LOG_INFO, "Player Sprite Clicked!");
        playerHp -= 10;
        playerSprite.SetState("hurt");
    });

    // 2. 바닥 배경 오브젝트 설정 (Z: 0)
    backgroundProp.RegisterClip("prop", "resources/character_idle.png");
    backgroundProp.SetPosition(400.0f, 300.0f);
    backgroundProp.SetScale(2.5f);
    backgroundProp.SetRotationSpeed(0.0f);
    backgroundProp.SetZOrder(0);

    backgroundProp.SetOnClick([this]() {
        TraceLog(LOG_INFO, "Background Prop Clicked!");
        score += 10;
    });

    // 3. 상단 회전 이펙트 설정 (Z: 20)
    floatingEffect.RegisterClip("effect", "resources/character_hurt.png");
    floatingEffect.SetPosition(400.0f, 270.0f);
    floatingEffect.SetScale(0.6f);
    floatingEffect.SetRotationSpeed(180.0f);
    floatingEffect.SetZOrder(20);

    floatingEffect.SetOnClick([this]() {
        TraceLog(LOG_INFO, "Floating Effect Clicked!");
        score += 50;
    });

    // 4. 렌더 목록 등록
    renderList.push_back(&playerSprite);
    renderList.push_back(&backgroundProp);
    renderList.push_back(&floatingEffect);

    // 5. 버튼 등록
    clickManager.AddRegion("hit_area", raylib::Rectangle{ 50, 500, 150, 40 }, [this]() {
        playerHp -= 50;
        if (playerHp <= 50 && playerHp > 0) {
            playerSprite.SetState("hurt");
        }
        });

    clickManager.AddRegion("score_area", raylib::Rectangle{ 220, 500, 150, 40 }, [this]() {
        score += 50;
        });
}

void GameplayScene::Update() {
    clickManager.Update();

    // 스프라이트 클릭 판정 (Z-Order 내림차순 검사)
    if (raylib::Mouse::IsButtonPressed(MOUSE_BUTTON_LEFT)) {
        raylib::Vector2 mousePos = Engine::Display::Display::Instance().GetVirtualMousePosition();

        auto sortedList = renderList;
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

    for (auto* sprite : renderList) {
        sprite->Update();
    }

    raylib::Vector2 pos = playerSprite.GetPosition();
    bool isMoving = false;

    if (IsKeyDown(KEY_RIGHT)) { pos.x += 4.0f; isMoving = true; }
    if (IsKeyDown(KEY_LEFT)) { pos.x -= 4.0f; isMoving = true; }
    if (IsKeyDown(KEY_UP)) { pos.y -= 4.0f; isMoving = true; }
    if (IsKeyDown(KEY_DOWN)) { pos.y += 4.0f; isMoving = true; }

    pos.x = std::clamp(pos.x, 30.0f, 770.0f);
    pos.y = std::clamp(pos.y, 30.0f, 570.0f);
    playerSprite.SetPosition(pos);

    floatingEffect.SetPosition(pos.x, pos.y - 30.0f);

    if (playerHp > 50) {
        if (isMoving) {
            playerSprite.SetState("walk");
        }
        else {
            playerSprite.SetState("idle");
        }
    }

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

    std::stable_sort(renderList.begin(), renderList.end(), [](const auto* a, const auto* b) {
        return a->GetZOrder() < b->GetZOrder();
        });

    for (const auto* sprite : renderList) {
        sprite->Draw();
    }

    raylib::DrawText("GAMEPLAY SCENE", 30, 30, 24, raylib::Color::Black());
    raylib::DrawText(TextFormat("Player HP: %d", playerHp), 30, 80, 20, raylib::Color::Red());
    raylib::DrawText(TextFormat("Score: %d / 100", score), 30, 110, 20, raylib::Color::DarkBlue());
    raylib::DrawText(TextFormat("Current State: %s", playerSprite.GetCurrentState().c_str()), 30, 140, 20, raylib::Color::DarkGreen());

    raylib::Rectangle(50, 500, 150, 40).Draw(raylib::Color::Red());
    raylib::DrawText("Click: Take Damage", 60, 512, 14, raylib::Color::White());

    raylib::Rectangle(220, 500, 150, 40).Draw(raylib::Color::Green());
    raylib::DrawText("Click: Add Score", 240, 512, 14, raylib::Color::Black());

    clickManager.DrawDebug();
}

void GameplayScene::Unload() {
    clickManager.Clear();
    for (auto* sprite : renderList) {
        sprite->Unload();
    }
    renderList.clear();
}

std::string GameplayScene::GetNextScene() const {
    return nextScene;
}
