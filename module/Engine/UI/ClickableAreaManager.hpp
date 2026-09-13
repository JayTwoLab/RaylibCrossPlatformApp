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
            std::function<void()> onClick{ nullptr };
            int zOrder{ 0 };
            bool enabled{ true };
            bool isHovered{ false };
            bool isPressed{ false };

            std::shared_ptr<Engine::Graphics::RotatingSprite> sprite{ nullptr };
            std::string text{ "" };
            raylib::Color textColor{ raylib::Color::Black() };
            raylib::Color backgroundColor{ raylib::Color::White() };
            int fontSize{ 20 };
            bool hasBackground{ false };
            std::shared_ptr<raylib::Font> font{ nullptr };
            bool useCustomFont{ false };
        };

        class ClickableAreaManager {
        private:
            std::vector<ClickableRegion> regions;
            bool isSorted_{ true };

            ClickableRegion* FindRegionById(const std::string& id);
            void SortRegionsIfNeeded();

        public:
            void AddRegion(const std::string& id, const raylib::Rectangle& bounds, std::function<void()> onClick = nullptr, int zOrder = 0);
            void RemoveRegion(const std::string& id);
            void SetRegionSprite(const std::string& id, std::shared_ptr<Engine::Graphics::RotatingSprite> sprite);
            void SetRegionPosition(const std::string& id, const raylib::Vector2& newPosition);
            void SetRegionPosition(const std::string& id, float x, float y);
            void SetRegionZOrder(const std::string& id, int zOrder);
            void SetRegionEnabled(const std::string& id, bool enabled);

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
