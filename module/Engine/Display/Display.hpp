#pragma once

#include "raylib-cpp.hpp"

namespace Engine {
    namespace Display {
        class Display {
        public:
            // Access the singleton instance
            static Display& Instance() {
                static Display instance;
                return instance;
            }

            // Non-copyable, non-movable
            Display(const Display&) = delete;
            Display& operator=(const Display&) = delete;
            Display(Display&&) = delete;
            Display& operator=(Display&&) = delete;

            int VirtualWidth;
            int VirtualHeight;
             
            raylib::Vector2 GetVirtualMousePosition();

        private:
            // Private ctor ensures only Instance() can create the object
            Display() : VirtualWidth(0), VirtualHeight(0) {}
            ~Display() = default;
        };

    } // namespace Display
} // namespace Engine