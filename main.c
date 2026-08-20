#include "raylib.h"
#include "raymath.h"
#include <stdio.h>

#define HEIGHT 600
#define WIDTH 800
#define TEXTCOLOR BLACK
#define BALLROWS 6
#define BALLCOLS 14

/*
Pages
0 => Start
1 => Resume
2 => About
3 =>GamePlay
*/
int main(void)
{
    InitWindow(WIDTH, HEIGHT, "Bouncing Ball");
    SetTargetFPS(60);
    int pageIndex = 3;
    int score = 0;
    // Menu Page
    int selectedOption = 0;
    int exit = 0;

    // GamePlay
    Texture2D shooters[3];
    for(int i=1;i<=3;i++){
        char path[50];
        sprintf(path,"assets/shooter_%d.png",i);
        shooters[i-1] = LoadTexture(path);
    }
    int shooterIndex = GetRandomValue(0,2);

    Texture2D bg = LoadTexture("assets/bg.png");
    while (!WindowShouldClose() && !exit)
    {
        BeginDrawing();
        DrawTexture(bg, 0, 0, WHITE);
        switch (pageIndex)
        {
        case 0: // Menu
            if (selectedOption == 0)
            {
                DrawText("New Game", GetScreenWidth() / 2 - MeasureText("New Game", 60) / 2, 200, 60, TEXTCOLOR); // Hover effect
            }
            else
            {
                DrawText("New Game", GetScreenWidth() / 2 - MeasureText("New Game", 40) / 2, 200, 40, TEXTCOLOR);
            }

            if (selectedOption == 1)
            {
                DrawText("About", GetScreenWidth() / 2 - MeasureText("About", 60) / 2, 280, 60, TEXTCOLOR); // Hover effect
            }
            else
            {
                DrawText("About", GetScreenWidth() / 2 - MeasureText("About", 40) / 2, 280, 40, TEXTCOLOR);
            }

            if (selectedOption == 2)
            {
                DrawText("Exit", GetScreenWidth() / 2 - MeasureText("Exit", 60) / 2, 360, 60, TEXTCOLOR); // Hover effect
            }
            else
            {
                DrawText("Exit", GetScreenWidth() / 2 - MeasureText("Exit", 40) / 2, 360, 40, TEXTCOLOR);
            }

            if (IsKeyPressed(KEY_UP))
            {

                if (selectedOption > 0)
                    selectedOption--;
            }
            if (IsKeyPressed(KEY_DOWN))
            {

                if (selectedOption < 2)
                    selectedOption++;
            }

            // Page Shifting
            if (IsKeyPressed(KEY_ENTER))
            {
                switch (selectedOption)
                {
                case 0:
                    pageIndex = 3;
                    break;

                case 1:
                    pageIndex = 2;
                    break;

                case 2:
                    exit = 1;
                    break;
                }
            }

            break;
        
        case 1: // Resume page
            break;
        case 2: // About
            if (IsKeyPressed(KEY_LEFT))
            {
                pageIndex = 0;
            }
            DrawText("About", GetScreenWidth() / 2 - MeasureText("About", 60) / 2, 10, 60, TEXTCOLOR);
            DrawText("Go Back", 10 , 10, 30, TEXTCOLOR);
            break;

        case 3: // GamePlay

            // loding ball images
            for (int i = 0; i < 2; i++)
            {
                char path[50];
                sprintf(path, "assets/ball_%d_small_alt.png", i + 1);
            }

            int balls[BALLROWS][BALLCOLS];
            for (int i = 0; i < BALLROWS; i++)
            {
                for (int j = 0; j < BALLCOLS; j++)
                {
                }
            }

            // shooter
            DrawTexturePro(
                shooters[shooterIndex],
                (Rectangle){0,0,shooters[shooterIndex].width,shooters[shooterIndex].height},
                (Rectangle){
                    WIDTH / 2 - 40,
                    HEIGHT - 130,
                    80,
                    120
                },
                Vector2Zero(),0,WHITE
            );
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                Vector2 pos = GetMousePosition();
                printf("%f %f\n",pos.x,pos.y);
            }
            // score
            char scores[30];
            sprintf(scores,"SCORE : %d",score);
            DrawText(scores, 10 , 10, 30, TEXTCOLOR);
            break;
        }
        EndDrawing();
    }

    UnloadTexture(bg);
    for(int i=0;i<3;i++){
        UnloadTexture(shooters[i]);
    }
    CloseWindow();

    return 0;
}