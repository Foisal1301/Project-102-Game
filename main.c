//<<<<<<< HEAD
//=======
#include "raylib.h"
#include "raymath.h"
#include <stdio.h>
#include <stdlib.h>

#define DEBUG 1
#define HEIGHT 600
#define WIDTH 800
#define TEXTCOLOR BLACK
#define BALLROWS 16
#define BALLCOLS 14
#define BALLNUM 2
#define BALLRADIUS ((WIDTH - 12) / (2 * BALLCOLS + 0.5))

/*
Pages
0 => Start
1 => Resume
2 => About
3 =>GamePlay
*/

// int randBalls(int ballNumber) // Swapno
// {

//     srand(time(NULL));
//     int x;
//     x = rand() % ballNumber + 1;
//     return x;
// }

int main(void)
{
    InitWindow(WIDTH, HEIGHT, "Bouncing Ball");
    SetTargetFPS(60);
    int pageIndex = DEBUG ? 3 : 0;
    int score = 0;

    // Global
    Texture2D bg = LoadTexture("assets/bg.png");

    // Menu Page
    int selectedOption = 0;
    int exit = 0;

    // GamePlay
    // Grid // Swapno
    int randBallIdx[BALLROWS][BALLCOLS];
    int gridIsFilled[BALLROWS][BALLCOLS];

    for (int i = 0; i < BALLROWS; i++)
    {
        for (int j = 0; j < BALLCOLS; j++)
        {
            gridIsFilled[i][j] = 0;
        }
    }

    for (int i = 0; i < 6; i++)
    {
        for (int j = 0; j < BALLCOLS; j++)
        {
            randBallIdx[i][j] = GetRandomValue(0, 1); // rand() % 2;
            gridIsFilled[i][j] = 1;
        }
    }
    Texture2D balls[BALLNUM];

    for (int i = 0; i < BALLNUM; i++)
    {
        char path[50];
        sprintf(path, "assets/ball_%d.png", i + 1);
        balls[i] = LoadTexture(path);
    }

    // Shooter
    bool shooted = false;
    Vector2 cannonBase = {WIDTH / 2.0f, HEIGHT * 0.95f};
    Vector2 cannonOrigin = {40, 120};
    float cannonAngle = 0;
    Texture2D shooters[3];
    for (int i = 1; i <= 3; i++)
    {
        char path[50];
        sprintf(path, "assets/shooter_%d.png", i);
        shooters[i - 1] = LoadTexture(path);
    }
    int shooterIndex = GetRandomValue(0, 1);

    Texture2D bullets[2];
    for (int i = 1; i <= 2; i++)
    {
        char path[50];
        sprintf(path, "assets/ball_%d.png", i);
        bullets[i - 1] = LoadTexture(path);
    }
    Vector2 bulletPosition = {0, 0};
    Vector2 bulletVelocity = {0, 0};

    while (!WindowShouldClose() && !exit)
    {
        int ballXadd = 0, ballYadd = 0;
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
            DrawText("Go Back", 10, 10, 30, TEXTCOLOR);
            break;

        case 3: // GamePlay
            if (IsKeyPressed(KEY_LEFT))
            {
                pageIndex = 1;
            }
            // drawing ball images //Swapno

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

            // shooter
            Vector2 mousePos = GetMousePosition();
            float dx = mousePos.x - cannonBase.x;
            float dy = mousePos.y - cannonBase.y;
            float tempAngle = (atan2f(dy, dx) * RAD2DEG);
            // if(tempAngle<=70 && tempAngle>=-65)
            cannonAngle = tempAngle;
            DrawTexturePro(
                shooters[shooterIndex],
                (Rectangle){0, 0, shooters[shooterIndex].width, shooters[shooterIndex].height},
                (Rectangle){
                    cannonBase.x, // cannonPos.x+50,//WIDTH / 2 - 40,
                    cannonBase.y, // cannonPos.y+100,//HEIGHT - 130,
                    80,
                    120},
                cannonOrigin, cannonAngle + 90, WHITE);
            if (IsKeyPressed(KEY_SPACE))
            {
                if (!shooted)
                {
                    shooted = true;
                    bulletPosition.x = cannonBase.x;
                    bulletPosition.y = cannonBase.y;
                    printf("%f\n", cannonAngle);
                    bulletVelocity.x = 20 * cos(cannonAngle);
                    bulletVelocity.y = 20 * sin(cannonAngle);
                    printf("%f\n", sin(3.1416 / 2));
                    if (bulletVelocity.y > 0)
                        bulletVelocity.y = -bulletVelocity.y;
                    printf("%f %f\n", GetMousePosition().x, GetMousePosition().y);
                }
            }

            if (shooted)
            {
                bulletPosition.x += bulletVelocity.x;
                bulletPosition.y += bulletVelocity.y;
                if (bulletPosition.x < 0)
                {
                    bulletPosition.x = 0;
                    bulletVelocity.x = -bulletVelocity.x;
                }
                else if (bulletPosition.x > WIDTH)
                {
                    bulletPosition.x = WIDTH;
                    bulletVelocity.x = -bulletVelocity.x;
                }
                if (bulletPosition.y < 0)
                {
                    bulletPosition.y = 0;
                    bulletVelocity.x = 0;
                    bulletVelocity.y = 0;
                    shooterIndex = GetRandomValue(0, 1);
                    shooted = false;
                }
                if (bulletPosition.x > 800)
                {
                    bulletPosition.x = 800;
                    bulletVelocity.x = -bulletVelocity.x;
                }
                

                DrawTexturePro(
                    bullets[shooterIndex],
                    (Rectangle){0, 0, bullets[shooterIndex].width, bullets[shooterIndex].height},
                    (Rectangle){bulletPosition.x, bulletPosition.y, BALLRADIUS * 2, BALLRADIUS * 2},
                    Vector2Zero(), 0, WHITE);
            }

            // score
            char scores[30];
            sprintf(scores, "SCORE : %d", score);
            DrawText(scores, 10, 10, 30, TEXTCOLOR);
            break;
        }
        EndDrawing();
    }

    UnloadTexture(bg);
    for (int i = 0; i < 3; i++)
    {
        UnloadTexture(shooters[i]);
    }
    for (int i = 0; i < BALLNUM; i++) // Swapno
    {
        UnloadTexture(balls[i]);
    }
    CloseWindow();

    return 0;
}
//>>>>>>> ce01f8a47b0c6a6d3587212f8be859fcd79cf9f7
