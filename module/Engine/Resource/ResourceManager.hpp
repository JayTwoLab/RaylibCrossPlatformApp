#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <functional>
#include <filesystem>
#include "raylib-cpp.hpp"

namespace Engine {
    namespace Resource {

        class ResourceManager {
        public:
            static ResourceManager& Instance();

            void SetResourcePath(const std::filesystem::path& path);
            std::filesystem::path GetResourcePath() const;
        private:
            std::filesystem::path resourcePath;
        };

    }
}
