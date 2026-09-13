#pragma once

#include <string>
#include "raylib.h"
#include "imgui.h"

namespace Engine {
    namespace UI {

        void SetupImGui(const std::string& fontPath = "", float fontSize = 18.0f, bool darkTheme = true);
        void BeginImGui();
        void EndImGui();
        void ShutdownImGui();

        bool WantCaptureMouse();
        bool WantCaptureKeyboard();

    } // namespace UI
} // namespace Engine
