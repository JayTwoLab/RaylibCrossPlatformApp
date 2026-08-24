#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <functional>
#include "raylib-cpp.hpp"
#include "Engine/Scene/Scene.hpp"

namespace Engine {
    namespace Scene {

        class SceneManager {
        private:
            std::unique_ptr<Scene> currentScene;
            std::unordered_map<std::string, std::function<std::unique_ptr<Scene>()>> sceneFactories;

        public:
            template <typename T>
            void RegisterScene(const std::string& name) {
                sceneFactories[name] = []() { return std::make_unique<T>(); };
            } 

            void ChangeScene(const std::string& name);
            void Update();
            void Draw();
        };

    } // namespace Scene
} // namespace Engine