#include <algorithm>
#include <filesystem>
#include "raylib-cpp.hpp"
#include "Engine/Engine.hpp"

#include "TitleScene.hpp"
#include "GameplayScene.hpp"
#include "GameOverScene.hpp"
#include "StageClearScene.hpp"

int main(int argc, char** argv) {
#ifdef NDEBUG
    // Release 모드: 치명적 오류만 출력하거나 로깅 억제
    ::SetTraceLogLevel(LOG_FATAL);
#else
    // Debug 모드: 모든 로그 출력
    ::SetTraceLogLevel(LOG_ALL);
#endif

    namespace ED = Engine::Display;
    namespace ES = Engine::Scene;
    namespace EU = Engine::UI;
    namespace ER = Engine::Resource;

    using EDD = ED::Display;
    auto& display = EDD::Instance();

    using ERM = ER::ResourceManager;
    auto& resourceManager = ERM::Instance();

    // =========================================================================
    // 리소스 경로 설정 (USE_RRES 플래그 유무에 따른 자동 분기)
    // =========================================================================
#if defined(USE_RRES)
    // rres 패키지 모드: 단일 아카이브 파일 경로 설정
#ifdef NDEBUG
    ::ChangeDirectory(::GetApplicationDirectory());
    resourceManager.SetResourcePath("resources.rres");
#else
    auto currentPrjDir = CURRENT_PROJECT_DIR;
    std::filesystem::path pkgPath = std::filesystem::path(currentPrjDir) / "resources.rres";
    resourceManager.SetResourcePath(pkgPath);
#endif
#else
    // 일반 디스크 모드: resources 폴더 경로 설정
#ifdef NDEBUG
    ::ChangeDirectory(::GetApplicationDirectory());
    std::string resPathName = "resources";
    resourceManager.SetResourcePath(resPathName);
#else
    std::string resPathName = "resources";
    auto currentPrjDir = CURRENT_PROJECT_DIR;
    std::filesystem::path resPath = std::filesystem::path(currentPrjDir) / resPathName;
    resourceManager.SetResourcePath(resPath);
#endif
#endif

    // 오디오 디바이스 초기화
    raylib::AudioDevice audioDevice;
    if (!::IsAudioDeviceReady()) {
#ifdef __ANDROID__
        TraceLog(LOG_ERROR, "Failed to initialize audio device on Android!");
        TraceLog(LOG_WARNING, "Check microphone permissions in AndroidManifest.xml");
#elif defined(_WIN32)
        TraceLog(LOG_ERROR, "Failed to initialize audio device on Windows!");
#elif defined(__APPLE__)
        TraceLog(LOG_ERROR, "Failed to initialize audio device on macOS!");
#else
        TraceLog(LOG_ERROR, "Failed to initialize audio device!");
#endif
        return -1;
    }
    auto masterVolume = 0.5f;
    ::SetMasterVolume(masterVolume);

    // 가상 해상도 설정 (16:9 기준)
    display.VirtualWidth = 1280;
    display.VirtualHeight = 720;

    const int virtualScreenWidth = display.VirtualWidth;
    const int virtualScreenHeight = display.VirtualHeight;

    unsigned int configFlags = FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT;
    ::SetConfigFlags(configFlags);

    // 윈도우 생성
    std::string windowName = "Raylib-CPP Scalable App";
    raylib::Window window(virtualScreenWidth, virtualScreenHeight, windowName.c_str());

    int framePerSecond = 60;
    ::SetTargetFPS(framePerSecond);

    // 가상 해상도 렌더 텍스처
    raylib::RenderTexture2D target(virtualScreenWidth, virtualScreenHeight);
    ::SetTextureFilter(target.GetTexture(), TEXTURE_FILTER_BILINEAR);

    // 씬 매니저 초기화 및 씬 등록
    ES::SceneManager sceneManager;
    sceneManager.RegisterScene<TitleScene>("Title");
    sceneManager.RegisterScene<GameplayScene>("Gameplay");
    sceneManager.RegisterScene<GameOverScene>("GameOver");
    sceneManager.RegisterScene<StageClearScene>("StageClear");

    sceneManager.ChangeScene("Title");

    // ESC 키로 즉시 종료되는 기본 동작 방지
    SetExitKey(KEY_NULL);

    // 메인 루프
    while (!window.ShouldClose()) {
        const int screenWidth = GetScreenWidth();
        const int screenHeight = GetScreenHeight();

        // 화면 비율 계산 (레터박스 스케일링)
        auto scaleX = static_cast<float>(screenWidth) / static_cast<float>(virtualScreenWidth);
        auto scaleY = static_cast<float>(screenHeight) / static_cast<float>(virtualScreenHeight);
        float scale = std::min(scaleX, scaleY);

        // [1] 씬 로직 업데이트
        sceneManager.Update();

        // [2] 가상 캔버스(RenderTexture)에 렌더링
        target.BeginMode();
        {
            sceneManager.Draw();
        }
        target.EndMode();

        // [3] 레터박스 적용 후 실제 윈도우 화면에 그리기
        window.BeginDrawing();
        {
            raylib::Color::Black().ClearBackground();

            raylib::Rectangle srcRect = {
                0.0f,
                0.0f,
                static_cast<float>(target.GetTexture().width),
                -static_cast<float>(target.GetTexture().height) // OpenGL 좌표계 뒤집힘 보정
            };

            raylib::Rectangle destRect = {
                (screenWidth - (virtualScreenWidth * scale)) * 0.5f,
                (screenHeight - (virtualScreenHeight * scale)) * 0.5f,
                virtualScreenWidth * scale,
                virtualScreenHeight * scale
            };

            auto origin_draw = raylib::Vector2{ 0, 0 };
            auto rotation_draw = 0.0f;
            auto tint_draw = raylib::Color::White();
            target.GetTexture().Draw(srcRect, destRect, origin_draw, rotation_draw, tint_draw);
        }
        window.EndDrawing();
    }

    return 0;
}
