#pragma once

#include <string>
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
            virtual void Update() = 0; // Update the scene logic. Upload() is called before Draw(). 
            virtual void Draw() = 0; // Draw the scene

            // Scene transition functions
            virtual std::string GetNextScene() = 0;

        protected:
            std::string nextScene_{""}; // Next scene name

        };

    } // namespace Scene
} // namespace Engine
