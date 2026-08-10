#include "raylib.h"

#define HEIGHT 600
#define WIDTH 800

int main(void)
{
    InitWindow(WIDTH, HEIGHT, "Game");
    SetTargetFPS(60);

    Texture2D bg = LoadTexture("assets/bg.png");
    while (!WindowShouldClose())
    {
        BeginDrawing();
        // ClearBackground(RAYWHITE);

        DrawTexture(bg,0,0,WHITE);
        
        EndDrawing();
    }

    UnloadTexture(bg);
    CloseWindow();

    return 0;
}