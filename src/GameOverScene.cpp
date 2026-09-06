#include <cassert>
#include "GameOverScene.hpp"

GameOverScene::GameOverScene() {
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

    nanumBoldFont_ = std::make_shared<raylib::Font>(); // Korean font
    std::vector<int> codepoints;
    for (int i = 32; i <= 126; ++i) { // ASCII (32 ~ 126)
        codepoints.push_back(i);
    }
    for (int i = 0xAC00; i <= 0xD7A3; ++i) { // Hangul (가 ~ 힣: 0xAC00 ~ 0xD7A3, total 11,172 characters)
        codepoints.push_back(i);
    }
    *nanumBoldFont_ = raylib::LoadFontEx(
        (resPath / "NanumGothicBold.ttf").string().c_str(),
        32, // Reference size for generating the font texture (recommended: slightly larger than the size to be used)
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

    clickManager_.Draw(); // draw sprite in clickable area
    clickManager_.DrawBoundary(); // draw debug rectangles for clickable areas
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
