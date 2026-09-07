#pragma once

#include <string>
#include <string_view>
#include <filesystem>
#include <memory>
#include <vector>
#include "raylib-cpp.hpp"

#if defined(USE_RRES)
#include "rres.h"
#endif

namespace Engine {
    namespace Resource {

        class ResourceManager {
        public:
            static ResourceManager& Instance();

            ResourceManager(const ResourceManager&) = delete;
            ResourceManager& operator=(const ResourceManager&) = delete;
            ResourceManager(ResourceManager&&) = delete;
            ResourceManager& operator=(ResourceManager&&) = delete;

            void SetResourcePath(const std::filesystem::path& path);
            std::filesystem::path GetResourcePath() const;

            bool Init();
            void Shutdown();

            // 그래픽 & 사운드 로딩
            raylib::Texture2D LoadTexture(const std::string& assetPath, unsigned int fallbackId = 0);
            raylib::Wave      LoadWave(const std::string& assetPath, unsigned int fallbackId = 0);
            raylib::Sound     LoadSound(const std::string& assetPath, unsigned int fallbackId = 0);
            raylib::Music     LoadMusic(const std::string& assetPath, unsigned int fallbackId = 0);
            std::string       LoadText(const std::string& assetPath, unsigned int fallbackId = 0);

            // 기본 폰트 로드
            raylib::Font                  LoadFont(const std::string& assetPath, unsigned int fallbackId = 0);
            std::shared_ptr<raylib::Font> LoadFontShared(const std::string& assetPath, unsigned int fallbackId = 0);

            // 한글/확장 코드포인트용 폰트 로드 (GameOverScene 한글 깨짐 대응)
            std::shared_ptr<raylib::Font> LoadFontExShared(
                const std::string& assetPath,
                int fontSize,
                const std::vector<int>& codepoints,
                unsigned int fallbackId = 0
            );

        private:
            ResourceManager();
            ~ResourceManager();

            std::filesystem::path resourcePath;
            bool isInitialized_{ false };

#if defined(USE_RRES)
            rresCentralDir centralDir_{ 0 };
            unsigned int ResolveResourceId(const std::string& assetPath, unsigned int fallbackId);
#endif
            std::string ResolveDiskPath(const std::string& assetPath) const;
        };

    } // namespace Resource
} // namespace Engine
