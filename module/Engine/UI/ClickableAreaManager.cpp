#include "Engine/UI/ClickableAreaManager.hpp"
#include "Engine/Engine.hpp"

namespace Engine {
    namespace UI {

        ClickableRegion* ClickableAreaManager::FindRegionById(const std::string& id) {
            for (auto& region : regions) {
                if (region.id == id) {
                    return &region;
                }
            }
            return nullptr;
        }

        void ClickableAreaManager::AddRegion(const std::string& id, const raylib::Rectangle& bounds, std::function<void()> onClick) {
            ClickableRegion region;
            region.id = id;
            region.bounds = bounds;
            region.onClick = onClick;
            region.isHovered = false;
            region.sprite = nullptr;
            region.text = "";
            region.textColor = raylib::Color::Black();
            region.backgroundColor = raylib::Color::White();
            region.fontSize = 20;
            region.hasBackground = false;
            region.font = nullptr;
            region.useCustomFont = false;
            regions.push_back(region);
        }

        void ClickableAreaManager::SetRegionSprite(const std::string& id, std::shared_ptr<Engine::Graphics::RotatingSprite> sprite) {
            ClickableRegion* region = FindRegionById(id);
            if (!region || !sprite) {
                return;
            }

            region->sprite = sprite;

            raylib::Vector2 centerPos = {
                region->bounds.x + region->bounds.width / 2.0f,
                region->bounds.y + region->bounds.height / 2.0f
            };
            sprite->SetPosition(centerPos);

            raylib::Rectangle spriteBounds = sprite->GetBounds();
            if (spriteBounds.width > 0 && spriteBounds.height > 0) {
                float scaleX = region->bounds.width / spriteBounds.width;
                float scaleY = region->bounds.height / spriteBounds.height;

                float scale = scaleX < scaleY ? scaleX : scaleY;
                sprite->SetScale(scale);
            }
        }

        void ClickableAreaManager::SetRegionPosition(const std::string& id, const raylib::Vector2& newPosition) {
            ClickableRegion* region = FindRegionById(id);
            if (region) {
                region->bounds.x = newPosition.x;
                region->bounds.y = newPosition.y;

                if (region->sprite) {
                    raylib::Vector2 centerPos = {
                        region->bounds.x + region->bounds.width / 2.0f,
                        region->bounds.y + region->bounds.height / 2.0f
                    };
                    region->sprite->SetPosition(centerPos);
                }
            }
        }

        void ClickableAreaManager::SetRegionPosition(const std::string& id, float x, float y) {
            SetRegionPosition(id, raylib::Vector2{x, y});
        }

        void ClickableAreaManager::SetRegionText(const std::string& id, const std::string& text, const raylib::Color& color, int fontSize) {
            ClickableRegion* region = FindRegionById(id);
            if (region) {
                region->text = text;
                region->textColor = color;
                region->fontSize = fontSize;
                region->hasBackground = false;
                region->useCustomFont = false;
            }
        }

        void ClickableAreaManager::SetRegionText(const std::string& id, const std::string& text, const raylib::Color& color, const raylib::Color& backgroundColor, int fontSize) {
            ClickableRegion* region = FindRegionById(id);
            if (region) {
                region->text = text;
                region->textColor = color;
                region->backgroundColor = backgroundColor;
                region->fontSize = fontSize;
                region->hasBackground = true;
                region->useCustomFont = false;
            }
        }

        void ClickableAreaManager::SetRegionText(const std::string& id, const std::string& text, std::shared_ptr<raylib::Font> font, const raylib::Color& color, int fontSize) {
            ClickableRegion* region = FindRegionById(id);
            if (region) {
                region->text = text;
                region->font = font;
                region->textColor = color;
                region->fontSize = fontSize;
                region->hasBackground = false;
                region->useCustomFont = true;
            }
        }

        void ClickableAreaManager::SetRegionText(const std::string& id, const std::string& text, std::shared_ptr<raylib::Font> font, const raylib::Color& color, const raylib::Color& backgroundColor, int fontSize) {
            ClickableRegion* region = FindRegionById(id);
            if (region) {
                region->text = text;
                region->font = font;
                region->textColor = color;
                region->backgroundColor = backgroundColor;
                region->fontSize = fontSize;
                region->hasBackground = true;
                region->useCustomFont = true;
            }
        }

        void ClickableAreaManager::Clear() {
            regions.clear();
        }

        void ClickableAreaManager::Update() { 
            // Use mouse coordinates adjusted to the virtual canvas (800x450) instead of the actual window resolution

            namespace ED = Engine::Display;
            using EDD = Engine::Display::Display;

            raylib::Vector2 mousePos = EDD::Instance().GetVirtualMousePosition();
            bool isClicked = raylib::Mouse::IsButtonPressed(MOUSE_BUTTON_LEFT);

            for (auto& region : regions) {
                region.isHovered = region.bounds.CheckCollision(mousePos);

                if (region.sprite) {
                    region.sprite->Update();
                }

                if (region.isHovered && isClicked) {
                    if (region.onClick) {
                        region.onClick();
                    }
                    break;
                }
            }
        }

        void ClickableAreaManager::Draw() const {
            for (const auto& region : regions) {
                if (region.sprite) {
                    region.sprite->Draw();
                }

                if (!region.text.empty()) {
                    int textWidth;
                    int textHeight;

                    if (region.hasBackground) {
                        raylib::Rectangle textBgBounds = {
                            region.bounds.x,
                            region.bounds.y,
                            region.bounds.width,
                            region.bounds.height
                        };
                        textBgBounds.Draw(region.backgroundColor);
                    }

                    if (region.useCustomFont && region.font) {
                        raylib::Vector2 textSize = region.font->MeasureText(region.text.c_str(), static_cast<float>(region.fontSize), 1.0f);
                        textWidth = static_cast<int>(textSize.x);
                        textHeight = static_cast<int>(textSize.y);

                        float textX = region.bounds.x + (region.bounds.width - textWidth) / 2.0f;
                        float textY = region.bounds.y + (region.bounds.height - textHeight) / 2.0f;

                        region.font->DrawText(region.text.c_str(), raylib::Vector2{textX, textY}, static_cast<float>(region.fontSize), 1.0f, region.textColor);
                    } else {
                        textWidth = raylib::MeasureText(region.text.c_str(), region.fontSize);
                        textHeight = region.fontSize;

                        float textX = region.bounds.x + (region.bounds.width - textWidth) / 2.0f;
                        float textY = region.bounds.y + (region.bounds.height - textHeight) / 2.0f;

                        raylib::DrawText(region.text.c_str(), static_cast<int>(textX), static_cast<int>(textY), region.fontSize, region.textColor);
                    }
                }
            }
        }

        void ClickableAreaManager::DrawBoundary() const {
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
