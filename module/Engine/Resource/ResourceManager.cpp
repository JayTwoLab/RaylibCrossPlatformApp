#include "Engine/Resource/ResourceManager.hpp"

namespace Engine {
    namespace Resource {

        ResourceManager& ResourceManager::Instance() {
            static ResourceManager instance;
            return instance;
        }
        void ResourceManager::SetResourcePath(const std::filesystem::path& path) {
            resourcePath = path;
        }
        std::filesystem::path ResourceManager::GetResourcePath() const {
            return resourcePath;
        }

    }
}
