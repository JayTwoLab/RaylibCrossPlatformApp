#include "Engine/UI/ClickableAreaManager.hpp"
#include "Engine/Engine.hpp"

namespace Engine {
    namespace UI {

        void ClickableAreaManager::AddRegion(const std::string& id, const raylib::Rectangle& bounds, std::function<void()> onClick) {
            regions.push_back({ id, bounds, onClick, false });
        }

        void ClickableAreaManager::Clear() {
            regions.clear();
        }

        void ClickableAreaManager::Update() { 
            // 실제 창 해상도 대신 가상 캔버스(800x450) 기준 보정된 마우스 좌표 사용

			namespace ED = Engine::Display;
			using EDD = Engine::Display::Display;

			raylib::Vector2 mousePos = EDD::Instance().GetVirtualMousePosition();
            bool isClicked = raylib::Mouse::IsButtonPressed(MOUSE_BUTTON_LEFT);

            for (auto& region : regions) {
                region.isHovered = region.bounds.CheckCollision(mousePos);

                if (region.isHovered && isClicked) {
                    if (region.onClick) {
                        region.onClick();
                    }
                    break;
                }
            }
        }

        void ClickableAreaManager::DrawDebug() const {
            for (const auto& region : regions) {
                raylib::Color color = region.isHovered ? raylib::Color::Yellow() : raylib::Color::Lime();

                raylib::Rectangle shadowBounds = {
                    region.bounds.x - 1.0f,
                    region.bounds.y - 1.0f,
                    region.bounds.width + 2.0f,
                    region.bounds.height + 2.0f
                };
                shadowBounds.DrawLines(raylib::Color::Black(), 1.0f);

                region.bounds.DrawLines(color, 2.0f);
            }
        }

    } // namespace UI
} // namespace Engine