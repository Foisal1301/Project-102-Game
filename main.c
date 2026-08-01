#include "raylib.h"

#define HEIGHT 600
#define WIDTH 800
int main(void)
{
    InitWindow(WIDTH, HEIGHT, "Game");
    SetTargetFPS(60);

    float ground = 3*HEIGHT/4;

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("Never ever!", 190, 200, 80, LIGHTGRAY);

        Rectangle groundRect = {0,ground,WIDTH,HEIGHT-ground};
        DrawRectangleRec(groundRect,GetColor(0x014B43FF));
        DrawRectangleLinesEx(groundRect,3,BLACK);
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
