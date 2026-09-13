#include "StageClearScene.hpp"

StageClearScene::StageClearScene() {
    nextScene_ = "";
}

void StageClearScene::Init() {
    nextScene_ = "";
    clickManager_.Clear();

    {
        auto back_to_title_name = "back_to_title";
        clickManager_.AddRegion(
            back_to_title_name, raylib::Rectangle{ 540, 400, 200, 60 },
            [this]() { this->nextScene_ = "Title"; }
        );

        auto fontSize = 24;
        auto fontColor = raylib::Color::White();
        auto backgroundColor = raylib::Color::DarkBlue();
        clickManager_.SetRegionText(back_to_title_name, "TITLE", fontColor, backgroundColor, fontSize);
    }
}

void StageClearScene::Update() {
    clickManager_.Update();
}

void StageClearScene::Draw() {
    raylib::Color::DarkGreen().ClearBackground();

    {
        auto textX = 470;
        auto textY = 250;
        auto fontSize = 50;
        auto textColor = raylib::Color::Gold();
        raylib::DrawText("STAGE CLEAR!", textX, textY, fontSize, textColor);
    }

    {
        clickManager_.Draw();
        clickManager_.DrawBoundary();
    }
}

void StageClearScene::Unload() {
    clickManager_.Clear();
}

std::string StageClearScene::GetNextScene() {
    return nextScene_;
}
