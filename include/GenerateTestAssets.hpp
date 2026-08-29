#pragma once

#include "raylib-cpp.hpp"
#include <filesystem>

namespace TestUtils {

    inline void GenerateAssets() {

        // Create the resources directory if it doesn't exist
        if (!std::filesystem::exists("resources")) {
            std::filesystem::create_directories("resources");
        }

        // 1. character_idle.png (64x64 single image)
        {
            raylib::Image img(64, 64, raylib::Color::Blank());

            // Body (blue circle)
            img.DrawCircle(32, 32, 24, raylib::Color::SkyBlue());
            // Forward direction indicator dot (orange dot for rotation identification)
            img.DrawCircle(32, 14, 6, raylib::Color::Orange());
            // Central core (white)
            img.DrawCircle(32, 32, 8, raylib::Color::White());

            img.Export("resources/character_idle.png");
        }

        // 2. character_hurt.png (64x64 single image)
        {
            raylib::Image img(64, 64, raylib::Color::Blank());

            // Body (red)
            img.DrawCircle(32, 32, 24, raylib::Color::Red());
            // Hit indicators (yellow)
            img.DrawCircle(32, 14, 6, raylib::Color::Yellow());
            img.DrawCircle(32, 32, 10, raylib::Color::Yellow());

            img.Export("resources/character_hurt.png");
        }

        // 3. character_walk_sheet.png (256x64 sprite sheet - 4 frames)
        {
            raylib::Image sheet(256, 64, raylib::Color::Blank());

            for (int frame = 0; frame < 4; frame++) {
                int centerX = frame * 64 + 32;
                int centerY = 32;

                int radius = 22 + (frame % 2) * 4;
                int offsetY = (frame == 1 || frame == 3) ? -3 : 2;

                // Greenish walking/moving frame
                sheet.DrawCircle(centerX, centerY + offsetY, radius, raylib::Color::Lime());
                // Front indicator
                sheet.DrawCircle(centerX, centerY + offsetY - 16, 5, raylib::Color::DarkGreen());
                // Core
                sheet.DrawCircle(centerX, centerY + offsetY, 6, raylib::Color::White());
            }

            sheet.Export("resources/character_walk_sheet.png");
        }

        TraceLog(LOG_INFO, "[AssetGen] Test PNG assets generated in ./resources/");
    }

} // namespace TestUtils
