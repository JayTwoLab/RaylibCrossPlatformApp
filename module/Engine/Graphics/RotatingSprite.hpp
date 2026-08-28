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

            void SetOnClick(std::function<void()> callback);
            bool TriggerClick();

            raylib::Rectangle GetBounds() const;

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
