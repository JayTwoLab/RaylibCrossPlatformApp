#pragma once

#include <string>
#include <vector>
#include <functional>
#include <memory>
#include "raylib-cpp.hpp"
#include "Engine/Graphics/RotatingSprite.hpp"

namespace Engine {
    namespace UI {

        struct ClickableRegion {
            std::string id;
            raylib::Rectangle bounds;
            std::function<void()> onClick;
            bool isHovered;
            bool isPressed; // 추가: 마우스 클릭 유지 여부
            std::shared_ptr<Engine::Graphics::RotatingSprite> sprite;
            std::string text;
            raylib::Color textColor;
            raylib::Color backgroundColor;
            int fontSize;
            bool hasBackground;
            std::shared_ptr<raylib::Font> font;
            bool useCustomFont;
        };

        class ClickableAreaManager { 
        private:
            std::vector<ClickableRegion> regions;

            ClickableRegion* FindRegionById(const std::string& id);

        public:
            void AddRegion(const std::string& id, const raylib::Rectangle& bounds, std::function<void()> onClick);
            void SetRegionSprite(const std::string& id, std::shared_ptr<Engine::Graphics::RotatingSprite> sprite);
            void SetRegionPosition(const std::string& id, const raylib::Vector2& newPosition);
            void SetRegionPosition(const std::string& id, float x, float y);
            void SetRegionText(const std::string& id, const std::string& text, const raylib::Color& color = raylib::Color::Black(), int fontSize = 20);
            void SetRegionText(const std::string& id, const std::string& text, const raylib::Color& color, const raylib::Color& backgroundColor, int fontSize = 20);
            void SetRegionText(const std::string& id, const std::string& text, std::shared_ptr<raylib::Font> font, const raylib::Color& color, int fontSize = 20);
            void SetRegionText(const std::string& id, const std::string& text, std::shared_ptr<raylib::Font> font, const raylib::Color& color, const raylib::Color& backgroundColor, int fontSize = 20);
            void Clear();
            void Update();
            void Draw() const;
            void DrawBoundary() const;
        };

    } // namespace UI
} // namespace Engine
