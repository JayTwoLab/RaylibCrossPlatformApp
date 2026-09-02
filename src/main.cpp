#include <algorithm>
#include <filesystem>
#include "raylib-cpp.hpp" // raylib-cpp 
#include "Engine/Engine.hpp" // module

#include "TitleScene.hpp"
#include "GameplayScene.hpp"
#include "GameOverScene.hpp"
#include "StageClearScene.hpp"

int main(int argc, char** argv) {
#ifdef NDEBUG
    // Release mode: output only fatal errors or disable logging entirely
    ::SetTraceLogLevel(LOG_FATAL);
    // ::SetTraceLogLevel(LOG_NONE); // disable logs
#else
    // Debug mode: output all info/warnings/errors
    ::SetTraceLogLevel(LOG_ALL);   // or LOG_INFO
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
    // Release mode
    ::ChangeDirectory(::GetApplicationDirectory()); // Change working directory to executable location

    std::string resPathName = "resources";
    std::filesystem::path resPath = resPathName;
        // auto currentPrjDir = CURRENT_PROJECT_DIR;
        // resPath = std::filesystem::path(currentPrjDir) / resPathName;
    resourceManager.SetResourcePath(resPath);
#else
    // Debug mode
    std::string resPathName = "resources";
    auto currentPrjDir = CURRENT_PROJECT_DIR; // current CMakeLists.txt path
    std::filesystem::path resPath = std::filesystem::path(currentPrjDir) / resPathName;
    resourceManager.SetResourcePath(resPath);
#endif

    raylib::AudioDevice audioDevice; // Initialize audio device
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
    ::SetMasterVolume(masterVolume); // Set master volume to 50%

    // Set virtual screen size
    display.VirtualWidth  = 800;
    display.VirtualHeight = 600;

    const int virtualScreenWidth  = display.VirtualWidth;
    const int virtualScreenHeight = display.VirtualHeight;

    auto configFlags =
        FLAG_WINDOW_RESIZABLE | // Allow window resizing
        FLAG_VSYNC_HINT; // Enable vertical sync
    ::SetConfigFlags(configFlags);

    // Create window
    auto windowName = "Raylib-CPP Scalable App";
    raylib::Window window(virtualScreenWidth, virtualScreenHeight, windowName);

    int framePerSecond = 60;
    ::SetTargetFPS(framePerSecond);

    raylib::RenderTexture2D target(virtualScreenWidth, virtualScreenHeight);
    auto textureFilter = TEXTURE_FILTER_BILINEAR; // linear filtering (bilinear filtering)
    ::SetTextureFilter(target.GetTexture(), textureFilter);

    ES::SceneManager sceneManager; // Scene manager

    // Register scenes
    sceneManager.RegisterScene<TitleScene>("Title");
    sceneManager.RegisterScene<GameplayScene>("Gameplay");
    sceneManager.RegisterScene<GameOverScene>("GameOver");
    sceneManager.RegisterScene<StageClearScene>("StageClear");

    sceneManager.ChangeScene("Title"); // Set initial scene    

    while (!window.ShouldClose()) {
        // Get current screen size
        const int screenWidth  = GetScreenWidth();
        const int screenHeight = GetScreenHeight();

        // Calculate scale to match virtual screen size
        auto scaleX = (float)screenWidth  / (float)virtualScreenWidth;
        auto scaleY = (float)screenHeight / (float)virtualScreenHeight;
        float scale = std::min(scaleX, scaleY);

        // [1] Update scene logic (ClickableAreaManager internally uses corrected coordinates automatically)
        //  NOTE: SceneManager's Update() is called before Draw().
        sceneManager.Update();

        // [2] Draw to virtual render texture
        target.BeginMode();
            sceneManager.Draw();
        target.EndMode();

        // [3] Apply letterboxing and render to screen
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
