
#include "raylib-cpp.hpp"
#include "Engine/Scene/SceneManager.hpp"

namespace Engine {
    namespace Scene {

        void SceneManager::ChangeScene(const std::string& name) {
            auto it = sceneFactories_.find(name);
            if (it == sceneFactories_.end()) {
                TraceLog(LOG_WARNING, "Scene not found: %s", name.c_str());
                return;
            }

            if (currentScene_) {
                currentScene_->Unload();
            }

            currentScene_ = it->second();
            if (currentScene_) {
                currentScene_->Init();
            }
        }

        void SceneManager::Update() {
            if (!currentScene_)
                return;

            currentScene_->Update();

            std::string next = currentScene_->GetNextScene();
            if (!next.empty()) {
                ChangeScene(next);
            }
        }

        void SceneManager::Draw() {
            if (currentScene_) {
                currentScene_->Draw();
            }
        }

    } // namespace Scene
} // namespace Engine
