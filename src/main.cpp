#include "raylib-cpp.hpp"

int main(int argc, char** argv) {
    // 1. 화면 및 윈도우 초기화 (RAII 지원으로 소멸자에서 자동 종료)
    const int screenWidth = 800;
    const int screenHeight = 450;

    raylib::Window window(screenWidth, screenHeight, "raylib-cpp example");
    SetTargetFPS(60);

    // 2. 공의 상태값 설정
    raylib::Vector2 ballPosition(screenWidth / 2.0f, screenHeight / 2.0f);
    raylib::Vector2 ballSpeed(5.0f, 4.0f);
    const float ballRadius = 24.0f;

    // 3. 메인 게임 루프
    while (!window.ShouldClose()) {
        // [업데이트] 공 위치 이동 및 벽면 충돌 처리
        ballPosition += ballSpeed;

        if ((ballPosition.x >= (screenWidth - ballRadius)) || (ballPosition.x <= ballRadius)) {
            ballSpeed.x *= -1.0f;
        }
        if ((ballPosition.y >= (screenHeight - ballRadius)) || (ballPosition.y <= ballRadius)) {
            ballSpeed.y *= -1.0f;
        }

        // [렌더링]
        BeginDrawing();
            window.ClearBackground(RAYWHITE);

            // raylib-cpp 메서드를 활용한 드로잉
            ballPosition.DrawCircle(ballRadius, MAROON);

            raylib::DrawText("raylib-cpp example", 20, 20, 20, DARKGRAY);
            raylib::DrawText(std::string("FPS: ") + std::to_string(GetFPS()), 20, 50, 20, LIME);
            raylib::DrawText("Press ESC to exit.", 20, screenHeight - 40, 18, LIGHTGRAY);

        EndDrawing();
    }

    return 0;
}