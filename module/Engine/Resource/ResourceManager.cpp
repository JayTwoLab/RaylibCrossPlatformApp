#if defined(USE_RRES)
#define RRES_IMPLEMENTATION
#include "rres.h"
#define RRES_RAYLIB_IMPLEMENTATION
#include "rres-raylib.h"
#endif

#include "Engine/Resource/ResourceManager.hpp"

// Platform identification
#if defined(_WIN32)
#define PLATFORM_WINDOWS
#elif defined(__ANDROID__)
#define PLATFORM_ANDROID
#elif defined(__APPLE__)
#include <TargetConditionals.h>
#if TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR
#define PLATFORM_IOS
#else
#define PLATFORM_MACOS
#endif
#elif defined(__linux__)
#define PLATFORM_LINUX
#endif

namespace Engine {
    namespace Resource {

        ResourceManager& ResourceManager::Instance() {
            static ResourceManager instance;
            return instance;
        }

        ResourceManager::ResourceManager() {
            Init();
        }

        ResourceManager::~ResourceManager() {
            Shutdown();
        }

        void ResourceManager::SetResourcePath(const std::filesystem::path& path) {
            resourcePath = path;
#if defined(USE_RRES)
            if (isInitialized_) {
                Shutdown();
                Init();
            }
#endif
        }

        std::filesystem::path ResourceManager::GetResourcePath() const {
            return resourcePath;
        }

        bool ResourceManager::Init() {
#if defined(USE_RRES)
            if (resourcePath.empty()) {
#if defined(PLATFORM_WINDOWS) || defined(PLATFORM_LINUX)
                resourcePath = "assets.rres";
#elif defined(PLATFORM_MACOS)
                // macOS app bundle: check Contents/Resources first, fallback to executable directory
                std::filesystem::path bundleRes = std::filesystem::path(::GetApplicationDirectory()) / "Contents" / "Resources" / "assets.rres";
                if (std::filesystem::exists(bundleRes)) {
                    resourcePath = bundleRes;
                }
                else {
                    resourcePath = std::filesystem::path(::GetApplicationDirectory()) / "assets.rres";
                }
#elif defined(PLATFORM_IOS) || defined(PLATFORM_ANDROID)
                // Sandboxed mobile environments require the application directory prefix
                resourcePath = std::filesystem::path(::GetApplicationDirectory()) / "assets.rres";
#else
                resourcePath = "assets.rres";
#endif
            }

            std::string pkgStr = resourcePath.string();
            centralDir_ = rresLoadCentralDirectory(pkgStr.c_str());
            isInitialized_ = (centralDir_.count > 0);

            if (!isInitialized_) {
                TraceLog(LOG_ERROR, "RRES: Failed to load Central Directory from [%s]", pkgStr.c_str());
                return false;
            }
            TraceLog(LOG_INFO, "RRES: Package loaded successfully (%u entries)", centralDir_.count);
            return true;
#else
            isInitialized_ = true;
            return true;
#endif
        }

        void ResourceManager::Shutdown() {
#if defined(USE_RRES)
            if (isInitialized_ && centralDir_.entries != nullptr) {
                rresUnloadCentralDirectory(centralDir_);
                centralDir_ = { 0 };
            }
#endif
            isInitialized_ = false;
        }

        std::string ResourceManager::ResolveDiskPath(const std::string& assetPath) const {
            if (assetPath.empty()) return {};

            std::filesystem::path p(assetPath);
            if (p.is_absolute()) return assetPath;

            std::filesystem::path basePath = resourcePath;

            // When no custom base path is specified, use platform-native asset directory
            if (basePath.empty()) {
#if defined(PLATFORM_WINDOWS) || defined(PLATFORM_LINUX)
                basePath = std::filesystem::current_path();
#elif defined(PLATFORM_MACOS)
                std::filesystem::path bundleRes = std::filesystem::path(::GetApplicationDirectory()) / "Contents" / "Resources";
                basePath = std::filesystem::exists(bundleRes) ? bundleRes : std::filesystem::path(::GetApplicationDirectory());
#elif defined(PLATFORM_IOS) || defined(PLATFORM_ANDROID)
                basePath = std::filesystem::path(::GetApplicationDirectory());
#else
                basePath = std::filesystem::current_path();
#endif
            }

            return (basePath / p).lexically_normal().string();
        }

#if defined(USE_RRES)
        unsigned int ResourceManager::ResolveResourceId(const std::string& assetPath, unsigned int fallbackId) {
            if (!assetPath.empty() && centralDir_.entries != nullptr) {
                unsigned int id = rresGetResourceId(centralDir_, assetPath.c_str());
                if (id != 0) return id;
            }
            return fallbackId;
        }
#endif

        raylib::Texture2D ResourceManager::LoadTexture(const std::string& assetPath, unsigned int fallbackId) {
#if defined(USE_RRES)
            unsigned int id = ResolveResourceId(assetPath, fallbackId);
            if (id == 0) {
                TraceLog(LOG_WARNING, "RRES: Texture asset ID not found: %s", assetPath.c_str());
                return raylib::Texture2D();
            }

            rresResourceChunk chunk = rresLoadResourceChunk(resourcePath.string().c_str(), id);
            UnpackResourceChunk(&chunk);
            ::Image rawImg = LoadImageFromResource(chunk);
            rresUnloadResourceChunk(chunk);

            // Upload to GPU memory and immediately release CPU source buffer
            ::Texture2D rawTex = ::LoadTextureFromImage(rawImg);
            ::UnloadImage(rawImg);
            return raylib::Texture2D(rawTex);
#else
            (void)fallbackId;
            return raylib::Texture2D(ResolveDiskPath(assetPath));
#endif
        }

        raylib::Wave ResourceManager::LoadWave(const std::string& assetPath, unsigned int fallbackId) {
#if defined(USE_RRES)
            unsigned int id = ResolveResourceId(assetPath, fallbackId);
            if (id == 0) return raylib::Wave();

            rresResourceChunk chunk = rresLoadResourceChunk(resourcePath.string().c_str(), id);
            UnpackResourceChunk(&chunk);
            ::Wave rawWave = LoadWaveFromResource(chunk);
            rresUnloadResourceChunk(chunk);

            return raylib::Wave(rawWave);
#else
            (void)fallbackId;
            return raylib::Wave(ResolveDiskPath(assetPath));
#endif
        }

        raylib::Sound ResourceManager::LoadSound(const std::string& assetPath, unsigned int fallbackId) {
#if defined(USE_RRES)
            raylib::Wave wave = LoadWave(assetPath, fallbackId);
            return raylib::Sound(wave);
#else
            (void)fallbackId;
            return raylib::Sound(ResolveDiskPath(assetPath));
#endif
        }

        raylib::Music ResourceManager::LoadMusic(const std::string& assetPath, unsigned int fallbackId) {
#if defined(USE_RRES)
            unsigned int id = ResolveResourceId(assetPath, fallbackId);
            if (id == 0) return raylib::Music();

            rresResourceChunk chunk = rresLoadResourceChunk(resourcePath.string().c_str(), id);
            UnpackResourceChunk(&chunk);

            unsigned int dataSize = 0;
            void* rawData = LoadDataFromResource(chunk, &dataSize);
            std::string ext = std::filesystem::path(assetPath).extension().string();

            // Load streaming music from memory buffer
            ::Music rawMusic = ::LoadMusicStreamFromMemory(ext.c_str(), static_cast<const unsigned char*>(rawData), dataSize);
            rresUnloadResourceChunk(chunk);
            return raylib::Music(rawMusic);
#else
            (void)fallbackId;
            return raylib::Music(ResolveDiskPath(assetPath));
#endif
        }

        raylib::Font ResourceManager::LoadFont(const std::string& assetPath, unsigned int fallbackId) {
#if defined(USE_RRES)
            unsigned int id = ResolveResourceId(assetPath, fallbackId);
            if (id == 0) return raylib::Font(::GetFontDefault());

            rresResourceMulti multi = rresLoadResourceMulti(resourcePath.string().c_str(), id);
            for (unsigned int i = 0; i < multi.count; ++i) {
                UnpackResourceChunk(&multi.chunks[i]);
            }
            ::Font rawFont = LoadFontFromResource(multi);
            rresUnloadResourceMulti(multi);

            return raylib::Font(rawFont);
#else
            (void)fallbackId;
            return raylib::Font(ResolveDiskPath(assetPath));
#endif
        }

        std::shared_ptr<raylib::Font> ResourceManager::LoadFontShared(const std::string& assetPath, unsigned int fallbackId) {
            return std::make_shared<raylib::Font>(LoadFont(assetPath, fallbackId));
        }

        std::shared_ptr<raylib::Font> ResourceManager::LoadFontExShared(
            const std::string& assetPath,
            int fontSize,
            const std::vector<int>& codepoints,
            unsigned int fallbackId)
        {
#if defined(USE_RRES)
            unsigned int id = ResolveResourceId(assetPath, fallbackId);
            if (id != 0) {
                rresResourceChunk chunk = rresLoadResourceChunk(resourcePath.string().c_str(), id);
                UnpackResourceChunk(&chunk);

                unsigned int dataSize = 0;
                void* rawData = LoadDataFromResource(chunk, &dataSize);
                std::string ext = std::filesystem::path(assetPath).extension().string();

                ::Font rawFont = ::LoadFontFromMemory(
                    ext.c_str(),
                    static_cast<const unsigned char*>(rawData),
                    dataSize,
                    fontSize,
                    const_cast<int*>(codepoints.data()),
                    static_cast<int>(codepoints.size())
                );
                rresUnloadResourceChunk(chunk);
                return std::make_shared<raylib::Font>(rawFont);
            }
            return std::make_shared<raylib::Font>(::GetFontDefault());
#else
            (void)fallbackId;
            std::string fullPath = ResolveDiskPath(assetPath);
            ::Font rawFont = ::LoadFontEx(
                fullPath.c_str(),
                fontSize,
                const_cast<int*>(codepoints.data()),
                static_cast<int>(codepoints.size())
            );
            return std::make_shared<raylib::Font>(rawFont);
#endif
        }

        std::string ResourceManager::LoadText(const std::string& assetPath, unsigned int fallbackId) {
#if defined(USE_RRES)
            unsigned int id = ResolveResourceId(assetPath, fallbackId);
            if (id == 0) return {};

            rresResourceChunk chunk = rresLoadResourceChunk(resourcePath.string().c_str(), id);
            UnpackResourceChunk(&chunk);
            char* rawText = LoadTextFromResource(chunk);
            rresUnloadResourceChunk(chunk);

            if (!rawText) return {};
            std::string result(rawText);
            RL_FREE(rawText);
            return result;
#else
            (void)fallbackId;
            char* rawText = ::LoadFileText(ResolveDiskPath(assetPath).c_str());
            if (!rawText) return {};
            std::string result(rawText);
            ::UnloadFileText(rawText);
            return result;
#endif
        }

    } // namespace Resource
} // namespace Engine
