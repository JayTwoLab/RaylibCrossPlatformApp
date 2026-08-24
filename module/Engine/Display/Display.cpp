#include <algorithm>
#include "Engine/Scene/Scene.hpp"
#include "Engine/Scene/SceneManager.hpp"
#include "Engine/UI/ClickableAreaManager.hpp"
#include "Engine/Display/Display.hpp"

namespace Engine {
    namespace Display {

        raylib::Vector2 Display::GetVirtualMousePosition() {
            raylib::Vector2 mouse = raylib::Mouse::GetPosition();

            int screenWidth = GetScreenWidth();
            int screenHeight = GetScreenHeight();
             
			int VirtualWidth = Display::Instance().VirtualWidth;
			int VirtualHeight = Display::Instance().VirtualHeight;

            float scale = std::min((float)screenWidth / VirtualWidth, (float)screenHeight / VirtualHeight);

            float offsetX = (screenWidth - (VirtualWidth * scale)) * 0.5f;
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