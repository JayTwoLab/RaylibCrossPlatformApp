#include "raylib.h"

int main(int argc, char** argv)
{
    // 1. 화면 해상도 설정 (데스크톱 및 모바일 기본값)
    const int screenWidth = 800;
    const int screenHeight = 450;

    // 모바일/데스크톱 플래그 설정 (화면 크기 조절 가능하도록)
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);

    InitWindow(screenWidth, screenHeight, "raylib Cross-Platform Test Application");

    // 2. 기본 타겟 FPS 설정 (60FPS)
    SetTargetFPS(60);

    // 공 위치 초기화
    Vector2 ballPosition = { (float)GetScreenWidth() / 2, (float)GetScreenHeight() / 2 };
    Color ballColor = MAROON;

    // -------------------------------------------------------------------------
    // 메인 게임 루프 (창이 닫히거나 ESC를 누를 때까지 반복)
    // -------------------------------------------------------------------------
    while (!WindowShouldClose())
    {
        // --- 업데이트 (Update) ---
        
        // 키보드 입력 처리
        if (IsKeyDown(KEY_RIGHT)) ballPosition.x += 4.0f;
        if (IsKeyDown(KEY_LEFT))  ballPosition.x -= 4.0f;
        if (IsKeyDown(KEY_UP))    ballPosition.y -= 4.0f;
        if (IsKeyDown(KEY_DOWN))  ballPosition.y += 4.0f;

        // 마우스/터치 입력 처리 (모바일 및 데스크톱 공통)
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) || GetTouchPointCount() > 0)
        {
            ballPosition = GetMousePosition();
        }

        // --- 그리기 (Draw) ---
        BeginDrawing();

            ClearBackground(RAYWHITE);

            // 텍스트 출력 테스트
            DrawText("raylib Cross-Platform Test App", 20, 20, 20, DARKGRAY);
            DrawText("Use ARROW KEYS or MOUSE/TOUCH to move the circle", 20, 50, 16, GRAY);

            // 입력 위치에 원 그리기
            DrawCircleV(ballPosition, 30, ballColor);

            // FPS 및 화면 해상도 정보 출력
            DrawText(TextFormat("FPS: %i", GetFPS()), 20, GetScreenHeight() - 40, 20, LIME);
            DrawText(TextFormat("Screen: %dx%d", GetScreenWidth(), GetScreenHeight()), 120, GetScreenHeight() - 40, 20, DARKBLUE);

        EndDrawing();
    }

    // -------------------------------------------------------------------------
    // 종료 처리 (De-Initialization)
    // -------------------------------------------------------------------------
    CloseWindow(); // 창 닫기 및 OpenGL 컨텍스트 해제

    return 0;
}