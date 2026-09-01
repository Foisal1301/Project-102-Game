#include "raylib.h"
#include "raymath.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define HEIGHT 600
#define WIDTH 800
#define BALLROWS 16
#define BALLCOLS 14
#define BALLNUM 2
#define BALLRADIUS ((WIDTH - 12) / (2 * BALLCOLS + 0.5))

// PAGES {"Menu","RESUME","ABOUT","GAMEPLAY"}

int pageIndex = 3;

// My functions

int randBalls(int ballNumber)
{

    srand(time(NULL));
    int x;
    x = rand() % ballNumber + 1;
    return x;
}

int main(void)
{
    InitWindow(WIDTH, HEIGHT, "Game");
    SetTargetFPS(60);

    int randBallIdx[6][BALLCOLS];

    for (int i = 0; i < 6; i++)
    {
        for (int j = 0; j < BALLCOLS; j++)
        {
            randBallIdx[i][j] = rand() % 2;
        }
    }

    Texture2D bg = LoadTexture("assets/bg.png");
    Texture2D balls[BALLNUM];

    for (int i = 0; i < BALLNUM; i++)
    {
        char path[50];
        sprintf(path, "assets/ball_%d_small_alt.png", i + 1);
        balls[i] = LoadTexture(path);
    }

    while (!WindowShouldClose())
    {
        int ballXadd = 0, ballYadd = 0;

        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawTexture(bg, 0, 0, WHITE);

        switch (pageIndex)
        {
        case 3:

                // drawing ball images

                for (int i = 0; i < 6; i++)
                {
                    for (int j = 0; j < BALLCOLS; j++)
                    {
                        if ((i % 2) == 0)
                        {
                            ballXadd = BALLRADIUS;
                        }
                        else
                        {
                            ballXadd = 0;
                        }

                        /*if (i == 0)
                        {
                            ballYadd = BALLRADIUS;
                        }else{
                            ballYadd = 0;
                        }*/

                        DrawTexturePro(balls[randBallIdx[i][j]],
                                       (Rectangle){0, 0, balls[randBallIdx[i][j]].width, balls[randBallIdx[i][j]].height},
                                       (Rectangle){j * BALLRADIUS * 2 + ballXadd, i * (BALLRADIUS * 1.735), BALLRADIUS * 2, BALLRADIUS * 2},
                                       Vector2Zero(), 0, WHITE);
                    }
                }
            

            break;
        }

        EndDrawing();
    }

    UnloadTexture(bg);
    for (int i = 0; i < BALLNUM; i++)
    {
        UnloadTexture(balls[i]);
    }
    CloseWindow();

    return 0;
}