#include "Engine/UI/rlImGui.hpp"
#include "Engine/Display/Display.hpp"
#include "rlgl.h"
#include <cstdint>
#include <cfloat>
#include <algorithm>
#include <filesystem>
#include <vector>

namespace Engine {
    namespace UI {

        static Texture2D g_FontTexture = { 0 };

        static void RenderDrawData(ImDrawData* draw_data) {
            rlDrawRenderBatchActive();
            rlDisableBackfaceCulling();
            rlDisableDepthTest();

            // High-DPI scaling support (ratio of physical pixels to logical coordinates)
            ImVec2 clip_scale = draw_data->FramebufferScale;

            for (int n = 0; n < draw_data->CmdListsCount; n++) {
                const ImDrawList* cmd_list = draw_data->CmdLists[n];
                const ImDrawVert* vtx_buffer = cmd_list->VtxBuffer.Data;
                const ImDrawIdx* idx_buffer = cmd_list->IdxBuffer.Data;

                for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++) {
                    const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];

                    if (pcmd->UserCallback != nullptr) {
                        pcmd->UserCallback(cmd_list, pcmd);
                    }
                    else {
                        // Calculate scissor rect for OpenGL bottom-left coordinates with High-DPI scaling
                        int scissorX = static_cast<int>(pcmd->ClipRect.x * clip_scale.x);
                        int scissorY = static_cast<int>((draw_data->DisplaySize.y - pcmd->ClipRect.w) * clip_scale.y);
                        int scissorW = static_cast<int>((pcmd->ClipRect.z - pcmd->ClipRect.x) * clip_scale.x);
                        int scissorH = static_cast<int>((pcmd->ClipRect.w - pcmd->ClipRect.y) * clip_scale.y);

                        if (scissorW <= 0 || scissorH <= 0) continue;

                        rlEnableScissorTest();
                        rlScissor(scissorX, scissorY, scissorW, scissorH);

                        ImTextureID texId = pcmd->GetTexID();
                        uint32_t glTextureId = (texId != 0)
                            ? static_cast<uint32_t>((uintptr_t)texId)
                            : g_FontTexture.id;

                        rlSetTexture(glTextureId);
                        rlBegin(RL_TRIANGLES);

                        for (unsigned int i = 0; i < pcmd->ElemCount; i++) {
                            ImDrawIdx idx = idx_buffer[pcmd->IdxOffset + i];
                            const ImDrawVert& v = vtx_buffer[pcmd->VtxOffset + idx];

                            rlColor4ub(
                                static_cast<unsigned char>(v.col & 0xFF),
                                static_cast<unsigned char>((v.col >> 8) & 0xFF),
                                static_cast<unsigned char>((v.col >> 16) & 0xFF),
                                static_cast<unsigned char>((v.col >> 24) & 0xFF)
                            );
                            rlTexCoord2f(v.uv.x, v.uv.y);
                            rlVertex2f(v.pos.x, v.pos.y);
                        }

                        rlEnd();

                        // Flush render batch immediately while scissor and texture bindings remain active
                        rlDrawRenderBatchActive();
                    }
                }
            }

            rlSetTexture(0);
            rlDisableScissorTest();
            rlEnableBackfaceCulling();
        }

        void SetupImGui(const std::string& fontPath, float fontSize, bool darkTheme) {
            IMGUI_CHECKVERSION();
            ImGui::CreateContext();

            if (darkTheme) {
                ImGui::StyleColorsDark();
            }
            else {
                ImGui::StyleColorsLight();
            }

            ImGuiIO& io = ImGui::GetIO();
            io.BackendPlatformName = "imgui_impl_raylib";
            io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

            bool fontLoaded = false;

            // 1. Try user-specified font path first
            if (!fontPath.empty() && std::filesystem::exists(fontPath)) {
                ImFont* font = io.Fonts->AddFontFromFileTTF(
                    fontPath.c_str(),
                    fontSize,
                    nullptr,
                    io.Fonts->GetGlyphRangesKorean()
                );
                if (font != nullptr) {
                    fontLoaded = true;
                    TraceLog(LOG_INFO, "ImGui: User Korean font loaded successfully: %s", fontPath.c_str());
                }
            }

            // 2. OS-specific system Korean font fallback
            if (!fontLoaded) {
#if defined(_WIN32)
                const std::vector<const char*> systemFonts = {
                    "C:/Windows/Fonts/malgun.ttf",       // Malgun Gothic
                    "C:/Windows/Fonts/malgunbd.ttf",     // Malgun Gothic Bold
                    "C:/Windows/Fonts/gulim.ttc"         // Gulim
                };
#elif defined(__APPLE__)
                const std::vector<const char*> systemFonts = {
                    "/System/Library/Fonts/AppleSDGothicNeo.ttc",
                    "/System/Library/Fonts/Supplemental/AppleGothic.ttf"
                };
#elif defined(__ANDROID__)
                const std::vector<const char*> systemFonts = {
                    "/system/fonts/NotoSansCJK-Regular.ttc",
                    "/system/fonts/NotoSansKR-Regular.otf",
                    "/system/fonts/NanumGothic.ttf"
                };
#elif defined(__linux__)
                const std::vector<const char*> systemFonts = {
                    "/usr/share/fonts/truetype/nanum/NanumGothic.ttf",
                    "/usr/share/fonts/opentype/noto/NotoSansCJK-Regular.ttc",
                    "/usr/share/fonts/truetype/noto/NotoSansCJK-Regular.ttc"
                };
#else
                const std::vector<const char*> systemFonts = {};
#endif

                for (const char* path : systemFonts) {
                    if (std::filesystem::exists(path)) {
                        ImFont* font = io.Fonts->AddFontFromFileTTF(
                            path,
                            fontSize,
                            nullptr,
                            io.Fonts->GetGlyphRangesKorean()
                        );
                        if (font != nullptr) {
                            fontLoaded = true;
                            TraceLog(LOG_INFO, "ImGui: System Korean font fallback loaded: %s", path);
                            break;
                        }
                    }
                }
            }

            // 3. Fallback to default ProggyClean font if all attempts fail
            if (!fontLoaded) {
                TraceLog(LOG_WARNING, "ImGui: Failed to load Korean font, falling back to default font");
                io.Fonts->AddFontDefault();
            }

            unsigned char* pixels = nullptr;
            int width = 0, height = 0;
            io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

            Image fontImage = {
                pixels,
                width,
                height,
                1,
                PIXELFORMAT_UNCOMPRESSED_R8G8B8A8
            };
            g_FontTexture = LoadTextureFromImage(fontImage);
            io.Fonts->SetTexID((ImTextureID)(uintptr_t)g_FontTexture.id);
        }

        void BeginImGui() {
            ImGuiIO& io = ImGui::GetIO();

            auto& display = Engine::Display::Display::Instance();
            float screenW = static_cast<float>(GetScreenWidth());
            float screenH = static_cast<float>(GetScreenHeight());

            // Set ImGui workspace display size to virtual canvas resolution
            float virtualW = (display.VirtualWidth > 0) ? static_cast<float>(display.VirtualWidth) : screenW;
            float virtualH = (display.VirtualHeight > 0) ? static_cast<float>(display.VirtualHeight) : screenH;

            io.DisplaySize = ImVec2(virtualW, virtualH);
            io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);
            io.DeltaTime = (GetFrameTime() > 0.0f) ? GetFrameTime() : (1.0f / 60.0f);

            // Map screen mouse coordinates to virtual resolution by inverting letterbox scale and offset
            Vector2 mousePos = GetMousePosition();
            float scaleX = screenW / virtualW;
            float scaleY = screenH / virtualH;
            float scale = std::min(scaleX, scaleY);

            float offsetX = (screenW - (virtualW * scale)) * 0.5f;
            float offsetY = (screenH - (virtualH * scale)) * 0.5f;

            // Pass valid coordinates only when mouse is within canvas boundaries, not letterbox bars
            bool isInsideCanvas = (mousePos.x >= offsetX && mousePos.x <= offsetX + (virtualW * scale) &&
                mousePos.y >= offsetY && mousePos.y <= offsetY + (virtualH * scale));

            if (isInsideCanvas && scale > 0.0f) {
                float vmx = (mousePos.x - offsetX) / scale;
                float vmy = (mousePos.y - offsetY) / scale;
                io.MousePos = ImVec2(vmx, vmy);
            }
            else {
                io.MousePos = ImVec2(-FLT_MAX, -FLT_MAX);
            }

            io.MouseDown[0] = IsMouseButtonDown(MOUSE_BUTTON_LEFT);
            io.MouseDown[1] = IsMouseButtonDown(MOUSE_BUTTON_RIGHT);
            io.MouseDown[2] = IsMouseButtonDown(MOUSE_BUTTON_MIDDLE);
            io.MouseWheel += GetMouseWheelMove();

            int key = GetCharPressed();
            while (key > 0) {
                io.AddInputCharacter(key);
                key = GetCharPressed();
            }

            ImGui::NewFrame();
        }

        void EndImGui() {
            ImGui::Render();
            RenderDrawData(ImGui::GetDrawData());
        }

        void ShutdownImGui() {
            if (g_FontTexture.id > 0) {
                UnloadTexture(g_FontTexture);
                g_FontTexture.id = 0;
            }
            ImGui::DestroyContext();
        }

        bool WantCaptureMouse() {
            return ImGui::GetCurrentContext() != nullptr && ImGui::GetIO().WantCaptureMouse;
        }

        bool WantCaptureKeyboard() {
            return ImGui::GetCurrentContext() != nullptr && ImGui::GetIO().WantCaptureKeyboard;
        }

    } // namespace UI
} // namespace Engine
