#pragma once

#include <string>
#include "raylib-cpp.hpp"

namespace Engine {
    namespace Scene {

        class Scene {
        public:
            virtual ~Scene() = default;

            virtual void Init() = 0;
            virtual void Update() = 0;
            virtual void Draw() = 0;
            virtual void Unload() = 0;

            virtual std::string GetNextScene() const;
        };

    } // namespace Scene
} // namespace Engine