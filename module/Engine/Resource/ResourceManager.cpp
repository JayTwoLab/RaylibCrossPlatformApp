#if defined(USE_RRES)
#define RRES_IMPLEMENTATION
#include "rres.h"
#define RRES_RAYLIB_IMPLEMENTATION
#include "rres-raylib.h"
#endif

#include "Engine/Resource/ResourceManager.hpp"

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
                resourcePath = "assets.rres";
            }
            std::string pkgStr = resourcePath.string();
            centralDir_ = rresLoadCentralDirectory(pkgStr.c_str());
            isInitialized_ = (centralDir_.count > 0);

            if (!isInitialized_) {
                TraceLog(LOG_ERROR, "RRES: [%s] Central Directory 로드 실패", pkgStr.c_str());
                return false;
            }
            TraceLog(LOG_INFO, "RRES: 패키지 로드 완료 (%u entries)", centralDir_.count);
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
            if (resourcePath.empty()) return assetPath;
            std::filesystem::path p(assetPath);
            if (p.is_absolute()) return assetPath;
            return (resourcePath / p).lexically_normal().string();
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
                TraceLog(LOG_WARNING, "RRES: 텍스처 에셋 ID를 찾을 수 없음: %s", assetPath.c_str());
                return raylib::Texture2D();
            }

            rresResourceChunk chunk = rresLoadResourceChunk(resourcePath.string().c_str(), id);
            UnpackResourceChunk(&chunk);
            ::Image rawImg = LoadImageFromResource(chunk);
            rresUnloadResourceChunk(chunk);

            // GPU 메모리로 업로드 후 CPU 원본 버퍼 즉시 해제
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

            // 메모리 버퍼로부터 스트리밍 Music 로드
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
