#include "raylib.h"

int main(void)
{
    InitWindow(800, 600, "Game");
    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("Swaaaaaaaar!", 190, 200, 20, LIGHTGRAY);
        DrawText("Yo check check", 190, 200, 20, LIGHTGRAY);
        DrawText("East or West,I am the best!", 190, 200, 20, LIGHTGRAY);
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
