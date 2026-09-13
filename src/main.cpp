#include <algorithm>
#include <filesystem>
#include "raylib-cpp.hpp"
#include "Engine/Engine.hpp"
#include "Engine/UI/rlImGui.hpp"

#include "TitleScene.hpp"
#include "GameplayScene.hpp"
#include "GameOverScene.hpp"
#include "StageClearScene.hpp"

int main(int argc, char** argv) {
#ifdef NDEBUG
    ::SetTraceLogLevel(LOG_FATAL);
#else
    ::SetTraceLogLevel(LOG_ALL);
#endif

    namespace ED = Engine::Display;
    namespace ES = Engine::Scene;
    namespace EU = Engine::UI;
    namespace ER = Engine::Resource;

    auto& display = ED::Display::Instance();
    auto& resourceManager = ER::ResourceManager::Instance();

#if defined(USE_RRES)
#   ifdef NDEBUG
    ::ChangeDirectory(::GetApplicationDirectory());
    resourceManager.SetResourcePath("resources.rres");
#   else
    auto currentPrjDir = CURRENT_PROJECT_DIR;
    std::filesystem::path pkgPath = std::filesystem::path(currentPrjDir) / "resources.rres";
    resourceManager.SetResourcePath(pkgPath);
#   endif
#else
#   ifdef NDEBUG
    ::ChangeDirectory(::GetApplicationDirectory());
    std::string resPathName = "resources";
    resourceManager.SetResourcePath(resPathName);
#   else
    std::string resPathName = "resources";
    auto currentPrjDir = CURRENT_PROJECT_DIR;
    std::filesystem::path resPath = std::filesystem::path(currentPrjDir) / resPathName;
    resourceManager.SetResourcePath(resPath);
#   endif
#endif

    raylib::AudioDevice audioDevice;
    if (!::IsAudioDeviceReady()) {
        TraceLog(LOG_ERROR, "Failed to initialize AudioDevice");
        return -1;
    }
    ::SetMasterVolume(0.5f);

    display.VirtualWidth = 1280;
    display.VirtualHeight = 720;

    const int virtualScreenWidth = display.VirtualWidth;
    const int virtualScreenHeight = display.VirtualHeight;

    ::SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
    raylib::Window window(virtualScreenWidth, virtualScreenHeight, "Raylib-CPP Scalable App");
    ::SetTargetFPS(60);

    // 한글 폰트 설정
    std::filesystem::path koreanFontDiskPath = resourceManager.GetResourcePath() / "NanumGothicBold.ttf";
    EU::SetupImGui(koreanFontDiskPath.string(), 18.0f, true);

    raylib::RenderTexture2D target(virtualScreenWidth, virtualScreenHeight);
    ::SetTextureFilter(target.GetTexture(), TEXTURE_FILTER_BILINEAR);

    ES::SceneManager sceneManager;
    sceneManager.RegisterScene<TitleScene>("Title");
    sceneManager.RegisterScene<GameplayScene>("Gameplay");
    sceneManager.RegisterScene<GameOverScene>("GameOver");
    sceneManager.RegisterScene<StageClearScene>("StageClear");

    sceneManager.ChangeScene("Title");

    SetExitKey(KEY_NULL);

    // 메인 루프
    while (!window.ShouldClose()) {
        const int screenWidth = GetScreenWidth();
        const int screenHeight = GetScreenHeight();

        float scaleX = static_cast<float>(screenWidth) / static_cast<float>(virtualScreenWidth);
        float scaleY = static_cast<float>(screenHeight) / static_cast<float>(virtualScreenHeight);
        float scale = std::min(scaleX, scaleY);

        target.BeginMode();
        {
            EU::BeginImGui();
            {
                sceneManager.Update();
                sceneManager.Draw();
                sceneManager.DrawImGui();
            }
            EU::EndImGui();
        }
        target.EndMode();

        window.BeginDrawing();
        {
            raylib::Color::Black().ClearBackground();

            raylib::Rectangle srcRect = {
                0.0f,
                0.0f,
                static_cast<float>(target.GetTexture().width),
                -static_cast<float>(target.GetTexture().height)
            };

            raylib::Rectangle destRect = {
                (screenWidth - (virtualScreenWidth * scale)) * 0.5f,
                (screenHeight - (virtualScreenHeight * scale)) * 0.5f,
                virtualScreenWidth * scale,
                virtualScreenHeight * scale
            };

            target.GetTexture().Draw(srcRect, destRect, { 0.0f, 0.0f }, 0.0f, raylib::Color::White());
        }
        window.EndDrawing();
    }

    EU::ShutdownImGui();

    return 0;
}
