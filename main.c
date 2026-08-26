#include "raylib.h"
#include <stdio.h>

#define HEIGHT 600
#define WIDTH 800
#define BALLROWS 6
#define BALLCOLS 14

// PAGES {"Menu","RESUME","ABOUT","GAMEPLAY"}

int pageIndex = 3;
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

        switch (pageIndex)
        {
        case 3:

        //loding ball images

        for (int i = 0; i < 2; i++)
        {
            char path[50];
            sprintf(path, "assets/ball_%d_small_alt.png", i+1);
            
        }
        

            int balls[BALLROWS][BALLCOLS];
            for (int i = 0; i < BALLROWS; i++)
            {
                for (int j = 0; j < BALLCOLS; j++)
                {
                    
                }
                
            }
            

            break;
        }
        
        EndDrawing();
    }

    UnloadTexture(bg);
    CloseWindow();

    return 0;
}