#include <cassert>
#include "GameOverScene.hpp"

GameOverScene::GameOverScene() {
    // next scene 초기화
    nextScene_ = "";

    nanumBoldFont_ = nullptr;
}

void GameOverScene::Init() {

    namespace ER = Engine::Resource;
    using ERM = ER::ResourceManager;
    auto& resourceManager = ERM::Instance();

    auto resPath = resourceManager.GetResourcePath(); // resource path

    nextScene_ = "";
    clickManager_.Clear();

    // "retry_button"
    clickManager_.AddRegion(
        "retry_button", raylib::Rectangle{ 250, 250, 130, 50 },
        [this]() { this->nextScene_ = "Gameplay"; }
    );
    auto sprite = std::make_shared<Engine::Graphics::RotatingSprite>();
    sprite->RegisterClip("backchar", (resPath / "back_char.png").string());
    sprite->SetClipState("backchar");
    // sprite->SetScale(2.0f);
    // sprite->SetPosition(250, 250);
    sprite->SetRotationSpeed(180.0f);
    clickManager_.SetRegionSprite("retry_button", sprite);

    // "title_button" 
    clickManager_.AddRegion(
        "title_button", raylib::Rectangle{ 420, 250, 130, 50 },
        [this]() { this->nextScene_ = "Title"; }
    );

    // 1. 필요한 유니코드 코드포인트 목록 준비 (기본 ASCII + 한글 음절 범위)
    std::vector<int> codepoints;

    // 기본 ASCII (32 ~ 126)
    for (int i = 32; i <= 126; ++i) {
        codepoints.push_back(i);
    }

    // 한글 완성형 음절 (가 ~ 힣: 0xAC00 ~ 0xD7A3, 총 11,172자)
    for (int i = 0xAC00; i <= 0xD7A3; ++i) {
        codepoints.push_back(i);
    }

    nanumBoldFont_ = std::make_shared<raylib::Font>();
    *nanumBoldFont_ = raylib::LoadFontEx(
        (resPath / "NanumGothicBold.ttf").string().c_str(),
        32, // 폰트 텍스처를 생성할 기준 크기 (권장: 사용 크기보다 조금 넉넉하게)
        codepoints.data(),
        static_cast<int>(codepoints.size())
    );

    assert(nanumBoldFont_);
    clickManager_.SetRegionText("title_button",
        "TITLE타이틀", nanumBoldFont_, raylib::Color::White(), raylib::Color::DarkGray(), 20);

}

void GameOverScene::Update() { 
    clickManager_.Update();
}

void GameOverScene::Draw() {
    raylib::Color::Maroon().ClearBackground();

    raylib::DrawText("GAME OVER", 280, 150, 40, raylib::Color::White());
    // raylib::Rectangle(250, 250, 130, 50).Draw(raylib::Color::DarkGray());
    // raylib::DrawText("RETRY", 285, 265, 20, raylib::Color::White());
    // raylib::Rectangle(420, 250, 130, 50).Draw(raylib::Color::DarkGray());
    // raylib::DrawText("TITLE", 460, 265, 20, raylib::Color::White());

    clickManager_.Draw(); // draw sprite in clickable area
    clickManager_.DrawDebug(); // draw debug rectangles for clickable areas
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
