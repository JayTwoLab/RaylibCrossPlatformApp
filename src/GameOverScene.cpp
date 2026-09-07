#include <cassert>
#include "GameOverScene.hpp"

GameOverScene::GameOverScene() {
    nextScene_ = "";
    nanumBoldFont_ = nullptr;
}

void GameOverScene::Init() {
    nextScene_ = "";
    clickManager_.Clear();

    // 1. 재시도 버튼 및 스프라이트
    clickManager_.AddRegion(
        "retry_button", raylib::Rectangle{ 250, 250, 130, 50 },
        [this]() { this->nextScene_ = "Gameplay"; }
    );

    auto sprite = std::make_shared<Engine::Graphics::RotatingSprite>();
    // 파일명만 넘기면 ResourceManager가 디스크 또는 rres에서 자동 탐색
    sprite->RegisterClip("backchar", "back_char.png");
    sprite->SetClipState("backchar");
    sprite->SetRotationSpeed(180.0f);
    clickManager_.SetRegionSprite("retry_button", sprite);

    // 2. 타이틀 버튼
    clickManager_.AddRegion(
        "title_button", raylib::Rectangle{ 420, 250, 130, 50 },
        [this]() { this->nextScene_ = "Title"; }
    );

    // 3. 한글 폰트 생성 및 로드
    std::vector<int> codepoints;
    codepoints.reserve(95 + 11172);
    for (int i = 32; i <= 126; ++i) { // ASCII
        codepoints.push_back(i);
    }
    for (int i = 0xAC00; i <= 0xD7A3; ++i) { // 한글 완성형 전체
        codepoints.push_back(i);
    }

    // ResourceManager를 통한 확장 폰트 로드
    nanumBoldFont_ = Engine::Resource::ResourceManager::Instance().LoadFontExShared(
        "NanumGothicBold.ttf", 32, codepoints
    );

    assert(nanumBoldFont_ != nullptr);
    clickManager_.SetRegionText(
        "title_button",
        "TITLE타이틀",
        nanumBoldFont_,
        raylib::Color::White(),
        raylib::Color::DarkGray(),
        20
    );
}

void GameOverScene::Update() {
    clickManager_.Update();
}

void GameOverScene::Draw() {
    raylib::Color::Maroon().ClearBackground();
    raylib::DrawText("GAME OVER", 280, 150, 40, raylib::Color::White());
    clickManager_.Draw();
    clickManager_.DrawBoundary();
}

void GameOverScene::Unload() {
    clickManager_.Clear();
    if (nanumBoldFont_) {
        nanumBoldFont_->Unload();
        nanumBoldFont_ = nullptr;
    }
}

std::string GameOverScene::GetNextScene() {
    return nextScene_;
}
