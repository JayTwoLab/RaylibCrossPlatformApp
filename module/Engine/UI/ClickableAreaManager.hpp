#pragma once

#include <string>
#include <vector>
#include <functional>
#include "raylib-cpp.hpp"

namespace Engine {
    namespace UI {

        struct ClickableRegion {
            std::string id;
            raylib::Rectangle bounds;
            std::function<void()> onClick;
            bool isHovered;
        };

        class ClickableAreaManager {
        private:
            std::vector<ClickableRegion> regions;

        public:
            void AddRegion(const std::string& id, const raylib::Rectangle& bounds, std::function<void()> onClick);
            void Clear();
            void Update();
            void DrawDebug() const;
        };

    } // namespace UI
} // namespace Engine