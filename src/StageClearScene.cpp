#include "StageClearScene.hpp"

StageClearScene::StageClearScene() {
    nextScene_ = "";
}

void StageClearScene::Init() {
    nextScene_ = "";
    clickManager_.Clear();

    clickManager_.AddRegion(
        "back_to_title", raylib::Rectangle{ 540, 400, 200, 60 },
        [this]() { this->nextScene_ = "Title"; }
    );
    clickManager_.SetRegionText("back_to_title", "TITLE", raylib::Color::White(), raylib::Color::DarkBlue(), 24);
}

void StageClearScene::Update() {
    clickManager_.Update();
}

void StageClearScene::Draw() {
    raylib::Color::DarkGreen().ClearBackground();
    raylib::DrawText("STAGE CLEAR!", 470, 250, 50, raylib::Color::Gold());

    clickManager_.Draw();
    clickManager_.DrawBoundary();
}

void StageClearScene::Unload() {
    clickManager_.Clear();
}

std::string StageClearScene::GetNextScene() {
    return nextScene_;
}
