#include "raylib.h"
#include "raymath.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define HEIGHT 600
#define WIDTH 800
#define TEXTCOLOR RAYWHITE
#define BALLROWS 16
#define BALLCOLS 14
#define BALLNUM 3
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
3 => GamePlay
4 => GameOver
MAXY = 450
*/
Rectangle existedBalls[BALLROWS * BALLCOLS];
int randBallIdx[BALLROWS * BALLCOLS];
int ballXadd = 0, ballYadd = 0;
int ballIndex = 0;
int removedBalls=0;
int score = 0;
float timeRemaining = 30;
bool shooted;

// ২টি বিন্দুর দূরত্ব সূত্র
float GetDistance(Vector2 p1, Vector2 p2) {
    return sqrtf((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y));
}

// সবচেয়ে কাছের খালি গ্রিড ইনডেক্স খুঁজে বের করা
int FindClosestEmptyCell(Vector2 pos) {
    int closestIdx = -1;
    float minDistance = 999999.0f;

    for (int i = 0; i < BALLROWS * BALLCOLS; i++) {
        if (existedBalls[i].width == 0) {
            int r = i / BALLCOLS;
            int c = i % BALLCOLS;
            float xAdd = (r % 2 == 0) ? BALLRADIUS : 0;

            Vector2 cellPos = {
                c * BALLRADIUS * 2 + xAdd + BALLRADIUS,
                r * (BALLRADIUS * 1.735) + BALLRADIUS
            };

            float dist = GetDistance(pos, cellPos);
            if (dist < minDistance) {
                minDistance = dist;
                closestIdx = i;
            }
        }
    }
    return closestIdx;
}

// কানেক্টেড একই রঙের বল খোঁজার সহজ রিকার্সিভ ফাংশন (L1T1 Friendly)
void FindConnectedBalls(int index, int targetColor, bool visited[], int matchedIndices[], int *count) {
    visited[index] = true;
    matchedIndices[*count] = index;
    (*count)++;

    Vector2 p1 = {existedBalls[index].x + BALLRADIUS, existedBalls[index].y + BALLRADIUS};

    for (int i = 0; i < BALLROWS * BALLCOLS; i++) {
        if (!visited[i] && existedBalls[i].width > 0 && randBallIdx[i] == targetColor) {
            Vector2 p2 = {existedBalls[i].x + BALLRADIUS, existedBalls[i].y + BALLRADIUS};
            float dist = GetDistance(p1, p2);

            // সংলগ্ন ২টা বলের কেন্দ্রস্থলের দূরত্ব BALLRADIUS * 2.3 এর কম হলে কানেক্টেড ধরা হবে
            if (dist < BALLRADIUS * 2.3f) {
                FindConnectedBalls(i, targetColor, visited, matchedIndices, count);
            }
        }
    }
}

// ৩ বা তার বেশি বল থাকলে পপ/ভ্যানিশ করার মূল লজিক
void CheckSimpleMatches(int hitIndex) {
    bool visited[BALLROWS * BALLCOLS] = { false };
    int matchedIndices[BALLROWS * BALLCOLS];
    int count = 0;
    int targetColor = randBallIdx[hitIndex];

    FindConnectedBalls(hitIndex, targetColor, visited, matchedIndices, &count);

    if (count >= 3) {
        for (int i = 0; i < count; i++) {
            int idx = matchedIndices[i];
            existedBalls[idx] = (Rectangle){0, 0, 0, 0};
            removedBalls++;
        }
        score += count * 10;
    }
}

void NewGame(){
    timeRemaining = 30;
    removedBalls=0;
    score = 0;
    shooted=false;
    ballIndex=0;

    for (int i = 0; i < BALLROWS * BALLCOLS; i++) 
    {
        existedBalls[i] = (Rectangle){0, 0, 0, 0};
    }

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

            existedBalls[i * BALLCOLS + j] = (Rectangle){
                j * BALLRADIUS * 2 + ballXadd,
                i * (BALLRADIUS * 1.735),
                BALLRADIUS * 2,
                BALLRADIUS * 2};

            randBallIdx[i * BALLCOLS + j] = GetRandomValue(0, 2);
            ballIndex++;
        }
    }
}

int main(void) 
{
    InitWindow(WIDTH, HEIGHT, "Bouncing Ball");
    SetTargetFPS(60);
    int pageIndex = 0;


    // Global
    Texture2D bg = LoadTexture("assets/bg.png");

    // Menu Page
    int selectedOption = 0;
    int exit = 0;

    NewGame();

    Texture2D balls[BALLNUM + 1];

    for (int i = 0; i < BALLNUM; i++) 
    {
        char path[50];
        sprintf(path, "assets/ball_%d.png", i + 1);
        balls[i] = LoadTexture(path);
    }
    balls[BALLNUM] = LoadTexture("assets/ball_t.png");

    Vector2 cannonBase = {WIDTH / 2.0f, HEIGHT * 0.95f};
    Vector2 cannonOrigin = {CANNON_WIDTH / 2, CANNON_HEIGHT};
    float cannonAngle = 0;
    Texture2D shooters[BALLNUM];
    for (int i = 0; i < BALLNUM; i++) {
        char path[50];
        sprintf(path, "assets/shooter_%d.png", i + 1);
        shooters[i] = LoadTexture(path);
    }
    int shooterIndex = GetRandomValue(0, BALLNUM - 1);
    int shooterIndex2 = GetRandomValue(0, BALLNUM - 1);
    Vector2 bulletPosition = {0, 0};
    Vector2 bulletVelocity = {0, 0};

    int selected = 0;
    int selected2 = 0;

    while (!WindowShouldClose() && !exit) {
        BeginDrawing();
        DrawTexture(bg, 0, 0, WHITE);

        switch (pageIndex) {
        case 0: // Menu
            if (selectedOption == 0) DrawText("New Game", GetScreenWidth() / 2 - MeasureText("New Game", HOVER_FONTSIZE) / 2, TEXTPOSY, HOVER_FONTSIZE, TEXTCOLOR);
            else DrawText("New Game", GetScreenWidth() / 2 - MeasureText("New Game", FONTSIZE) / 2, TEXTPOSY, FONTSIZE, TEXTCOLOR);

            if (selectedOption == 1) DrawText("About", GetScreenWidth() / 2 - MeasureText("About", HOVER_FONTSIZE) / 2, TEXTPOSY + LINEGAPFORTEXT, HOVER_FONTSIZE, TEXTCOLOR);
            else DrawText("About", GetScreenWidth() / 2 - MeasureText("About", FONTSIZE) / 2, TEXTPOSY + LINEGAPFORTEXT, FONTSIZE, TEXTCOLOR);

            if (selectedOption == 2) DrawText("Exit", GetScreenWidth() / 2 - MeasureText("Exit", HOVER_FONTSIZE) / 2, TEXTPOSY + 2 * LINEGAPFORTEXT, HOVER_FONTSIZE, TEXTCOLOR);
            else DrawText("Exit", GetScreenWidth() / 2 - MeasureText("Exit", FONTSIZE) / 2, TEXTPOSY + 2 * LINEGAPFORTEXT, FONTSIZE, TEXTCOLOR);

            if (IsKeyPressed(KEY_UP) && selectedOption > 0) selectedOption--;
            if (IsKeyPressed(KEY_DOWN) && selectedOption < 2) selectedOption++;

            if (IsKeyPressed(KEY_ENTER)) {
                switch (selectedOption) {
                case 0: NewGame(); pageIndex = 3; break;
                case 1: pageIndex = 2; break;
                case 2: exit = 1; break;
                }
            }
            break;

        case 1: // Resume Page
            if (selected2 == 0) DrawText("Resume", WIDTH / 2 - MeasureText("Resume", HOVER_FONTSIZE) / 2, 200, HOVER_FONTSIZE, TEXTCOLOR);
            else DrawText("Resume", WIDTH / 2 - MeasureText("Resume", FONTSIZE) / 2, 200, FONTSIZE, TEXTCOLOR);

            if (selected2 == 1) DrawText("New Game", WIDTH / 2 - MeasureText("New Game", HOVER_FONTSIZE) / 2, 200 + LINEGAPFORTEXT, HOVER_FONTSIZE, TEXTCOLOR);
            else DrawText("New Game", WIDTH / 2 - MeasureText("New Game", FONTSIZE) / 2, 200 + LINEGAPFORTEXT, FONTSIZE, TEXTCOLOR);

            if (selected2 == 2) DrawText("Main Menu", WIDTH / 2 - MeasureText("Main Menu", HOVER_FONTSIZE) / 2, 200 + 2 * LINEGAPFORTEXT, HOVER_FONTSIZE, TEXTCOLOR);
            else DrawText("Main Menu", WIDTH / 2 - MeasureText("Main Menu", FONTSIZE) / 2, 200 + 2 * LINEGAPFORTEXT, FONTSIZE, TEXTCOLOR);

            if (selected2 == 3) DrawText("Exit", WIDTH / 2 - MeasureText("Exit", HOVER_FONTSIZE) / 2, 200 + 3 * LINEGAPFORTEXT, HOVER_FONTSIZE, TEXTCOLOR);
            else DrawText("Exit", WIDTH / 2 - MeasureText("Exit", FONTSIZE) / 2, 200 + 3 * LINEGAPFORTEXT, FONTSIZE, TEXTCOLOR);

            if (IsKeyPressed(KEY_UP) && selected2 > 0) selected2--;
            if (IsKeyPressed(KEY_DOWN) && selected2 < 3) selected2++;

            if (IsKeyPressed(KEY_ENTER)) {
                if (selected2 == 0) pageIndex = 3;
                else if (selected2 == 1) { NewGame(); pageIndex = 3; }
                else if (selected2 == 2) { NewGame(); pageIndex = 0; }
                else exit = true;
                selected2 = 0;
            }
            break;

        case 2: // About Page
            if (IsKeyPressed(KEY_LEFT)) pageIndex = 0;
            DrawRectangle(10, 40, WIDTH - 20, HEIGHT - 50, Fade(BLACK, 0.7f));
            DrawText("About", GetScreenWidth() / 2 - MeasureText("About", HOVER_FONTSIZE) / 2, 60, HOVER_FONTSIZE, TEXTCOLOR);
            DrawText("Go Back", 10, 10, 30, TEXTCOLOR);
            DrawText("Bouncing Ball", GetScreenWidth() / 2 - MeasureText("Bouncing Ball", FONTSIZE) / 2, 60 + LINEGAPFORTEXT, FONTSIZE, TEXTCOLOR);
            DrawText("A simple ball bouncing game made with C and Raylib", 10, 200, 30, TEXTCOLOR);
            DrawText("Developers: Md. Foisal and Shahariar Sajid Swapno", 10, 200 + LINEGAPFORTEXT, 30, TEXTCOLOR);
            DrawText("CONTROLS", GetScreenWidth() / 2 - MeasureText("CONTROLS", 30) / 2, 200 + 2 * LINEGAPFORTEXT, 30, TEXTCOLOR);
            DrawText("SPACE/MOUSE-LEFT: SHOOT", 10, 200 + 3 * LINEGAPFORTEXT, 30, TEXTCOLOR);
            DrawText("LEFT ARROW: RESUME GAME", 10, 200 + 3.5 * LINEGAPFORTEXT, 30, TEXTCOLOR);
            break;

        case 3: // GamePlay
            timeRemaining -= GetFrameTime();
            DrawRectangle(0, 0, WIDTH, HEIGHT, Fade(BLACK, 0.7f));

            if (timeRemaining < 0) pageIndex = 4;
            if (IsKeyPressed(KEY_LEFT)) pageIndex = 1;

            // বিদ্যমান বলসমূহ রেন্ডার
            for (int i = 0; i < BALLROWS * BALLCOLS; i++) {
                if (existedBalls[i].width > 0) {
                    DrawTexturePro(balls[randBallIdx[i]],
                        (Rectangle){0, 0, balls[randBallIdx[i]].width, balls[randBallIdx[i]].height},
                        existedBalls[i],
                        Vector2Zero(), 0, WHITE);
                }
            }

            // শুটার মোড়ানোর এঙ্গেল হিসেব
            Vector2 mousePos = GetMousePosition();
            float dx = mousePos.x - cannonBase.x;
            float dy = mousePos.y - cannonBase.y;
            float tempAngle = (atan2f(dy, dx) * RAD2DEG);
            if (tempAngle <= -35 && tempAngle >= -145) cannonAngle = tempAngle;

            DrawTexturePro(
                shooters[shooterIndex],
                (Rectangle){0, 0, shooters[shooterIndex].width, shooters[shooterIndex].height},
                (Rectangle){cannonBase.x, cannonBase.y, CANNON_WIDTH, CANNON_HEIGHT},
                cannonOrigin, cannonAngle + 90, WHITE
            );

            // ফায়ার লজিক
            if (IsKeyPressed(KEY_SPACE) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                if (!shooted) {
                    shooted = true;
                    bulletPosition.x = cannonBase.x + CANNON_HEIGHT * cos(cannonAngle * DEG2RAD) - BALLRADIUS;
                    bulletPosition.y = cannonBase.y + CANNON_HEIGHT * sin(cannonAngle * DEG2RAD) - BALLRADIUS;
                    bulletVelocity.x = VELOCITY_OF_BULLET * cos(cannonAngle * DEG2RAD);
                    bulletVelocity.y = VELOCITY_OF_BULLET * sin(cannonAngle * DEG2RAD);
                    if (bulletVelocity.y > 0) bulletVelocity.y = -bulletVelocity.y;
                }
            }

            // বুলেটের ফিজিক্স এবং কলিশন
            if (shooted) {
                Rectangle shootedBall = (Rectangle){bulletPosition.x, bulletPosition.y, BALLRADIUS * 2, BALLRADIUS * 2};
                bulletPosition.x += bulletVelocity.x;
                bulletPosition.y += bulletVelocity.y;

                // ওয়ালে রিফ্লেকশন/বাউন্স
                if (bulletPosition.x <= 0 || bulletPosition.x >= WIDTH - BALLRADIUS * 2) {
                    bulletVelocity.x = -bulletVelocity.x;
                }

                // ওপরের ছাদের সাথে আটকে যাওয়ার লজিক
                if (bulletPosition.y <= 0) {
                    Vector2 bulletCenter = {bulletPosition.x + BALLRADIUS, bulletPosition.y + BALLRADIUS};
                    int targetIdx = FindClosestEmptyCell(bulletCenter);

                    if (targetIdx != -1) {
                        int r = targetIdx / BALLCOLS;
                        int c = targetIdx % BALLCOLS;
                        float xAdd = (r % 2 == 0) ? BALLRADIUS : 0;

                        existedBalls[targetIdx] = (Rectangle){
                            c * BALLRADIUS * 2 + xAdd,
                            r * (BALLRADIUS * 1.735),
                            BALLRADIUS * 2, BALLRADIUS * 2
                        };
                        randBallIdx[targetIdx] = shooterIndex;

                        CheckSimpleMatches(targetIdx);
                    }

                    shooted = false;
                    shooterIndex = shooterIndex2;
                    shooterIndex2 = GetRandomValue(0, BALLNUM - 1);
                }

                // গ্রিডের বলের সাথে হিট লজিক
                bool isCollision = false;
                for (int i = 0; i < BALLROWS * BALLCOLS && !isCollision; i++) {
                    if (existedBalls[i].width > 0 && CheckCollisionRecs(shootedBall, existedBalls[i])) {
                        isCollision = true;

                        Vector2 bulletCenter = {bulletPosition.x + BALLRADIUS, bulletPosition.y + BALLRADIUS};
                        int targetIdx = FindClosestEmptyCell(bulletCenter);

                        if (targetIdx != -1) {
                            int r = targetIdx / BALLCOLS;
                            int c = targetIdx % BALLCOLS;
                            float xAdd = (r % 2 == 0) ? BALLRADIUS : 0;

                            existedBalls[targetIdx] = (Rectangle){
                                c * BALLRADIUS * 2 + xAdd,
                                r * (BALLRADIUS * 1.735),
                                BALLRADIUS * 2, BALLRADIUS * 2
                            };
                            randBallIdx[targetIdx] = shooterIndex;

                            // Match-3 চেক এবং পপ করা
                            CheckSimpleMatches(targetIdx);

                            // ৪৫০ পিক্সেলের নিচে বল পৌঁছালে গেম ওভার
                            if ((r * (BALLRADIUS * 1.735)) >= 450) {
                                pageIndex = 4;
                            }
                        }

                        shooted = false;
                        shooterIndex = shooterIndex2;
                        shooterIndex2 = GetRandomValue(0, BALLNUM - 1);
                    }
                }

                DrawTexturePro(
                    balls[shooterIndex],
                    (Rectangle){0, 0, balls[shooterIndex].width, balls[shooterIndex].height},
                    (Rectangle){bulletPosition.x, bulletPosition.y, BALLRADIUS * 2, BALLRADIUS * 2},
                    Vector2Zero(), 0, WHITE
                );
            }

            // Score & UI Interface
            char scores[30];
            sprintf(scores, "SCORE : %d", score);
            char time[20];
            sprintf(time, "TIME: %.0f", timeRemaining);
            DrawText(scores, 10, HEIGHT - MeasureTextEx(GetFontDefault(), scores, FONTSIZE, FONTSIZE / 10).y, FONTSIZE, TEXTCOLOR);

            int nextTextHeight = MeasureTextEx(GetFontDefault(), "NEXT: ", FONTSIZE, FONTSIZE / 10).y;
            int timeHeight = MeasureTextEx(GetFontDefault(), time, FONTSIZE, FONTSIZE / 10).y + nextTextHeight;
            DrawText(time, WIDTH - MeasureText(time, FONTSIZE) - 20, HEIGHT - timeHeight, FONTSIZE, TEXTCOLOR);
            DrawText("NEXT: ", WIDTH - MeasureText("NEXT: ", FONTSIZE) - 70, HEIGHT - nextTextHeight, FONTSIZE, TEXTCOLOR);

            Rectangle nextColor = {WIDTH - 60, HEIGHT - MeasureTextEx(GetFontDefault(), "NEXT: ", FONTSIZE, FONTSIZE / 10).y, 55, FONTSIZE};
            switch (shooterIndex2) {
            case 0: DrawRectangleRec(nextColor, (Color){255, 76, 97, 255}); break;
            case 1: DrawRectangleRec(nextColor, (Color){65, 159, 221, 255}); break;
            case 2: DrawRectangleRec(nextColor, (Color){67, 153, 108, 255}); break;
            }
            break;

        case 4: // GameOver Page
            char finalScores[30];
            sprintf(finalScores, "SCORE : %d", score);
            DrawText("Game Over", WIDTH / 2 - MeasureText("Game Over", FONTSIZE * 2) / 2, 20, FONTSIZE * 2, WHITE);
            DrawText(finalScores, WIDTH / 2 - MeasureText(finalScores, FONTSIZE) / 2, 200, FONTSIZE, WHITE);

            if (selected == 0) {
                DrawText("New Game", 200 - MeasureText("New Game", FONTSIZE) / 2, 300, HOVER_FONTSIZE, TEXTCOLOR);
                DrawText("Exit", 600 - MeasureText("Exit", FONTSIZE) / 2, 300, FONTSIZE, TEXTCOLOR);
            } else {
                DrawText("New Game", 200 - MeasureText("New Game", FONTSIZE) / 2, 300, FONTSIZE, TEXTCOLOR);
                DrawText("Exit", 600 - MeasureText("Exit", FONTSIZE) / 2, 300, HOVER_FONTSIZE, TEXTCOLOR);
            }

            if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_LEFT)) selected = !selected;
            if (IsKeyPressed(KEY_ENTER)) {
                if (selected == 1) exit = true;
                else {
                    NewGame();
                    selected = 0;
                    pageIndex = 3;
                }
            }
            break;
        }
        EndDrawing();
    }

    UnloadTexture(bg);
    for (int i = 0; i <= BALLNUM; i++) UnloadTexture(balls[i]);
    for (int i = 0; i < BALLNUM; i++) UnloadTexture(shooters[i]);

    CloseWindow();
    return 0;
}