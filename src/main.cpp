#include "raylib-cpp.hpp"
#include <algorithm>

#include "Engine/Engine.hpp"

#include "TitleScene.hpp"
#include "GameplayScene.hpp"
#include "GameOverScene.hpp"
#include "StageClearScene.hpp"

int main() {
    namespace ED = Engine::Display;
    using EDD = Engine::Display::Display;

    EDD::Instance().VirtualWidth  = 800;
	EDD::Instance().VirtualHeight = 600;

	const int virtualScreenWidth = EDD::Instance().VirtualWidth;
	const int virtualScreenHeight = EDD::Instance().VirtualHeight;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);

    raylib::Window window(virtualScreenWidth, virtualScreenHeight, "Raylib-CPP Scalable App");
    SetTargetFPS(60);

    raylib::RenderTexture2D target(virtualScreenWidth, virtualScreenHeight);
    SetTextureFilter(target.GetTexture(), TEXTURE_FILTER_BILINEAR);

    Engine::Scene::SceneManager sceneManager;

    sceneManager.RegisterScene<TitleScene>("Title");
    sceneManager.RegisterScene<GameplayScene>("Gameplay");
    sceneManager.RegisterScene<GameOverScene>("GameOver");
    sceneManager.RegisterScene<StageClearScene>("StageClear");

    sceneManager.ChangeScene("Title");

    while (!window.ShouldClose()) {
        int screenWidth = GetScreenWidth();
        int screenHeight = GetScreenHeight();

        auto scaleX = (float)screenWidth / (float)virtualScreenWidth;
		auto scaleY = (float)screenHeight / (float)virtualScreenHeight;
        float scale = std::min(scaleX, scaleY);

        // 씬 로직 업데이트 (ClickableAreaManager 내부에서 보정 좌표 자동 사용)
        sceneManager.Update();

        // 가상 렌더 텍스처에 그리기
        target.BeginMode();
        sceneManager.Draw();
        target.EndMode();

        // 레터박스 적용 후 화면에 렌더링
        window.BeginDrawing();
        raylib::Color::Black().ClearBackground();

        raylib::Rectangle srcRect = {
            0.0f, 0.0f,
            (float)target.GetTexture().width,
            -(float)target.GetTexture().height
        };

        raylib::Rectangle destRect = {
            (screenWidth - (virtualScreenWidth * scale)) * 0.5f,
            (screenHeight - (virtualScreenHeight * scale)) * 0.5f,
            virtualScreenWidth * scale,
            virtualScreenHeight * scale
        };

        target.GetTexture().Draw(srcRect, destRect, raylib::Vector2{ 0, 0 }, 0.0f, raylib::Color::White());
        window.EndDrawing();
    }

    return 0;
}