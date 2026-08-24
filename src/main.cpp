#include "raylib-cpp.hpp"
#include <algorithm>

#include "Engine/Engine.hpp"

#include "TitleScene.hpp"
#include "GameplayScene.hpp"
#include "GameOverScene.hpp"
#include "StageClearScene.hpp"

int main() {

#ifdef NDEBUG
    // 릴리즈 모드: 치명적인 에러만 출력하거나 아예 끔
    SetTraceLogLevel(LOG_FATAL); // 또는 LOG_NONE
#else
    // 디버그 모드: 모든 정보/경고/에러 출력
    SetTraceLogLevel(LOG_ALL);   // 또는 LOG_INFO
#endif

    namespace ED = Engine::Display;
    using EDD = Engine::Display::Display;
    auto& display = EDD::Instance();

    display.VirtualWidth  = 800;
	display.VirtualHeight = 600;

	const int virtualScreenWidth = display.VirtualWidth;
	const int virtualScreenHeight = display.VirtualHeight;

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
                0.0f, // Left
                0.0f, // Top
                (float)target.GetTexture().width, // Right
                -(float)target.GetTexture().height // Bottom (negative to flip vertically)
            };

            raylib::Rectangle destRect = {
                (screenWidth - (virtualScreenWidth * scale)) * 0.5f, // Center horizontally
                (screenHeight - (virtualScreenHeight * scale)) * 0.5f, // Center vertically
                virtualScreenWidth* scale, // Width scaled
                virtualScreenHeight* scale // Height scaled
            };

            auto origin_draw = raylib::Vector2{ 0, 0 };
            auto rotation_draw = 0.0f;
            auto tint_draw = raylib::Color::White();
            target.GetTexture().Draw(srcRect, destRect, origin_draw, rotation_draw, tint_draw);

        window.EndDrawing();
    }

    return 0;
}
