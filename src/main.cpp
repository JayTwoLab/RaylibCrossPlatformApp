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
    // display.VirtualWidth  = 800; display.VirtualHeight = 600; // ratio 4:3
    // display.VirtualWidth = 1024; display.VirtualHeight = 768; // ratio 4:3
    display.VirtualWidth = 1280; display.VirtualHeight = 720; // ratio 16:9
    // display.VirtualWidth = 1920; display.VirtualHeight = 1080; // Full HD resolution // ratio 16:9
    // display.VirtualWidth = 3840; display.VirtualHeight = 2160; // 4K resolution // ratio 16:9

    const int virtualScreenWidth  = display.VirtualWidth;
    const int virtualScreenHeight = display.VirtualHeight;

    unsigned int configFlags =
        FLAG_WINDOW_RESIZABLE | // Allow window resizing
       //  FLAG_FULLSCREEN_MODE | // Start in fullscreen mode
        FLAG_VSYNC_HINT; // Enable vertical sync
    // 
    // FLAG_VSYNC_HINT           // Set to try enabling V-Sync on GPU
    // FLAG_FULLSCREEN_MODE      // Set to run program in fullscreen
    // FLAG_WINDOW_RESIZABLE     // Set to allow resizable window
    // FLAG_WINDOW_UNDECORATED   // Set to disable window decoration (frame and buttons)
    // FLAG_WINDOW_HIDDEN        // Set to hide window
    // FLAG_WINDOW_MINIMIZED     // Set to minimize window (iconify)
    // FLAG_WINDOW_MAXIMIZED     // Set to maximize window (expanded to monitor)
    // FLAG_WINDOW_UNFOCUSED     // Set to window non focused
    // FLAG_WINDOW_TOPMOST       // Set to window always on top
    // FLAG_WINDOW_ALWAYS_RUN    // Set to allow windows running while minimized
    // FLAG_WINDOW_TRANSPARENT    // Set to allow transparent framebuffer
    // FLAG_WINDOW_HIGHDPI        // Set to support HighDPI
    // FLAG_WINDOW_MOUSE_PASSTHROUGH // Set to support mouse passthrough, only supported when  G_WINDOW_UNDECORATED
    // FLAG_BORDERLESS_WINDOWED_MODE // Set to run program in borderless windowed mode
    // FLAG_MSAA_4X_HINT           // Set to try enabling MSAA 4X
    // FLAG_INTERLACED_HINT        // Set to try enabling interlaced video format (for V3D)
    ::SetConfigFlags(configFlags);

    // Create window
    std::string windowName = "Raylib-CPP Scalable App";
    raylib::Window window(virtualScreenWidth, virtualScreenHeight, windowName.c_str());

    int framePerSecond = 60;
    ::SetTargetFPS(framePerSecond);

    raylib::RenderTexture2D target(virtualScreenWidth, virtualScreenHeight);

    int textureFilter = TEXTURE_FILTER_BILINEAR; // linear filtering (bilinear filtering)
    //  TEXTURE_FILTER_BILINEAR        // Linear filtering
    //  TEXTURE_FILTER_TRILINEAR       // Trilinear filtering (linear with mipmaps)
    //  TEXTURE_FILTER_ANISOTROPIC_4X  // Anisotropic filtering 4x
    //  TEXTURE_FILTER_ANISOTROPIC_8X  // Anisotropic filtering 8x
    //  TEXTURE_FILTER_ANISOTROPIC_16X // Anisotropic filtering 16x
    ::SetTextureFilter(target.GetTexture(), textureFilter);

    ES::SceneManager sceneManager; // Scene manager

    // Register scenes
    sceneManager.RegisterScene<TitleScene>("Title");
    sceneManager.RegisterScene<GameplayScene>("Gameplay");
    sceneManager.RegisterScene<GameOverScene>("GameOver");
    sceneManager.RegisterScene<StageClearScene>("StageClear");

    sceneManager.ChangeScene("Title"); // Set initial scene

    // Skip ESC key to exit the application (to avoid accidental exits)
    SetExitKey(KEY_NULL);

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
        {
            sceneManager.Draw();
        }
        target.EndMode();

        // [3] Apply letterboxing and render to screen
        window.BeginDrawing();
        {
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
                virtualScreenWidth * scale, // Width scaled
                virtualScreenHeight * scale // Height scaled
            };

            // Draw the render texture to the screen with scaling and letterboxing
            auto origin_draw = raylib::Vector2{ 0, 0 };
            auto rotation_draw = 0.0f;
            auto tint_draw = raylib::Color::White();
            target.GetTexture().Draw(srcRect, destRect, origin_draw, rotation_draw, tint_draw);

            if (IsKeyPressed(KEY_ESCAPE)) {
                // ESC key pressed:
                TraceLog(LOG_INFO, "ESC key pressed...");
            }

        }
        window.EndDrawing();
    }

    return 0;
}
