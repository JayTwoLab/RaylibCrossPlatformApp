#include <algorithm>
#include <filesystem>
#include "raylib-cpp.hpp" // raylib-cpp 
#include "Engine/Engine.hpp" // module

#include "TitleScene.hpp"
#include "GameplayScene.hpp"
#include "GameOverScene.hpp"
#include "StageClearScene.hpp"

#include "GenerateTestAssets.hpp"

int main() {
#ifdef NDEBUG
    // 릴리즈 모드: 치명적인 에러만 출력하거나 아예 끔
    SetTraceLogLevel(LOG_FATAL); 
    // SetTraceLogLevel(LOG_NONE); // 로그 끔
#else
    // 디버그 모드: 모든 정보/경고/에러 출력
    SetTraceLogLevel(LOG_ALL);   // 또는 LOG_INFO
#endif

    namespace ED = Engine::Display;
    namespace ES = Engine::Scene;
    namespace EU = Engine::UI;
    namespace ER = Engine::Resource;

    using EDD = ED::Display;
    auto& display = EDD::Instance();
    using ERM = ER::ResourceManager;
    auto& resourceManager = ERM::Instance();

#ifdef NDEBUG
    std::string resPathName = "resources";
    std::filesystem::path resPath = resPathName;
    resourceManager.SetResourcePath(resPath);
#else
    std::string resPathName = "resources";
    auto currentPrjDir = CURRENT_PROJECT_DIR; // 현재 CMakeLists.txt 의 경로
    std::filesystem::path resPath = std::filesystem::path(currentPrjDir) / resPathName;
    resourceManager.SetResourcePath(resPath);
#endif

    raylib::AudioDevice audioDevice; // 또는 InitAudioDevice();
    if (!IsAudioDeviceReady()) {
        TraceLog(LOG_ERROR, "오디오 장치 초기화 실패!");
        return -1;
    }

    // 가상 화면 크기 설정
    display.VirtualWidth  = 800;
	display.VirtualHeight = 600;

	const int virtualScreenWidth  = display.VirtualWidth;
	const int virtualScreenHeight = display.VirtualHeight;

    auto configFlags =
        FLAG_WINDOW_RESIZABLE | // 윈도우 크기 조절 가능
        FLAG_VSYNC_HINT; // 수직 동기화 활성화
    SetConfigFlags(configFlags);

    auto windowName = "Raylib-CPP Scalable App";
    raylib::Window window(virtualScreenWidth, virtualScreenHeight, windowName);
    SetTargetFPS(60);

    // 테스트용 PNG 파일 3종 자동 생성 (한 번 생성되면 resources 폴더에 저장됨)
    // TestUtils::GenerateAssets();

    raylib::RenderTexture2D target(virtualScreenWidth, virtualScreenHeight);
    auto textureFilter = TEXTURE_FILTER_BILINEAR; // 선형 필터링 (bilinear filtering)
    SetTextureFilter(target.GetTexture(), textureFilter);

    ES::SceneManager sceneManager; // 씬 관리자

    // Scene 등록
    sceneManager.RegisterScene<TitleScene>("Title");
    sceneManager.RegisterScene<GameplayScene>("Gameplay");
    sceneManager.RegisterScene<GameOverScene>("GameOver");
    sceneManager.RegisterScene<StageClearScene>("StageClear");

    sceneManager.ChangeScene("Title"); // 최초 씬 설정    

    while (!window.ShouldClose()) {
        // 현재 화면 크기 가져오기
        const int screenWidth  = GetScreenWidth();
        const int screenHeight = GetScreenHeight();

        // 가상 화면 크기에 맞춰 스케일 계산
        auto scaleX = (float)screenWidth  / (float)virtualScreenWidth;
		auto scaleY = (float)screenHeight / (float)virtualScreenHeight;
        float scale = std::min(scaleX, scaleY);

        // [1] 씬 로직 업데이트 (ClickableAreaManager 내부에서 보정 좌표 자동 사용)
        //  NOTE: 씬 매니저의 Update() 가 Draw() 보다 먼저 호출됨.
        sceneManager.Update();

        // [2] 가상 렌더 텍스처에 그리기
        target.BeginMode();
            sceneManager.Draw();
        target.EndMode();

        // [3] 레터박스 적용 후 화면에 렌더링
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

            // Draw the render texture to the screen with scaling and letterboxing
            auto origin_draw = raylib::Vector2{ 0, 0 };
            auto rotation_draw = 0.0f;
            auto tint_draw = raylib::Color::White();
            target.GetTexture().Draw(srcRect, destRect, origin_draw, rotation_draw, tint_draw);

        window.EndDrawing();
    }

    return 0;
}
