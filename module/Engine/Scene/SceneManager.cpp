
#include "raylib-cpp.hpp"
#include "Engine/Scene/SceneManager.hpp"

namespace Engine {
    namespace Scene {

        void SceneManager::ChangeScene(const std::string& name) {
            auto it = sceneFactories.find(name);
            if (it == sceneFactories.end()) {
                TraceLog(LOG_WARNING, "Scene not found: %s", name.c_str());
                return;
            }

            if (currentScene) {
                currentScene->Unload();
            }

            currentScene = it->second();
            currentScene->Init();
        }

        void SceneManager::Update() {
            if (!currentScene) return;

            currentScene->Update();

            std::string next = currentScene->GetNextScene();
            if (!next.empty()) {
                ChangeScene(next);
            }
        }

        void SceneManager::Draw() {
            if (currentScene) {
                currentScene->Draw();
            }
        }

    } // namespace Scene
} // namespace Engine