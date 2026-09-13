#pragma once

#include <string>
#include <memory>
#include <filesystem>
#include <vector>
#include "raylib-cpp.hpp"

namespace Engine {
    namespace Scene {

        class Scene {
        public:
            Scene() = default;
            virtual ~Scene() = default;

            // Scene lifecycle functions
            virtual void Init() = 0; // Initialize the scene
            virtual void Unload() = 0; // Unload the scene

            // Loop functions
            virtual void Update() = 0; // Update the scene logic
            virtual void Draw() = 0; // Draw the scene
            virtual void DrawImGui() {} // Draw ImGui debugger

            // Scene transition functions
            virtual std::string GetNextScene() = 0;

            // Scene font setting functions
            virtual void SetSceneFont(std::shared_ptr<raylib::Font> font);
            virtual void SetSceneFont(const std::filesystem::path& fontPath);
            virtual void SetSceneFont(const std::filesystem::path& fontPath, int fontSize, const std::vector<int>& codepoints = {});
            virtual std::shared_ptr<raylib::Font> GetSceneFont() const;
            static std::vector<int> GetKoreanCodePoints();

        protected:
            std::string nextScene_{ "" }; // Next scene name
            std::shared_ptr<raylib::Font> sceneFont_{ nullptr }; // Scene font object
        };

    } // namespace Scene
} // namespace Engine
