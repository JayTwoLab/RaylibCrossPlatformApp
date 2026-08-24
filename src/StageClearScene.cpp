#include "StageClearScene.hpp"

StageClearScene::StageClearScene() : nextScene("") {}

void StageClearScene::Init() {
    nextScene = "";
    clickManager.Clear();

    clickManager.AddRegion("back_to_title", raylib::Rectangle{ 330, 250, 140, 50 }, [this]() {
        this->nextScene = "Title";
        });
}

void StageClearScene::Update() {
    clickManager.Update();
} 

void StageClearScene::Draw() {
    raylib::Color::DarkGreen().ClearBackground();
    raylib::DrawText("STAGE CLEAR!", 250, 150, 40, raylib::Color::Gold());

    raylib::Rectangle(330, 250, 140, 50).Draw(raylib::Color::DarkBlue());
    raylib::DrawText("TITLE", 370, 265, 20, raylib::Color::White());

    clickManager.DrawDebug();
}

void StageClearScene::Unload() {
    clickManager.Clear();
}

std::string StageClearScene::GetNextScene() const {
    return nextScene;
}
