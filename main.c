#include "raylib.h"
#include "raymath.h"
#include <stdio.h>
#include <stdlib.h>

#define DEBUG 0
#define HEIGHT 600
#define WIDTH 800
#define TEXTCOLOR BLACK
#define BALLROWS 16
#define BALLCOLS 14
#define BALLNUM 2
#define BALLRADIUS ((WIDTH - 12) / (2 * BALLCOLS + 0.5))
#define CANNON_HEIGHT 120
#define CANNON_WIDTH 80
#define VELOCITY_OF_BULLET 20
#define FONTSIZE 40
#define HOVER_FONTSIZE 60
#define LINEGAPFORTEXT 80
#define TEXTPOSY 200
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
    int pageIndex = 0;
    int score = 0;

    // Global
    Texture2D bg = LoadTexture("assets/bg.png");

    // Menu Page
    int selectedOption = 0;
    int exit = 0;

    // GamePlay
    // Grid // Swapno
    int randBallIdx[6][BALLCOLS];
    for (int i = 0; i < 6; i++)
    {
        for (int j = 0; j < BALLCOLS; j++)
        {
            randBallIdx[i][j] = GetRandomValue(0,1);//rand() % 2;
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
    Vector2 cannonBase = {WIDTH / 2.0f,HEIGHT * 0.95f};
    Vector2 cannonOrigin = {CANNON_WIDTH/2,CANNON_HEIGHT};
    float cannonAngle = 0;
    Texture2D shooters[3];
    for(int i=1;i<=3;i++){
        char path[50];
        sprintf(path,"assets/shooter_%d.png",i);
        shooters[i-1] = LoadTexture(path);
    }
    int shooterIndex = GetRandomValue(0,1);

    Texture2D bullets[2];
    for(int i=1;i<=2;i++){
        char path[50];
        sprintf(path,"assets/ball_%d.png",i);
        bullets[i-1] = LoadTexture(path);
    }
    Vector2 bulletPosition = {0,0};
    Vector2 bulletVelocity = {0,0};

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
                DrawText("New Game", GetScreenWidth() / 2 - MeasureText("New Game", HOVER_FONTSIZE) / 2, TEXTPOSY, HOVER_FONTSIZE, TEXTCOLOR); // Hover effect
            }
            else
            {
                DrawText("New Game", GetScreenWidth() / 2 - MeasureText("New Game", FONTSIZE) / 2, TEXTPOSY, FONTSIZE, TEXTCOLOR);
            }

            if (selectedOption == 1)
            {
                DrawText("About", GetScreenWidth() / 2 - MeasureText("About", HOVER_FONTSIZE) / 2, TEXTPOSY+LINEGAPFORTEXT, HOVER_FONTSIZE, TEXTCOLOR); // Hover effect
            }
            else
            {
                DrawText("About", GetScreenWidth() / 2 - MeasureText("About", FONTSIZE) / 2, TEXTPOSY+LINEGAPFORTEXT, FONTSIZE, TEXTCOLOR);
            }

            if (selectedOption == 2)
            {
                DrawText("Exit", GetScreenWidth() / 2 - MeasureText("Exit", HOVER_FONTSIZE) / 2, TEXTPOSY+2*LINEGAPFORTEXT, HOVER_FONTSIZE, TEXTCOLOR); // Hover effect
            }
            else
            {
                DrawText("Exit", GetScreenWidth() / 2 - MeasureText("Exit", FONTSIZE) / 2, TEXTPOSY+2*LINEGAPFORTEXT, FONTSIZE, TEXTCOLOR);
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
            float tempAngle = (atan2f(dy,dx) * RAD2DEG);
            if(tempAngle<=-35 && tempAngle>=-145)
                cannonAngle = tempAngle;
            
            DrawTexturePro(
                shooters[shooterIndex],
                (Rectangle){0,0,shooters[shooterIndex].width,shooters[shooterIndex].height},
                (Rectangle){
                    cannonBase.x,// cannonPos.x+50,//WIDTH / 2 - 40,
                    cannonBase.y,// cannonPos.y+100,//HEIGHT - 130,
                    CANNON_WIDTH,
                    CANNON_HEIGHT
                },
                cannonOrigin ,cannonAngle+90,WHITE
            );
            if (IsKeyPressed(KEY_SPACE)) {
                if(!shooted){
                    shooted=true;
                    bulletPosition.x = cannonBase.x + CANNON_HEIGHT*cos(cannonAngle*DEG2RAD);
                    bulletPosition.y = cannonBase.y + CANNON_HEIGHT*sin(cannonAngle*DEG2RAD);
                    bulletVelocity.x = VELOCITY_OF_BULLET*cos(cannonAngle*DEG2RAD);
                    bulletVelocity.y = VELOCITY_OF_BULLET*sin(cannonAngle*DEG2RAD);
                    if(bulletVelocity.y>0) bulletVelocity.y = -bulletVelocity.y;
                }
            }

            if(shooted){
                bulletPosition.x += bulletVelocity.x;
                bulletPosition.y += bulletVelocity.y;
                if(bulletPosition.x<0){
                    bulletPosition.x = 0;
                    bulletVelocity.x = -bulletVelocity.x;
                }else if(bulletPosition.x>=WIDTH){
                    bulletPosition.x = WIDTH;
                    bulletVelocity.x = -bulletVelocity.x;
                }
                if(bulletPosition.y<0){
                    bulletPosition.y = 0;
                    bulletVelocity.x = 0;
                    bulletVelocity.y = 0;
                    shooterIndex = GetRandomValue(0,1);
                    shooted = false;
                }
                if(bulletPosition.x>800){
                    bulletPosition.x = 800;
                    bulletVelocity.x = -bulletVelocity.x;
                }
                
                DrawTexturePro(
                    bullets[shooterIndex],
                    (Rectangle){0,0,bullets[shooterIndex].width,bullets[shooterIndex].height},
                    (Rectangle){bulletPosition.x,bulletPosition.y,BALLRADIUS * 2, BALLRADIUS * 2},
                    Vector2Zero(),0,WHITE
                );
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
    for (int i = 0; i < BALLNUM; i++) // Swapno
    {
        UnloadTexture(balls[i]);
    }
    CloseWindow();

    return 0;
}