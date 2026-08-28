#pragma once

#include "raylib-cpp.hpp"
#include <filesystem>

namespace TestUtils {

    inline void GenerateAssets() {

        // resources 디렉터리가 없으면 생성
        if (!std::filesystem::exists("resources")) {
            std::filesystem::create_directories("resources");
        }

        // 1. character_idle.png (64x64 단일 이미지)
        {
            raylib::Image img(64, 64, raylib::Color::Blank());

            // 몸체 (파란색 원)
            img.DrawCircle(32, 32, 24, raylib::Color::SkyBlue());
            // 전방 방향 표시 점 (회전 식별용 주황색 점)
            img.DrawCircle(32, 14, 6, raylib::Color::Orange());
            // 중심 코어 (하얀색)
            img.DrawCircle(32, 32, 8, raylib::Color::White());

            img.Export("resources/character_idle.png");
        }

        // 2. character_hurt.png (64x64 단일 이미지)
        {
            raylib::Image img(64, 64, raylib::Color::Blank());

            // 몸체 (빨간색)
            img.DrawCircle(32, 32, 24, raylib::Color::Red());
            // 피격 표시 (노란색)
            img.DrawCircle(32, 14, 6, raylib::Color::Yellow());
            img.DrawCircle(32, 32, 10, raylib::Color::Yellow());

            img.Export("resources/character_hurt.png");
        }

        // 3. character_walk_sheet.png (256x64 스프라이트 시트 - 4프레임)
        {
            raylib::Image sheet(256, 64, raylib::Color::Blank());

            for (int frame = 0; frame < 4; frame++) {
                int centerX = frame * 64 + 32;
                int centerY = 32;

                int radius = 22 + (frame % 2) * 4;
                int offsetY = (frame == 1 || frame == 3) ? -3 : 2;

                // 초록색 계열 걷기/이동 프레임
                sheet.DrawCircle(centerX, centerY + offsetY, radius, raylib::Color::Lime());
                // 전방 표시
                sheet.DrawCircle(centerX, centerY + offsetY - 16, 5, raylib::Color::DarkGreen());
                // 코어
                sheet.DrawCircle(centerX, centerY + offsetY, 6, raylib::Color::White());
            }

            sheet.Export("resources/character_walk_sheet.png");
        }

        TraceLog(LOG_INFO, "[AssetGen] Test PNG assets generated in ./resources/");
    }

} // namespace TestUtils
