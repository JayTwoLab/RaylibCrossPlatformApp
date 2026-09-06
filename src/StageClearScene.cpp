#include "StageClearScene.hpp"

StageClearScene::StageClearScene() {
    nextScene_ = "";
}

void StageClearScene::Init() {
    nextScene_ = "";
    clickManager_.Clear();

    clickManager_.AddRegion(
        "back_to_title", raylib::Rectangle{ 330, 250, 140, 50 },
        [this]() { this->nextScene_ = "Title"; });

}

void StageClearScene::Update() {
    clickManager_.Update();
} 

void StageClearScene::Draw() {
    raylib::Color::DarkGreen().ClearBackground();
    raylib::DrawText("STAGE CLEAR!", 250, 150, 40, raylib::Color::Gold());

    raylib::Rectangle(330, 250, 140, 50).Draw(raylib::Color::DarkBlue());
    raylib::DrawText("TITLE", 370, 265, 20, raylib::Color::White());

    clickManager_.Draw();
    clickManager_.DrawBoundary();
}

void StageClearScene::Unload() {
    clickManager_.Clear();
}

std::string StageClearScene::GetNextScene() {
    return nextScene_;
}
