#include <algorithm>
#include <cmath>
#include "Engine/Display/Display.hpp"

namespace Engine {
    namespace Display {

        Display& Display::Instance() {
            static Display instance;
            return instance;
        }

        raylib::Vector2 Display::GetVirtualMousePosition() {
            raylib::Vector2 mouse = raylib::Mouse::GetPosition();

            int screenWidth = GetScreenWidth();
            int screenHeight = GetScreenHeight();

            int virtualWidth = Display::Instance().VirtualWidth;
            int virtualHeight = Display::Instance().VirtualHeight;

            if (virtualWidth <= 0 || virtualHeight <= 0) {
                return { 0.0f, 0.0f };
            }

            auto widthRatio = (float)screenWidth / (float)virtualWidth;
            auto heightRatio = (float)screenHeight / (float)virtualHeight;
            float scale = std::min(widthRatio, heightRatio);

            if (scale <= 0.0f || !std::isfinite(scale)) {
                return { 0.0f, 0.0f };
            }

            float offsetX = (screenWidth - (virtualWidth * scale)) * 0.5f;
            float offsetY = (screenHeight - (virtualHeight * scale)) * 0.5f;

            raylib::Vector2 virtualMouse = { 0.0f, 0.0f };
            virtualMouse.x = (mouse.x - offsetX) / scale;
            virtualMouse.y = (mouse.y - offsetY) / scale;

            virtualMouse.x = std::clamp(virtualMouse.x, 0.0f, (float)virtualWidth);
            virtualMouse.y = std::clamp(virtualMouse.y, 0.0f, (float)virtualHeight);

            return virtualMouse;
        }
    } // namespace Display
} // namespace Engine
