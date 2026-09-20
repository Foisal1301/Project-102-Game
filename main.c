#include "raylib.h"
#include "raymath.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define HEIGHT 600
#define WIDTH 800
#define TEXTCOLOR RAYWHITE
#define TEXTCOLOR2 BLACK
#define HINTTEXTCOLOR SKYBLUE
#define FONTSIZE 40
#define HOVER_FONTSIZE 60
#define LINEGAPFORTEXT 80
#define TEXTPOSY 200
#define BALLROWS 16
#define BALLCOLS 14
#define BALLNUM 3
#define BALLRADIUS ((WIDTH - 12) / (2 * BALLCOLS + 0.5))
#define CANNON_HEIGHT 120
#define CANNON_WIDTH 80
#define VELOCITY_OF_BULLET 20
#define TIMEREMAINING 75
#define BGMVOLUME 2
#define ANIMATION_TIME 0.1
/*
Pages
0 => Start
1 => Resume
2 => About
3 => GamePlay
4 => GameOver
5 => LeaderBoard
*/
Rectangle existedBalls[BALLROWS * BALLCOLS];
int randBallIdx[BALLROWS * BALLCOLS];
int ballXadd = 0, ballYadd = 0;
int ballIndex = 0;
int removedBalls=0;

int score = 0;
int pageIndex = 0;
float timeRemaining;
float timePassed;
float blastAnimation;
bool shooted;
int high_scores[5];
char gameOverReason[80];
bool changeColor = false;
void NewGame(){
    ballIndex=0;
    removedBalls=0;
    score = 0;
    timeRemaining = TIMEREMAINING;
    blastAnimation = 0;
    timePassed = 0;
    shooted=false;
    changeColor=false;
    strcpy(gameOverReason,"YOU WON!");

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
            
            int probabilityForTimeBall = GetRandomValue(0,9);
            if(probabilityForTimeBall==9) randBallIdx[i * BALLCOLS + j] = 3;
            else randBallIdx[i * BALLCOLS + j] = GetRandomValue(0, 2);
            ballIndex++;
        }
    }
}

void GameOver(){
    pageIndex=4;

    int index = -1;
    for(int i=0;i<5;i++){
        if(high_scores[i]<score){
            index = i;
            break;
        }
    }
    if(index!=-1){
        for(int i=4;i>index;i--) high_scores[i] = high_scores[i-1];
        high_scores[index] = score;

        FILE *highScoreFile=fopen("highScore.txt","w");
        if(highScoreFile!=NULL){
            for(int i=0;i<5;i++){
                fprintf(highScoreFile,"%d\n",high_scores[i]);
            }
            fclose(highScoreFile);
        }
    }
}

float GetDistance(Vector2 p1, Vector2 p2)
{
    return sqrtf((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y));
}

int placingIndex(Vector2 pos)
{
    int closestIdx = -1;
    float minDistance = 10000000.0;

    for (int i = 0; i < BALLROWS * BALLCOLS; i++)
    {
        if (existedBalls[i].width == 0)
        {
            int r = i / BALLCOLS;
            int c = i % BALLCOLS;
            float xAdd = (r % 2 == 0) ? BALLRADIUS : 0;

            Vector2 cellPos = {
                c * BALLRADIUS * 2 + xAdd + BALLRADIUS,
                r * (BALLRADIUS * 1.735) + BALLRADIUS};

            float dist = GetDistance(pos, cellPos);
            if (dist < minDistance)
            {
                minDistance = dist;
                closestIdx = i;
            }
        }
    }
    return closestIdx;
}

void FindConnectedBalls(int index, int targetColor, bool visited[], int matchedIndices[], int *count)
{
    visited[index] = true;
    matchedIndices[*count] = index;
    (*count)++;

    Vector2 p1 = {existedBalls[index].x + BALLRADIUS, existedBalls[index].y + BALLRADIUS};

    for (int i = 0; i < BALLROWS * BALLCOLS; i++)
    {
        if (!visited[i] && existedBalls[i].width > 0 && randBallIdx[i] == targetColor)
        {
            Vector2 p2 = {existedBalls[i].x + BALLRADIUS, existedBalls[i].y + BALLRADIUS};
            float dist = GetDistance(p1, p2);

            if (dist < BALLRADIUS * 2.2)
            {
                FindConnectedBalls(i, targetColor, visited, matchedIndices, count);
            }
        }
    }
}

bool CheckSimpleMatches(int hitIndex)
{
    bool isVanished = false;
    bool visited[BALLROWS * BALLCOLS] = {false};
    int matchedIndices[BALLROWS * BALLCOLS];
    int count = 0;
    int targetColor = randBallIdx[hitIndex];

    FindConnectedBalls(hitIndex, targetColor, visited, matchedIndices, &count);

    if (count >= 3)
    {
        for (int i = 0; i < count; i++)
        {
            int idx = matchedIndices[i];
            existedBalls[idx] = (Rectangle){0, 0, 0, 0};
            removedBalls++;
        }
        score += count * 10;
        isVanished=true;
    }
    return isVanished;
}

int main(void)
{
    InitWindow(WIDTH, HEIGHT, "Bouncing Ball");

    // BGM
    InitAudioDevice();
    Music menuBgm = LoadMusicStream("assets/sounds/menu.wav");
    Music gameplayResumeBgm = LoadMusicStream("assets/sounds/Gameplay_Resume.mp3");
    Music gameOverBgm = LoadMusicStream("assets/sounds/gameover.mp3");
    Music countDown = LoadMusicStream("assets/sounds/countdown.wav");
    Music *bgm = &menuBgm;

    // Sound effect
    Sound blast = LoadSound("assets/sounds/blast.mp3");
    Sound bonus = LoadSound("assets/sounds/Bonus.wav");
    Sound click = LoadSound("assets/sounds/Click.mp3");
    Sound navigate = LoadSound("assets/sounds/navigate.mp3");
    Sound gameover = LoadSound("assets/sounds/gameover_instant.mp3");
    Sound winSound = LoadSound("assets/sounds/win.wav");
    
    int vol = BGMVOLUME;
    PlayMusicStream(*bgm);
    PlayMusicStream(countDown);

    SetTargetFPS(60);

    // score
    FILE *highScoreFile=fopen("highScore.txt","r");
    if(highScoreFile!=NULL){
        for(int i=0;i<5;i++){
            int status = fscanf(highScoreFile,"%d",&high_scores[i]);
            if(status!=1){
                high_scores[i] = 0;
            }
        }
        fclose(highScoreFile);
    }else{ // creating file for highscores
        for (int i = 0; i < 5; i++) {
            high_scores[i] = 0;
        }
        FILE* file = fopen("highScore.txt", "w");
        if (file != NULL) {
            for (int i = 0; i < 5; i++) {
                fprintf(file, "%d\n", high_scores[i]);
            }
            fclose(file);
        }
    }
    // Global
    Texture2D bg = LoadTexture("assets/bg.png");
    Texture2D logo = LoadTexture("assets/logo.png");

    // Menu Page
    int selectedOption = 0;
    int exit = 0;

    // GameOver
    int selected = 0;

    // resume
    int selected2 = 0;
    const char *Resumehint = "[ PRESS 'P' TO PAUSE ]";
    int ResumehintWidth = MeasureText(Resumehint, 18);

    // LeaderBoard & About
    const char *backHint = "[ PRESS 'E' TO GO BACK ]";
    int hintWidth = MeasureText(backHint, 18);

    NewGame();
    Texture2D balls[BALLNUM+1];

    for (int i = 0; i < BALLNUM; i++)
    {
        char path[50];
        sprintf(path, "assets/ball_%d.png", i + 1);
        balls[i] = LoadTexture(path);
    }
    balls[BALLNUM] = LoadTexture("assets/ball_t.png");

    // Shooter
    Vector2 cannonBase = {WIDTH / 2.0f,HEIGHT * 0.95f};
    Vector2 cannonOrigin = {CANNON_WIDTH/2,CANNON_HEIGHT};
    float cannonAngle = 0;
    Texture2D shooters[BALLNUM];
    for(int i=0;i<BALLNUM;i++){
        char path[50];
        sprintf(path,"assets/shooter_%d.png",i+1);
        shooters[i] = LoadTexture(path);
    }

    // Blast
    Texture2D blastShooters[BALLNUM];
    for(int i=0;i<BALLNUM;i++){
        char path[50];
        sprintf(path,"assets/shooter_%d_blast.png",i+1);
        blastShooters[i] = LoadTexture(path);
    }

    int shooterIndex = GetRandomValue(0,BALLNUM-1);
    int shooterIndex2 = GetRandomValue(0,BALLNUM-1);
    Vector2 bulletPosition = {0,0};
    Vector2 bulletVelocity = {0,0};

    while (!WindowShouldClose() && !exit)
    {   
        char vol_text[30];
        if(vol!=0) strcpy(vol_text,"TURN OFF MUSIC");
        else strcpy(vol_text,"TURN ON MUSIC");

        UpdateMusicStream(*bgm);
        BeginDrawing();
        DrawTexture(bg, 0, 0, WHITE);
        if(IsKeyPressed(KEY_UP)||IsKeyPressed(KEY_DOWN)||IsKeyPressed(KEY_LEFT)||IsKeyPressed(KEY_RIGHT)) PlaySound(navigate);
        if(IsKeyPressed(KEY_ENTER)) PlaySound(click);
        switch (pageIndex)
        {
        case 0: // Menu
            if(bgm != &menuBgm){
                StopMusicStream(*bgm);
                bgm = &menuBgm;
                PlayMusicStream(*bgm);
            }
            SetMusicVolume(*bgm,vol);
            DrawTexturePro(logo,
                (Rectangle){0,0,logo.width,logo.height},
                (Rectangle){WIDTH/6,5,WIDTH/1.5,HEIGHT/3},
                Vector2Zero(),0,WHITE);
            if (selectedOption == 0)
            {
                DrawText("NEW GAME", GetScreenWidth() / 2 - MeasureText("NEW GAME", HOVER_FONTSIZE) / 2, TEXTPOSY, HOVER_FONTSIZE, TEXTCOLOR); // Hover effect
            }
            else
            {
                DrawText("NEW GAME", GetScreenWidth() / 2 - MeasureText("NEW GAME", FONTSIZE) / 2, TEXTPOSY, FONTSIZE, TEXTCOLOR);
            }

            if (selectedOption == 1)
            {
                DrawText("ABOUT", GetScreenWidth() / 2 - MeasureText("ABOUT", HOVER_FONTSIZE) / 2, TEXTPOSY+LINEGAPFORTEXT, HOVER_FONTSIZE, TEXTCOLOR); // Hover effect
            }
            else
            {
                DrawText("ABOUT", GetScreenWidth() / 2 - MeasureText("ABOUT", FONTSIZE) / 2, TEXTPOSY+LINEGAPFORTEXT, FONTSIZE, TEXTCOLOR);
            }

            if (selectedOption == 2)
            {
                DrawText("LEADERBOARD", GetScreenWidth() / 2 - MeasureText("LEADERBOARD", HOVER_FONTSIZE) / 2, TEXTPOSY+2*LINEGAPFORTEXT, HOVER_FONTSIZE, TEXTCOLOR); // Hover effect
            }
            else
            {
                DrawText("LEADERBOARD", GetScreenWidth() / 2 - MeasureText("LEADERBOARD", FONTSIZE) / 2, TEXTPOSY+2*LINEGAPFORTEXT, FONTSIZE, TEXTCOLOR);
            }
            if (selectedOption == 3)
            {
                DrawText(vol_text, GetScreenWidth() / 2 - MeasureText(vol_text, HOVER_FONTSIZE) / 2, TEXTPOSY+3*LINEGAPFORTEXT, HOVER_FONTSIZE, TEXTCOLOR); // Hover effect
            }
            else
            {
                DrawText(vol_text, GetScreenWidth() / 2 - MeasureText(vol_text, FONTSIZE) / 2, TEXTPOSY+3*LINEGAPFORTEXT, FONTSIZE, TEXTCOLOR);
            }

            if (selectedOption == 4)
            {
                DrawText("EXIT", GetScreenWidth() / 2 - MeasureText("EXIT", HOVER_FONTSIZE) / 2, TEXTPOSY+4*LINEGAPFORTEXT, HOVER_FONTSIZE, TEXTCOLOR); // Hover effect
            }
            else
            {
                DrawText("EXIT", GetScreenWidth() / 2 - MeasureText("EXIT", FONTSIZE) / 2, TEXTPOSY+4*LINEGAPFORTEXT, FONTSIZE, TEXTCOLOR);
            }

            if (IsKeyPressed(KEY_UP) && selectedOption > 0) 
                selectedOption--;
            
            if (IsKeyPressed(KEY_DOWN) && selectedOption < 4)
                selectedOption++;

            // Page Shifting
            if (IsKeyPressed(KEY_ENTER))
            {
                switch (selectedOption)
                {
                case 0:// GamePlay
                    selectedOption = 0;
                    pageIndex = 3;
                    break;

                case 1://About
                    selectedOption = 0;
                    pageIndex = 2;
                    break;
                case 2://Leaderboard
                    selectedOption = 0;
                    pageIndex = 5;
                    break;
                case 3:
                    if(vol==0) vol=BGMVOLUME;
                    else vol=0;
                    break;
                case 4://Exit
                    selectedOption = 0;
                    exit = 1;
                    break;
                }
            }

            break;
        
        case 1: // Resume page
            if(bgm != &gameplayResumeBgm){
                StopMusicStream(*bgm);
                bgm = &gameplayResumeBgm;
                PlayMusicStream(*bgm);
            }
            SetMusicVolume(*bgm,vol);
            // resume,new game,exit
            if(selected2==0){
                DrawText("RESUME",WIDTH/2 - MeasureText("RESUME",HOVER_FONTSIZE)/2,TEXTPOSY,HOVER_FONTSIZE,TEXTCOLOR);
            }else{
                DrawText("RESUME",WIDTH/2 - MeasureText("RESUME",FONTSIZE)/2,TEXTPOSY,FONTSIZE,TEXTCOLOR);
            }

            if(selected2==1){
                DrawText("NEW GAME",WIDTH/2 - MeasureText("NEW GAME",HOVER_FONTSIZE)/2,TEXTPOSY+LINEGAPFORTEXT,HOVER_FONTSIZE,TEXTCOLOR);
            }else{
                DrawText("NEW GAME",WIDTH/2 - MeasureText("NEW GAME",FONTSIZE)/2,TEXTPOSY+LINEGAPFORTEXT,FONTSIZE,TEXTCOLOR);
            }

            if(selected2==2){
                DrawText("MAIN MENU",WIDTH/2 - MeasureText("MAIN MENU",HOVER_FONTSIZE)/2,TEXTPOSY+2*LINEGAPFORTEXT,HOVER_FONTSIZE,TEXTCOLOR);
            }else{
                DrawText("MAIN MENU",WIDTH/2 - MeasureText("MAIN MENU",FONTSIZE)/2,TEXTPOSY+2*LINEGAPFORTEXT,FONTSIZE,TEXTCOLOR);
            }

            if(selected2==3){
                DrawText(vol_text,WIDTH/2 - MeasureText(vol_text,HOVER_FONTSIZE)/2,TEXTPOSY+3*LINEGAPFORTEXT,HOVER_FONTSIZE,TEXTCOLOR);
            }else{
                DrawText(vol_text,WIDTH/2 - MeasureText(vol_text,FONTSIZE)/2,TEXTPOSY+3*LINEGAPFORTEXT,FONTSIZE,TEXTCOLOR);
            }

            if(selected2==4){
                DrawText("EXIT",WIDTH/2 - MeasureText("EXIT",HOVER_FONTSIZE)/2,TEXTPOSY+4*LINEGAPFORTEXT,HOVER_FONTSIZE,TEXTCOLOR);
            }else{
                DrawText("EXIT",WIDTH/2 - MeasureText("EXIT",FONTSIZE)/2,TEXTPOSY+4*LINEGAPFORTEXT,FONTSIZE,TEXTCOLOR);
            }

            if (IsKeyPressed(KEY_UP) && selected2>0) selected2--;
            if (IsKeyPressed(KEY_DOWN) && selected2<4) selected2++;
            
            if (IsKeyPressed(KEY_ENTER)){
                if(selected2==0) pageIndex = 3;
                else if(selected2==1){
                    NewGame();
                    pageIndex=3;
                }else if(selected2==2){
                    NewGame();
                    pageIndex=0; 
                }else if(selected2==3){
                    if(vol==0) vol = BGMVOLUME;
                    else vol = 0;
                }
                else exit = true;
                selected2 = 0;
            }
            break;
        case 2: // About
            if(bgm != &menuBgm){
                StopMusicStream(*bgm);
                bgm = &menuBgm;
                PlayMusicStream(*bgm);
            }
            SetMusicVolume(*bgm,vol);
            if (IsKeyPressed(KEY_E))
            {
                PlaySound(navigate);
                pageIndex = 0;
            }

            DrawRectangle(10, 40, WIDTH-20, HEIGHT-50, Fade(BLACK, 0.7f));
            DrawText(backHint, (WIDTH - hintWidth) / 2, 0 + HEIGHT - 35, 18, HINTTEXTCOLOR);
            DrawText("ABOUT", GetScreenWidth() / 2 - MeasureText("ABOUT", HOVER_FONTSIZE) / 2, 60, HOVER_FONTSIZE, TEXTCOLOR);
            DrawText("BOUNCING BALL", GetScreenWidth() / 2 - MeasureText("BOUNCING BALL", FONTSIZE) / 2, 60+LINEGAPFORTEXT, FONTSIZE, TEXTCOLOR);
            DrawText("A SIMPLE BOUNCING BALL GAME MADE WITH C & RAYLIB", 10 , TEXTPOSY, 25, TEXTCOLOR);
            DrawText("DEVELOPERS: MD. FOISAL & SHAHARIAR SAJID SWAPNO", 10 , TEXTPOSY+2*LINEGAPFORTEXT*0.5, 25, TEXTCOLOR);
            break;

        case 3: // GamePlay
            if(bgm != &gameplayResumeBgm){
                StopMusicStream(*bgm);
                bgm = &gameplayResumeBgm;
                PlayMusicStream(*bgm);
            }
            SetMusicVolume(*bgm,vol);
            if((int)timeRemaining==10){
                StopMusicStream(countDown);
                PlayMusicStream(countDown);
            }
            if(timeRemaining<=10){
                UpdateMusicStream(countDown);
            }

            timeRemaining -= GetFrameTime();
            timePassed+= GetFrameTime();
            DrawRectangle(0,0, WIDTH, HEIGHT, Fade(BLACK, 0.3f));
            DrawText(Resumehint, (WIDTH - ResumehintWidth) / 2, 0 + HEIGHT - 20, 18, HINTTEXTCOLOR);
            if(removedBalls==ballIndex||timeRemaining<0){ // GameOver
                score+=timeRemaining*100;
                
                if(timeRemaining<=0){
                    strcpy(gameOverReason,"GAMEOVER! TIME UP!");
                    PlaySound(gameover);
                }else PlaySound(winSound);
                GameOver();
            }
            if (IsKeyPressed(KEY_P))
            {
                PlaySound(navigate);
                pageIndex = 1;
            }
            // drawing ball images //Swapno

            for (int i = 0; i < BALLROWS * BALLCOLS; i++)
            {   
                if (existedBalls[i].width > 0)
                    DrawTexturePro(balls[randBallIdx[i]],
                    (Rectangle){0, 0, balls[randBallIdx[i]].width, balls[randBallIdx[i]].height},
                    existedBalls[i],
                    Vector2Zero(), 0, WHITE);
            }

            // shooter
            Vector2 mousePos = GetMousePosition();
            float dx = mousePos.x - cannonBase.x;
            float dy = mousePos.y - cannonBase.y;
            float tempAngle = (atan2f(dy,dx) * RAD2DEG);
            if(tempAngle<=-35 && tempAngle>=-145)
                cannonAngle = tempAngle;
            
            if(blastAnimation<=0)
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
            else{
                DrawTexturePro(
                    blastShooters[shooterIndex],
                    (Rectangle){0, 0, blastShooters[shooterIndex].width, blastShooters[shooterIndex].height},
                    (Rectangle){cannonBase.x, cannonBase.y, CANNON_WIDTH, CANNON_HEIGHT},
                    cannonOrigin,//(Vector2){CANNON_WIDTH / 2.0f, 0},
                    cannonAngle + 90,
                    WHITE
                );
                blastAnimation -= GetFrameTime();
            }
            if (IsKeyPressed(KEY_SPACE)||IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                if(!shooted){
                    PlaySound(blast);
                    blastAnimation = ANIMATION_TIME;
                    shooted=true;
                    bulletPosition.x = cannonBase.x + CANNON_HEIGHT * cos(cannonAngle * DEG2RAD) - BALLRADIUS;
                    bulletPosition.y = cannonBase.y + CANNON_HEIGHT * sin(cannonAngle * DEG2RAD) - BALLRADIUS;

                    bulletVelocity.x = VELOCITY_OF_BULLET*cos(cannonAngle*DEG2RAD);
                    bulletVelocity.y = VELOCITY_OF_BULLET*sin(cannonAngle*DEG2RAD);
                    if(bulletVelocity.y>0) bulletVelocity.y = -bulletVelocity.y;
                }
            }
            bool isCollision = false;
            if(shooted){
                Rectangle shootedBall = (Rectangle){bulletPosition.x, bulletPosition.y, BALLRADIUS * 2, BALLRADIUS * 2};
                bulletPosition.x += bulletVelocity.x;
                bulletPosition.y += bulletVelocity.y;
                if (bulletPosition.x <= 0 || bulletPosition.x >= WIDTH - BALLRADIUS * 2)
                {
                    bulletVelocity.x = -bulletVelocity.x;
                }

                // collision with roof
                if(bulletPosition.y<=0){
                    Vector2 bulletCenter = {bulletPosition.x + BALLRADIUS, bulletPosition.y + BALLRADIUS};
                    int targetIdx = placingIndex(bulletCenter);

                    if (targetIdx != -1)
                    {
                        int r = targetIdx / BALLCOLS;
                        int c = targetIdx % BALLCOLS;
                        float xAdd = (r % 2 == 0) ? BALLRADIUS : 0;

                        existedBalls[targetIdx] = (Rectangle){
                            c * BALLRADIUS * 2 + xAdd,
                            r * (BALLRADIUS * 1.735),
                            BALLRADIUS * 2, BALLRADIUS * 2};
                        randBallIdx[targetIdx] = shooterIndex;
                        ballIndex++;
                        
                        CheckSimpleMatches(targetIdx);
                    }
                    changeColor = true;
                    isCollision = true;
                }

                for (int i = 0; i < BALLROWS * BALLCOLS && !isCollision; i++)
                {
                    if (existedBalls[i].width > 0)
                    {
                        Vector2 shootedCenter = {
                            shootedBall.x + BALLRADIUS,
                            shootedBall.y + BALLRADIUS};

                        Vector2 existedCenter = {
                            existedBalls[i].x + BALLRADIUS,
                            existedBalls[i].y + BALLRADIUS};

                        if (CheckCollisionCircles(
                                shootedCenter,
                                BALLRADIUS - 5,
                                existedCenter,
                                BALLRADIUS))
                        {
                            isCollision = true;

                            if(randBallIdx[i]==3){
                                timeRemaining += 10;
                                PlaySound(bonus);
                                existedBalls[i] = (Rectangle){0, 0, 0, 0};
                                removedBalls++;
                            }else{
                                Vector2 bulletCenter = {
                                bulletPosition.x + BALLRADIUS,
                                bulletPosition.y + BALLRADIUS};

                                int targetIdx = placingIndex(bulletCenter);

                                if (targetIdx != -1)
                                {
                                    int r = targetIdx / BALLCOLS;
                                    int c = targetIdx % BALLCOLS;

                                    float xAdd =
                                        (r % 2 == 0) ? BALLRADIUS : 0;

                                    existedBalls[targetIdx] = (Rectangle){
                                        c * BALLRADIUS * 2 + xAdd,
                                        r * (BALLRADIUS * 1.735),
                                        BALLRADIUS * 2,
                                        BALLRADIUS * 2};

                                    randBallIdx[targetIdx] = shooterIndex;
                                    ballIndex++;
                                    bool isVanished = CheckSimpleMatches(targetIdx);
                                    if (!isVanished && (r * (BALLRADIUS * 1.735f)) >= 375)
                                    {
                                        pageIndex = 4;
                                        PlaySound(gameover);
                                        strcpy(gameOverReason,"GAMEOVER! YOU CROSSED THE LINE!");
                                        GameOver();
                                    }
                                }
                            }
                            changeColor = true;
                        }
                    }
                }
                
                DrawTexturePro(
                    balls[shooterIndex],
                    (Rectangle){0,0,balls[shooterIndex].width,balls[shooterIndex].height},
                    (Rectangle){bulletPosition.x,bulletPosition.y,BALLRADIUS * 2, BALLRADIUS * 2},
                    Vector2Zero(),0,WHITE
                );
                if(changeColor){
                    shooted = false;
                    shooterIndex = shooterIndex2;
                    shooterIndex2 = GetRandomValue(0, BALLNUM - 1);
                    changeColor = false;
                }
                
            }

            // score
            char scores[30];
            sprintf(scores,"SCORE : %d",score);
            char time[20];
            sprintf(time,"TIME: %.0f",timeRemaining);
            DrawText(scores, 10 , HEIGHT-MeasureTextEx(GetFontDefault(),scores,FONTSIZE,FONTSIZE/10).y, FONTSIZE, TEXTCOLOR);

            int nextTextHeight = MeasureTextEx(GetFontDefault(),"NEXT: ",FONTSIZE,FONTSIZE/10).y;
            int timeHeight = MeasureTextEx(GetFontDefault(),time,FONTSIZE,FONTSIZE/10).y+nextTextHeight;
            DrawText(time,WIDTH - MeasureText(time,FONTSIZE)-20, HEIGHT-timeHeight, FONTSIZE, timeRemaining>=10?TEXTCOLOR:RED);
            DrawText("NEXT: ",WIDTH - MeasureText("NEXT: ",FONTSIZE)-70 , HEIGHT-nextTextHeight, FONTSIZE, TEXTCOLOR);
            Rectangle nextColor = {WIDTH - 60,HEIGHT-MeasureTextEx(GetFontDefault(),"NEXT: ",FONTSIZE,FONTSIZE/10).y,55,FONTSIZE};
            switch (shooterIndex2)
            {
            case 0:
                DrawRectangleRec(
                    nextColor,
                    (Color){255, 76, 97, 255}
                );
                break;
            case 1:
                DrawRectangleRec(
                    nextColor,
                    (Color){65, 159, 221, 255}
                );
                break;
            case 2:
                DrawRectangleRec(
                    nextColor,
                    (Color){67, 153, 108, 255}
                );
                break;
            }
            break;
        case 4:// GameOver
            if(bgm != &gameOverBgm){
                StopMusicStream(*bgm);
                bgm = &gameOverBgm;
                PlayMusicStream(*bgm);
            }
            StopMusicStream(countDown);
            SetMusicVolume(*bgm,vol);

            DrawText(gameOverReason, WIDTH/2 - MeasureText(gameOverReason,FONTSIZE)/2, 20 , FONTSIZE, TEXTCOLOR2);
            
            char high[50];
            sprintf(high,"HIGH SCORE: %d",high_scores[0]);
            DrawText(high, WIDTH/2 - MeasureText(high,FONTSIZE)/2, 120 , FONTSIZE, TEXTCOLOR2);
            
            DrawText(scores, WIDTH/2 - MeasureText(scores,FONTSIZE)/2, TEXTPOSY , FONTSIZE, TEXTCOLOR2);
            char timeText[100];
            sprintf(timeText,"TIME: %.0f S",timePassed);
            DrawText(timeText, WIDTH/2 - MeasureText(timeText,FONTSIZE)/2, TEXTPOSY+LINEGAPFORTEXT , FONTSIZE, TEXTCOLOR2);

            if(selected==0){
                DrawText("NEW GAME", 135 - MeasureText("NEW GAME",HOVER_FONTSIZE/1.5)/2, TEXTPOSY+2*LINEGAPFORTEXT , HOVER_FONTSIZE/1.5, TEXTCOLOR);
                
            }else{
                DrawText("NEW GAME", 135 - MeasureText("NEW GAME",FONTSIZE/1.5)/2, TEXTPOSY+2*LINEGAPFORTEXT , FONTSIZE/1.5, TEXTCOLOR);
            }

            if(selected==1){
                DrawText("LEADERBOARD", WIDTH/2 - MeasureText("LEADERBOARD",HOVER_FONTSIZE/1.5)/2, TEXTPOSY+2*LINEGAPFORTEXT , HOVER_FONTSIZE/1.5, TEXTCOLOR);
            }else{
                DrawText("LEADERBOARD", WIDTH/2 - MeasureText("LEADERBOARD",FONTSIZE/1.5)/2, TEXTPOSY+2*LINEGAPFORTEXT , FONTSIZE/1.5, TEXTCOLOR);
            }

            if(selected==2){
                DrawText("EXIT", 650 - MeasureText("EXIT",HOVER_FONTSIZE/1.5)/2, TEXTPOSY+2*LINEGAPFORTEXT , HOVER_FONTSIZE/1.5, TEXTCOLOR);
            }else{
                DrawText("EXIT", 650 - MeasureText("EXIT",FONTSIZE/1.5)/2, TEXTPOSY+2*LINEGAPFORTEXT , FONTSIZE/1.5, TEXTCOLOR);
            }

            if(IsKeyPressed(KEY_LEFT)&&selected>0) selected--;

            if(IsKeyPressed(KEY_RIGHT)&&selected<2) selected++;

            if(IsKeyPressed(KEY_ENTER)){
                if(selected==0){
                    NewGame();
                    selected = 0;
                    pageIndex = 3;
                }else if(selected==1){
                    NewGame();
                    selected=0;
                    pageIndex=5;
                }else exit = true;  
            }
            break;
        case 5:// Leaderboard
            if(bgm != &menuBgm){
                StopMusicStream(*bgm);
                bgm = &menuBgm;
                PlayMusicStream(*bgm);
            }
            SetMusicVolume(*bgm,vol);

            if (IsKeyPressed(KEY_E))
            {
                PlaySound(navigate);
                pageIndex = 0;
            }
            DrawRectangle(10, 40, WIDTH-20, HEIGHT-50, Fade(BLACK, 0.7f));
            DrawText("LEADERBOARD", GetScreenWidth() / 2 - MeasureText("LEADERBOARD", HOVER_FONTSIZE) / 2, 60, HOVER_FONTSIZE, TEXTCOLOR);
            DrawText(backHint, (WIDTH - hintWidth) / 2, 0 + HEIGHT - 35, 18, HINTTEXTCOLOR);

            for(int i=0;i<5;i++){
                char rankText[20],scoreText[20];
                sprintf(rankText,"RANK#%d",i+1);
                sprintf(scoreText,"%d",high_scores[i]);
                DrawText(rankText, 200, 150+i*LINEGAPFORTEXT, FONTSIZE/1.25, TEXTCOLOR);
                DrawText(scoreText, 500, 150+i*LINEGAPFORTEXT, FONTSIZE/1.25, TEXTCOLOR);
            }
            break;
        }
        EndDrawing();
    }

    // Unload all images
    UnloadTexture(bg);
    UnloadTexture(logo);
    for(int i=0;i<BALLNUM;i++){
        UnloadTexture(shooters[i]);
        UnloadTexture(balls[i]);
        UnloadTexture(blastShooters[i]);
    }
    UnloadTexture(balls[BALLNUM]);

    // Unload Sound and BGM's
    UnloadMusicStream(menuBgm);
    UnloadMusicStream(gameplayResumeBgm);
    UnloadMusicStream(gameOverBgm);
    UnloadSound(blast);
    UnloadSound(gameover);
    UnloadSound(navigate);
    UnloadSound(click);
    UnloadSound(bonus);
    UnloadMusicStream(countDown);
    UnloadSound(winSound);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}