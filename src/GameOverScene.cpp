#include <cassert>
#include "GameOverScene.hpp"

GameOverScene::GameOverScene() {
    nextScene_ = "";
    nanumBoldFont_ = nullptr;
}

void GameOverScene::Init() {
    nextScene_ = "";
    clickManager_.Clear();

    // 1. 재시도 버튼
    clickManager_.AddRegion(
        "retry_button", raylib::Rectangle{ 480, 360, 140, 50 },
        [this]() { this->nextScene_ = "Gameplay"; }
    );

    auto sprite = std::make_shared<Engine::Graphics::RotatingSprite>();
    sprite->RegisterClip("backchar", "back_char.png");
    sprite->SetClipState("backchar");
    sprite->SetRotationSpeed(180.0f);
    clickManager_.SetRegionSprite("retry_button", sprite);

    // 2. 타이틀 버튼
    clickManager_.AddRegion(
        "title_button", raylib::Rectangle{ 660, 360, 140, 50 },
        [this]() { this->nextScene_ = "Title"; }
    );

    // 3. 한글 폰트 로드
    std::vector<int> codepoints;
    codepoints.reserve(95 + 11172);
    for (int i = 32; i <= 126; ++i) {
        codepoints.push_back(i);
    }
    for (int i = 0xAC00; i <= 0xD7A3; ++i) {
        codepoints.push_back(i);
    }

    nanumBoldFont_ = Engine::Resource::ResourceManager::Instance().LoadFontExShared(
        "NanumGothicBold.ttf", 32, codepoints
    );

    assert(nanumBoldFont_ != nullptr);
    clickManager_.SetRegionText(
        "title_button",
        "To Title",
        nanumBoldFont_,
        raylib::Color::White(),
        raylib::Color::DarkGray(),
        22
    );
}

void GameOverScene::Update() {
    clickManager_.Update();
}

void GameOverScene::Draw() {
    raylib::Color::Maroon().ClearBackground();
    raylib::DrawText("GAME OVER", 500, 240, 50, raylib::Color::White());
    clickManager_.Draw();
    clickManager_.DrawBoundary();
}

void GameOverScene::Unload() {
    clickManager_.Clear();
    nanumBoldFont_.reset();
}

std::string GameOverScene::GetNextScene() {
    return nextScene_;
}
