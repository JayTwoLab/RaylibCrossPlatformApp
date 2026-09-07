#include "Engine/Graphics/RotatingSprite.hpp"
#include "Engine/Resource/ResourceManager.hpp"
#include <utility>

namespace Engine {
    namespace Graphics {

        RotatingSprite::RotatingSprite()
            : position{ 0.0f, 0.0f }, rotation(0.0f), rotationSpeed(0.0f),
            scale(1.0f), zOrder(0), currentFrame(0), frameTimer(0.0f) {
        }

        RotatingSprite::~RotatingSprite() {
            Unload();
        }

        bool RotatingSprite::RegisterClip(const std::string& key, const std::string& filePath) {
            return RegisterClipSheet(key, filePath, 1, 0.0f);
        }

        bool RotatingSprite::RegisterClipSheet(const std::string& key, const std::string& filePath, int frames, float animSpeed) {
            auto it = clips.find(key);
            if (it != clips.end()) {
                if (it->second.texture.id > 0) {
                    it->second.texture.Unload();
                }
            }

            // 디스크 모드와 rres 모드 모두 투명하게 지원
            raylib::Texture2D tex = Engine::Resource::ResourceManager::Instance().LoadTexture(filePath);
            if (tex.id <= 0) return false;

            SetTextureFilter(tex, TEXTURE_FILTER_BILINEAR);

            int count = frames > 0 ? frames : 1;
            SpriteClip clip;
            clip.texture = std::move(tex);
            clip.frameCount = count;
            clip.frameSpeed = animSpeed;
            clip.frameWidth = (float)clip.texture.width / count;
            clip.frameHeight = (float)clip.texture.height;

            clips[key] = std::move(clip);

            if (currentClip == nullptr) {
                SetClipState(key);
            }

            return true;
        }

        bool RotatingSprite::SetClipState(const std::string& key) {
            auto it = clips.find(key);
            if (it == clips.end()) return false;

            if (currentClipKey != key) {
                currentClipKey = key;
                currentClip = &it->second;
                currentFrame = 0;
                frameTimer = 0.0f;
            }
            return true;
        }

        void RotatingSprite::Update() {
            float dt = GetFrameTime();

            rotation += rotationSpeed * dt;
            if (rotation >= 360.0f) rotation -= 360.0f;
            if (rotation < 0.0f)   rotation += 360.0f;

            if (currentClip && currentClip->frameCount > 1 && currentClip->frameSpeed > 0.0f) {
                frameTimer += dt;
                if (frameTimer >= currentClip->frameSpeed) {
                    frameTimer = 0.0f;
                    currentFrame = (currentFrame + 1) % currentClip->frameCount;
                }
            }
        }

        void RotatingSprite::Draw() const {
            if (!currentClip || currentClip->texture.id <= 0) return;

            raylib::Rectangle srcRect = {
                (float)currentFrame * currentClip->frameWidth,
                0.0f,
                currentClip->frameWidth,
                currentClip->frameHeight
            };

            float destW = currentClip->frameWidth * scale;
            float destH = currentClip->frameHeight * scale;
            raylib::Rectangle destRect = { position.x, position.y, destW, destH };

            raylib::Vector2 origin = { destW * 0.5f, destH * 0.5f };

            currentClip->texture.Draw(srcRect, destRect, origin, rotation, raylib::Color::White());
        }

        void RotatingSprite::Unload() {
            for (auto& pair : clips) {
                if (pair.second.texture.id > 0) {
                    pair.second.texture.Unload();
                }
            }
            clips.clear();
            currentClip = nullptr;
            currentClipKey = "";
        }

        void RotatingSprite::SetOnClick(std::function<void()> callback) {
            onClickCallback = callback;
        }

        bool RotatingSprite::TriggerClick() {
            if (onClickCallback) {
                onClickCallback();
                return true;
            }
            return false;
        }

        raylib::Rectangle  RotatingSprite::GetBounds() const {
            if (!currentClip) return { position.x, position.y, 0.0f, 0.0f };
            float w = currentClip->frameWidth * scale;
            float h = currentClip->frameHeight * scale;
            return raylib::Rectangle{ position.x - (w * 0.5f), position.y - (h * 0.5f), w, h };
        }

        void RotatingSprite::SetPosition(const raylib::Vector2& pos) { position = pos; }
        void RotatingSprite::SetPosition(float x, float y) { position = { x, y }; }
        void RotatingSprite::SetRotation(float deg) { rotation = deg; }
        void RotatingSprite::SetRotationSpeed(float speed) { rotationSpeed = speed; }
        void RotatingSprite::SetScale(float s) { scale = s; }
        void RotatingSprite::SetZOrder(int z) { zOrder = z; }

        raylib::Vector2 RotatingSprite::GetPosition() const { return position; }
        float RotatingSprite::GetRotation() const { return rotation; }
        int RotatingSprite::GetZOrder() const { return zOrder; }
        std::string RotatingSprite::GetCurrentState() const { return currentClipKey; }

    } // namespace Graphics
} // namespace Engine
