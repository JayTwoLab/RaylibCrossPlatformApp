#pragma once

#include "raylib-cpp.hpp"

namespace Engine {
    namespace Display {
        class Display {
        public:
            static Display& Instance();

            Display(const Display&) = delete;
            Display& operator=(const Display&) = delete;
            Display(Display&&) = delete;
            Display& operator=(Display&&) = delete;

            int VirtualWidth;
            int VirtualHeight;

            raylib::Vector2 GetVirtualMousePosition();

        private:
            Display() : VirtualWidth(0), VirtualHeight(0) {}
            ~Display() = default;
        };
    } // namespace Display
} // namespace Engine
