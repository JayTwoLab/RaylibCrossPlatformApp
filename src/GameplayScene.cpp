#include <algorithm>
#include "GameplayScene.hpp"
#include "Engine/Engine.hpp"

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
    playerSprite_.SetPosition(640.0f, 360.0f);
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
    backgroundProp_.SetPosition(640.0f, 360.0f);
    backgroundProp_.SetScale(2.5f);
    backgroundProp_.SetRotationSpeed(0.0f);
    backgroundProp_.SetZOrder(0);
    backgroundProp_.SetOnClick([this]() {
        score_ += 10;
        TraceLog(LOG_INFO, "Background Prop Clicked! Score: %d", score_);
        });

    // 3. 회전 이펙트
    floatingEffect_.RegisterClip("effect", "rotate_char.png");
    floatingEffect_.SetPosition(640.0f, 320.0f);
    floatingEffect_.SetScale(0.6f);
    floatingEffect_.SetRotationSpeed(180.0f);
    floatingEffect_.SetZOrder(20);
    floatingEffect_.SetOnClick([this]() {
        score_ += 50;
        TraceLog(LOG_INFO, "Floating Effect Clicked! Score: %d", score_);
        });

    renderList_.push_back(&playerSprite_);
    renderList_.push_back(&backgroundProp_);
    renderList_.push_back(&floatingEffect_);

    std::stable_sort(renderList_.begin(), renderList_.end(), [](const auto* a, const auto* b) {
        return a->GetZOrder() < b->GetZOrder();
        });

    // 4. 하단 버튼 영역 등록
    clickManager_.AddRegion("hit_area", raylib::Rectangle{ 50, 620, 180, 50 }, [this]() {
        playerHp_ -= 50;
        if (playerHp_ <= 50 && playerHp_ > 0) {
            playerSprite_.SetClipState("hurt");
        }
        });
    clickManager_.SetRegionText("hit_area", "Damage (-50)", raylib::Color::White(), raylib::Color::Red(), 16);

    clickManager_.AddRegion("score_area", raylib::Rectangle{ 250, 620, 180, 50 }, [this]() {
        score_ += 50;
        });
    clickManager_.SetRegionText("score_area", "Get Score (+50)", raylib::Color::Black(), raylib::Color::Green(), 16);
}

void GameplayScene::Update() {
    clickManager_.Update();

    // ImGui 점유 시 게임 오브젝트 클릭 방지
    if (!Engine::UI::WantCaptureMouse() &&
        raylib::Mouse::IsButtonPressed(MOUSE_BUTTON_LEFT))
    {
        raylib::Vector2 mousePos = Engine::Display::Display::Instance().GetVirtualMousePosition();
        for (auto it = renderList_.rbegin(); it != renderList_.rend(); ++it) {
            if ((*it)->GetBounds().CheckCollision(mousePos)) {
                if ((*it)->TriggerClick()) {
                    break;
                }
            }
        }
    }

    for (auto* sprite : renderList_) {
        sprite->Update();
    }

    // ImGui 점유 시 키보드 이동 방지
    raylib::Vector2 pos = playerSprite_.GetPosition();
    bool isMoving = false;

    if (!Engine::UI::WantCaptureKeyboard()) {
        if (IsKeyDown(KEY_RIGHT)) { pos.x += 5.0f; isMoving = true; }
        if (IsKeyDown(KEY_LEFT)) { pos.x -= 5.0f; isMoving = true; }
        if (IsKeyDown(KEY_UP)) { pos.y -= 5.0f; isMoving = true; }
        if (IsKeyDown(KEY_DOWN)) { pos.y += 5.0f; isMoving = true; }
    }

    pos.x = std::clamp(pos.x, 50.0f, 1230.0f);
    pos.y = std::clamp(pos.y, 50.0f, 670.0f);
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

    raylib::DrawText("GAMEPLAY SCENE", 40, 40, 28, raylib::Color::Black());
    raylib::DrawText(TextFormat("Player HP: %d", playerHp_), 40, 85, 22, raylib::Color::Red());
    raylib::DrawText(TextFormat("Score: %d / 100", score_), 40, 115, 22, raylib::Color::DarkBlue());
    raylib::DrawText(TextFormat("Current State: %s", playerSprite_.GetCurrentState().c_str()), 40, 145, 20, raylib::Color::DarkGreen());

    clickManager_.Draw();
    clickManager_.DrawBoundary();
}

void GameplayScene::DrawImGui() {
    ImGui::Begin("Gameplay Inspector");
    {
        ImGui::Text("Performance: %.1f FPS (%.2f ms)", ImGui::GetIO().Framerate, 1000.0f / ImGui::GetIO().Framerate);
        ImGui::Separator();

        ImGui::SliderInt("Player HP", &playerHp_, 0, 100);
        ImGui::SliderInt("Game Score", &score_, 0, 100);

        if (ImGui::Button("Restore Full HP")) {
            playerHp_ = 100;
        }
        ImGui::SameLine();
        if (ImGui::Button("Reset Game")) {
            ResetFields();
        }

        ImGui::Separator();
        ImGui::Text("Input Capture Status:");
        ImGui::BulletText("Mouse Capture (WantCaptureMouse): %s", Engine::UI::WantCaptureMouse() ? "YES" : "NO");
        ImGui::BulletText("Keyboard Capture (WantCaptureKeyboard): %s", Engine::UI::WantCaptureKeyboard() ? "YES" : "NO");
    }
    ImGui::End();
}

void GameplayScene::Unload() {
    clickManager_.Clear();
    for (auto* sprite : renderList_)
        sprite->Unload();
    renderList_.clear();
}

std::string GameplayScene::GetNextScene() {
    return nextScene_;
}
