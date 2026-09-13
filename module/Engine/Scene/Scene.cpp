#include "Engine/Scene/Scene.hpp"
#include "Engine/Resource/ResourceManager.hpp"

namespace Engine {
    namespace Scene {

        void Scene::SetSceneFont(std::shared_ptr<raylib::Font> font) {
            sceneFont_ = font;
        }

        void Scene::SetSceneFont(const std::filesystem::path& fontPath) {
            if (!fontPath.empty()) {
                auto& rm = Engine::Resource::ResourceManager::Instance();
                sceneFont_ = rm.LoadFontShared(fontPath.string());
            }
        }

        void Scene::SetSceneFont(const std::filesystem::path& fontPath, int fontSize, const std::vector<int>& codepoints) {
            if (!fontPath.empty()) {
                auto& rm = Engine::Resource::ResourceManager::Instance();
                sceneFont_ = rm.LoadFontExShared(fontPath.string(), fontSize, codepoints);
            }
        }

        std::shared_ptr<raylib::Font> Scene::GetSceneFont() const {
            return sceneFont_;
        }

        std::vector<int> Scene::GetKoreanCodePoints() {
            std::vector<int> codepoints;
            codepoints.reserve(95 + 11172);
            for (int i = 32; i <= 126; ++i) {
                codepoints.push_back(i);
            }
            for (int i = 0xAC00; i <= 0xD7A3; ++i) {
                codepoints.push_back(i);
            }
            return codepoints;
        }

    } // namespace Scene
} // namespace Engine
