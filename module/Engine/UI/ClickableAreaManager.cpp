#include "Engine/UI/ClickableAreaManager.hpp"
#include "Engine/UI/rlImGui.hpp"
#include "Engine/Display/Display.hpp"
#include <algorithm>

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

        void ClickableAreaManager::SortRegionsIfNeeded() {
            if (!isSorted_) {
                std::stable_sort(regions.begin(), regions.end(),
                    [](const ClickableRegion& a, const ClickableRegion& b) {
                        return a.zOrder > b.zOrder;
                    });
                isSorted_ = true;
            }
        }

        void ClickableAreaManager::AddRegion(const std::string& id, const raylib::Rectangle& bounds, std::function<void()> onClick, int zOrder) {
            ClickableRegion* existing = FindRegionById(id);
            if (existing) {
                existing->bounds = bounds;
                existing->onClick = onClick;
                existing->zOrder = zOrder;
                isSorted_ = false;
                return;
            }

            ClickableRegion region;
            region.id = id;
            region.bounds = bounds;
            region.onClick = onClick;
            region.zOrder = zOrder;
            region.enabled = true;
            region.isHovered = false;
            region.isPressed = false;
            region.sprite = nullptr;
            region.text = "";
            region.textColor = raylib::Color::Black();
            region.backgroundColor = raylib::Color::White();
            region.fontSize = 20;
            region.hasBackground = false;
            region.font = nullptr;
            region.useCustomFont = false;

            regions.push_back(std::move(region));
            isSorted_ = false;
        }

        void ClickableAreaManager::RemoveRegion(const std::string& id) {
            regions.erase(
                std::remove_if(regions.begin(), regions.end(),
                    [&id](const ClickableRegion& r) { return r.id == id; }),
                regions.end()
            );
        }

        void ClickableAreaManager::SetRegionSprite(const std::string& id, std::shared_ptr<Engine::Graphics::RotatingSprite> sprite) {
            ClickableRegion* region = FindRegionById(id);
            if (!region || !sprite) return;

            region->sprite = sprite;

            raylib::Vector2 centerPos = {
                region->bounds.x + region->bounds.width * 0.5f,
                region->bounds.y + region->bounds.height * 0.5f
            };
            sprite->SetPosition(centerPos);

            raylib::Rectangle spriteBounds = sprite->GetBounds();
            if (spriteBounds.width > 0.0f && spriteBounds.height > 0.0f) {
                float scaleX = region->bounds.width / spriteBounds.width;
                float scaleY = region->bounds.height / spriteBounds.height;
                sprite->SetScale(std::min(scaleX, scaleY));
            }
        }

        void ClickableAreaManager::SetRegionPosition(const std::string& id, const raylib::Vector2& newPosition) {
            ClickableRegion* region = FindRegionById(id);
            if (!region) return;

            region->bounds.x = newPosition.x;
            region->bounds.y = newPosition.y;

            if (region->sprite) {
                raylib::Vector2 centerPos = {
                    region->bounds.x + region->bounds.width * 0.5f,
                    region->bounds.y + region->bounds.height * 0.5f
                };
                region->sprite->SetPosition(centerPos);
            }
        }

        void ClickableAreaManager::SetRegionPosition(const std::string& id, float x, float y) {
            SetRegionPosition(id, raylib::Vector2{ x, y });
        }

        void ClickableAreaManager::SetRegionZOrder(const std::string& id, int zOrder) {
            ClickableRegion* region = FindRegionById(id);
            if (region && region->zOrder != zOrder) {
                region->zOrder = zOrder;
                isSorted_ = false;
            }
        }

        void ClickableAreaManager::SetRegionEnabled(const std::string& id, bool enabled) {
            ClickableRegion* region = FindRegionById(id);
            if (region) {
                region->enabled = enabled;
                if (!enabled) {
                    region->isHovered = false;
                    region->isPressed = false;
                    if (region->sprite) region->sprite->SetClipState("idle");
                }
            }
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
            isSorted_ = true;
        }

        void ClickableAreaManager::Update() {
            if (WantCaptureMouse()) {
                for (auto& region : regions) {
                    region.isHovered = false;
                    if (region.isPressed) {
                        region.isPressed = false;
                        if (region.sprite) region.sprite->SetClipState("idle");
                    }
                    if (region.sprite) region.sprite->Update();
                }
                return;
            }

            SortRegionsIfNeeded();

            raylib::Vector2 mousePos = Engine::Display::Display::Instance().GetVirtualMousePosition();
            bool isButtonPressed = raylib::Mouse::IsButtonPressed(MOUSE_BUTTON_LEFT);
            bool isButtonDown = raylib::Mouse::IsButtonDown(MOUSE_BUTTON_LEFT);
            bool isButtonReleased = raylib::Mouse::IsButtonReleased(MOUSE_BUTTON_LEFT);

            bool inputBlocked = false;

            for (auto& region : regions) {
                if (!region.enabled) {
                    if (region.sprite) region.sprite->Update();
                    continue;
                }

                bool pointInside = region.bounds.CheckCollision(mousePos);
                region.isHovered = !inputBlocked && pointInside;

                if (region.isHovered) {
                    inputBlocked = true;

                    if (isButtonPressed) {
                        region.isPressed = true;
                        if (region.sprite) region.sprite->SetClipState("pressed");
                    }

                    if (isButtonReleased && region.isPressed) {
                        region.isPressed = false;
                        if (region.sprite) region.sprite->SetClipState("idle");
                        if (region.onClick) region.onClick();
                    }
                }
                else {
                    if (!isButtonDown || isButtonReleased) {
                        if (region.isPressed) {
                            region.isPressed = false;
                            if (region.sprite) region.sprite->SetClipState("idle");
                        }
                    }
                }

                if (region.sprite) {
                    region.sprite->Update();
                }
            }
        }

        void ClickableAreaManager::Draw() const {
            for (auto it = regions.rbegin(); it != regions.rend(); ++it) {
                const auto& region = *it;
                if (!region.enabled) continue;

                if (region.sprite) {
                    region.sprite->Draw();
                }

                if (!region.text.empty()) {
                    if (region.hasBackground) {
                        region.bounds.Draw(region.backgroundColor);
                    }

                    float textWidth = 0.0f;
                    float textHeight = static_cast<float>(region.fontSize);

                    if (region.useCustomFont && region.font) {
                        raylib::Vector2 textSize = region.font->MeasureText(region.text.c_str(), static_cast<float>(region.fontSize), 1.0f);
                        textWidth = textSize.x;
                        textHeight = textSize.y;

                        float textX = region.bounds.x + (region.bounds.width - textWidth) * 0.5f;
                        float textY = region.bounds.y + (region.bounds.height - textHeight) * 0.5f;

                        region.font->DrawText(region.text.c_str(), raylib::Vector2{ textX, textY }, static_cast<float>(region.fontSize), 1.0f, region.textColor);
                    }
                    else {
                        textWidth = static_cast<float>(raylib::MeasureText(region.text.c_str(), region.fontSize));

                        float textX = region.bounds.x + (region.bounds.width - textWidth) * 0.5f;
                        float textY = region.bounds.y + (region.bounds.height - textHeight) * 0.5f;

                        raylib::DrawText(region.text.c_str(), static_cast<int>(textX), static_cast<int>(textY), region.fontSize, region.textColor);
                    }
                }
            }
        }

        void ClickableAreaManager::DrawBoundary() const {
            for (const auto& region : regions) {
                if (!region.enabled || !region.isHovered) continue;

                raylib::Rectangle shadowBounds = {
                    region.bounds.x - 1.0f,
                    region.bounds.y - 1.0f,
                    region.bounds.width + 2.0f,
                    region.bounds.height + 2.0f
                };
                shadowBounds.DrawLines(raylib::Color::Black(), 1.0f);
                region.bounds.DrawLines(raylib::Color::Yellow(), 2.0f);
            }
        }

    } // namespace UI
} // namespace Engine
