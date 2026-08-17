#include "raylib-cpp.hpp"

int main(int argc, char** argv) {
    // 1. Initialize screen and window (automatically closed in destructor thanks to RAII)
    const int screenWidth = 800;
    const int screenHeight = 450;

    raylib::Window window(screenWidth, screenHeight, "raylib-cpp example");
    SetTargetFPS(60);

    // 2. Set ball state values
    raylib::Vector2 ballPosition(screenWidth / 2.0f, screenHeight / 2.0f);
    raylib::Vector2 ballSpeed(5.0f, 4.0f);
    const float ballRadius = 24.0f;

    // 3. Main game loop
    while (!window.ShouldClose()) {
        // [Update] Move ball position and handle wall collisions
        ballPosition += ballSpeed;

        if ((ballPosition.x >= (screenWidth - ballRadius)) || (ballPosition.x <= ballRadius)) {
            ballSpeed.x *= -1.0f;
        }
        if ((ballPosition.y >= (screenHeight - ballRadius)) || (ballPosition.y <= ballRadius)) {
            ballSpeed.y *= -1.0f;
        }

        // [Rendering]
        BeginDrawing();
            window.ClearBackground(RAYWHITE);

            // Drawing using raylib-cpp methods
            ballPosition.DrawCircle(ballRadius, MAROON);

            raylib::DrawText("raylib-cpp example", 20, 20, 20, DARKGRAY);
            raylib::DrawText(std::string("FPS: ") + std::to_string(GetFPS()), 20, 50, 20, LIME);
            raylib::DrawText("Press ESC to exit.", 20, screenHeight - 40, 18, LIGHTGRAY);

        EndDrawing();
    }

    return 0;
}