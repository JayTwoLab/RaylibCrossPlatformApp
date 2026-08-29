#include <algorithm>
#include "Engine/Scene/Scene.hpp"
#include "Engine/Scene/SceneManager.hpp"
#include "Engine/UI/ClickableAreaManager.hpp"
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
             
			int VirtualWidth = Display::Instance().VirtualWidth;
			int VirtualHeight = Display::Instance().VirtualHeight;

            if (VirtualWidth <= 0 || VirtualHeight <= 0) {
                return { 0.0f, 0.0f };
            }

            auto widthRatio = (float)screenWidth / (float)VirtualWidth;
            auto heightRatio = (float)screenHeight / (float)VirtualHeight;
            float scale = std::min(widthRatio, heightRatio);

            if (scale <= 0.0f || !std::isfinite(scale)) {
                return { 0.0f, 0.0f };
            }

            float offsetX = (screenWidth  - (VirtualWidth  * scale)) * 0.5f;
            float offsetY = (screenHeight - (VirtualHeight * scale)) * 0.5f;

            raylib::Vector2 virtualMouse = { 0.0f, 0.0f };
            virtualMouse.x = (mouse.x - offsetX) / scale;
            virtualMouse.y = (mouse.y - offsetY) / scale;

            virtualMouse.x = std::clamp(virtualMouse.x, 0.0f, (float)VirtualWidth);
            virtualMouse.y = std::clamp(virtualMouse.y, 0.0f, (float)VirtualHeight);

            return virtualMouse;
        }
    }
}
