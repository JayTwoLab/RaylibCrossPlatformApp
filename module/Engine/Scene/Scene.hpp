#pragma once

#include <string>
#include "raylib-cpp.hpp"

namespace Engine {
    namespace Scene {

        class Scene {
        public:
            Scene() = default;
            virtual ~Scene() = default;

            virtual void Init() = 0;
            virtual void Update() = 0;
            virtual void Draw() = 0;
            virtual void Unload() = 0;

            virtual std::string GetNextScene() = 0;

        protected:
            std::string nextScene_{""};

        };

    } // namespace Scene
} // namespace Engine
