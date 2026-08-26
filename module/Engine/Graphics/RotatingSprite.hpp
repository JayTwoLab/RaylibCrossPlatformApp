#pragma once

#include <string>
#include <unordered_map>
#include <functional>
#include "raylib-cpp.hpp"

namespace Engine {
    namespace Graphics {

        struct SpriteClip {
            raylib::Texture2D texture;
            int frameCount = 1;
            float frameSpeed = 0.1f;
            float frameWidth = 0.0f;
            float frameHeight = 0.0f;
        };

        class RotatingSprite {
        private:
            std::unordered_map<std::string, SpriteClip> clips;
            SpriteClip* currentClip = nullptr;
            std::string currentClipKey = "";

            raylib::Vector2 position;
            float rotation;
            float rotationSpeed;
            float scale;
            int zOrder;

            int currentFrame;
            float frameTimer;

            // 클릭 콜백 함수 포인터
            std::function<void()> onClickCallback = nullptr;

        public:
            RotatingSprite();
            ~RotatingSprite();

            bool RegisterClip(const std::string& key, const std::string& filePath);
            bool RegisterClipSheet(const std::string& key, const std::string& filePath, int frames, float animSpeed);
            bool SetClipState(const std::string& key);

            void Update();
            void Draw() const;
            void Unload();

            // 클릭 이벤트 설정 및 트리거
            void SetOnClick(std::function<void()> callback) { onClickCallback = callback; }
            bool TriggerClick() {
                if (onClickCallback) {
                    onClickCallback();
                    return true;
                }
                return false;
            }

            // 중심점 기준 사각형 충돌 영역 반환
            raylib::Rectangle GetBounds() const {
                if (!currentClip) return { position.x, position.y, 0.0f, 0.0f };
                float w = currentClip->frameWidth * scale;
                float h = currentClip->frameHeight * scale;
                return raylib::Rectangle{ position.x - (w * 0.5f), position.y - (h * 0.5f), w, h };
            }

            void SetPosition(const raylib::Vector2& pos);
            void SetPosition(float x, float y);
            void SetRotation(float deg);
            void SetRotationSpeed(float speed);
            void SetScale(float s);
            void SetZOrder(int z);

            raylib::Vector2 GetPosition() const;
            float GetRotation() const;
            int GetZOrder() const;
            std::string GetCurrentState() const;
        };

    } // namespace Graphics
} // namespace Engine
