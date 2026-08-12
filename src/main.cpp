#include "raylib.h"

int main(int argc, char** argv)
{
    // 1. Set screen resolution (desktop and mobile defaults)
    const int screenWidth = 800;
    const int screenHeight = 450;

    // Configure mobile/desktop flags (allow window resizing)
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);

    InitWindow(screenWidth, screenHeight, "raylib Cross-Platform Test Application");

    // 2. Set default target FPS (60 FPS)
    SetTargetFPS(60);

    // Initialize ball position
    Vector2 ballPosition = { (float)GetScreenWidth() / 2, (float)GetScreenHeight() / 2 };
    Color ballColor = MAROON;

    // -------------------------------------------------------------------------
    // Main game loop (runs until window closes or ESC pressed)
    // -------------------------------------------------------------------------
    while (!WindowShouldClose())
    {
        // --- Update ---
        
        // Handle keyboard input
        if (IsKeyDown(KEY_RIGHT)) ballPosition.x += 4.0f;
        if (IsKeyDown(KEY_LEFT))  ballPosition.x -= 4.0f;
        if (IsKeyDown(KEY_UP))    ballPosition.y -= 4.0f;
        if (IsKeyDown(KEY_DOWN))  ballPosition.y += 4.0f;

        // Handle mouse/touch input (common for mobile and desktop)
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) || GetTouchPointCount() > 0)
        {
            ballPosition = GetMousePosition();
        }

        // --- Draw ---
        BeginDrawing();

            ClearBackground(RAYWHITE);

            // Text output test
            DrawText("raylib Cross-Platform Test App", 20, 20, 20, DARKGRAY);
            DrawText("Use ARROW KEYS or MOUSE/TOUCH to move the circle", 20, 50, 16, GRAY);

            // Draw circle at input position
            DrawCircleV(ballPosition, 30, ballColor);

            // Draw FPS and screen resolution information
            DrawText(TextFormat("FPS: %i", GetFPS()), 20, GetScreenHeight() - 40, 20, LIME);
            DrawText(TextFormat("Screen: %dx%d", GetScreenWidth(), GetScreenHeight()), 120, GetScreenHeight() - 40, 20, DARKBLUE);

        EndDrawing();
    }

    // -------------------------------------------------------------------------
    // De-initialization
    // -------------------------------------------------------------------------
    CloseWindow(); // Close window and release OpenGL context

    return 0;
}
