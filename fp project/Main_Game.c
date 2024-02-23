#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>
#include <time.h>
#include <stdbool.h>
#include "mygame_structs.h"
#include "define.h"

clock_t lastShootTime = 0;
int fireInterval = 100; // Time between shots in milliseconds

//////////////
// functions///
/////////////
void SetWindowSize(int width, int height);
void initializeGame(GameState *gameState);
void initializeGameRecord(GameRecord *gameRecord);
void updateGameRecord(GameState *gameState, int elapsedTime, GameRecord *gameRecord);
void DisplayHeartsAndScore(GameState *gameState);
void SetBufferChar(GameState *gameState, int x, int y, char character);
void saveRecord(GameRecord *record, GameState *score);
void displayRecords();
void DrawSpaceship(GameState *gameState);
void drawBomb(GameState *gameState);
void DrawBomber(GameState *gameState);
void DrawBoom(GameState *gameState);
void DrawWarrior(GameState *gameState);
void DrawHealth(GameState *gameState);
void DrawEnemy(GameState *gameState);
void SpawnEnemy(GameState *gameState);
void updateEnemy(GameState *gameState);
void SpawnHealth(GameState *gameState, GameRecord *gameRecord);
void UpdateHealth(GameState *gameState, GameRecord *gameRecord);
void DrawBulletS(GameState *gameState);
void DrawGun(GameState *gameState);
void updateBullet(GameState *gameState);
void SpawnBullet(GameState *gameState);
void HandleBulletFiring(GameState *gameState);
void spawnBomb(GameState *gameState);
void updateBomb(GameState *gameState, GameRecord *gameRecord);
void spawnboom(GameState *gameState);
void updateboom(GameState *gameState);
void SpawnBomber(GameState *gameState, GameRecord *gameRecord);
bool shouldDropBomb(GameState *gameState);
void UpdateBomber(GameState *gameState);
void spawngun(GameState *gameState);
void updategun(GameState *gameState);
void SpawnWarrior(GameState *gameState, GameRecord *gameRecord);
bool shootwarrior(GameState *gameState);
void UpdateWarrior(GameState *gameState, GameRecord *gameRecord);
void spawnlife(GameState *gameState);
void updatelife(GameState *gameState);
void droplife(GameState *gameState);
void drawlife(GameState *gameState);
bool checkCollisionBullet_Bomb(const Bullet *bul, const Bomb *b);
bool checkCollisionBullet_Bomber(const Bullet *bul, const Bomber *b);
bool checkCollisionBullet_Warrior(const Bullet *bul, const Warrior *b);
bool checkCollisionBullet_Healt(const Bullet *bul, const Health_Box *b);
bool checkCollisionBomb(const SpaceShip *spaceship, const Bomb *b);
bool checkCollisionGun(const SpaceShip *spaceship, const GUN *b);
bool checkCollisionBomber(const SpaceShip *spaceship, const Bomber *b);
bool checkCollisionWarrior(const SpaceShip *spaceship, const Warrior *w);
bool checkCollisionBoom(const SpaceShip *spaceship, const Boom *b);
bool checkCollisio_extralife(const SpaceShip *spaceship, const Health_Box *w);
void handleCollision(GameState *gameState);
void MovePlayer(GameState *gameState);
void RenderBackground(GameState *gameState);
void RenderGame(GameState *gameState, GameRecord *gameRecord);
void HideCursor();
void PresentScreen(GameState *gameState);
void ClearScreen();
void FreeGame(GameState *gameState);
void gotoXY(int x, int y);
void DisplayMenu(int selection);

void SetWindowSize(int width, int height)
{
    COORD coord = {width, height};
    SMALL_RECT rect = {0, 0, width - 1, height - 1};

    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleScreenBufferSize(handle, coord);
    SetConsoleWindowInfo(handle, TRUE, &rect);
}

// this is where everythin is defined
// we start by creating a buffer in the size of the screen
void initializeGame(GameState *gameState)
{
    gameState->buffer = (char **)malloc(SCREEN_HEIGHT * sizeof(char *));
    for (int i = 0; i < SCREEN_HEIGHT; i++)
    {
        gameState->buffer[i] = (char *)malloc(SCREEN_WIDTH * sizeof(char));
        for (int j = 0; j < SCREEN_WIDTH; j++)
        {
            gameState->buffer[i][j] = EMPTY_SPACE;
        }
    }
    /// we could have define all of this on top instead of doing this but anyways....
    gameState->spaceship.X = (SCREEN_WIDTH - 12) / 2;
    gameState->spaceship.Y = SCREEN_HEIGHT - 6;
    /// characters lives///////
    //////////////////////////
    gameState->spaceship.heart = player_heart;
    gameState->Warrior.lives = Warrior_lives;
    gameState->Bomber.lives = Bomber_lives;
    gameState->Warrior.lives = Warrior_lives;
    gameState->spaceship.score = 0;
    gameState->cursor.X = gameState->spaceship.X + 6; // Set cursor initially to the center of spaceship
    gameState->cursor.Y = gameState->spaceship.Y + 2; // Set cursor initially to the center of spacesh
}
////////////////////////////////
/// To record the game data//////
///////////////////////////////
void initializeGameRecord(GameRecord *gameRecord)
{
    // Initialize the time string to an empty string
    memset(gameRecord->timeString, 0, sizeof(gameRecord->timeString));
    // Initialize the time spent to 0
    gameRecord->timeSpent = 0;
}
void updateGameRecord(GameState *gameState, int elapsedTime, GameRecord *gameRecord)
{
    // Update the time spent
    gameRecord->timeSpent += elapsedTime;
    // Update the score
    gameRecord->score = gameState->spaceship.score;
}
////////////////////////
/////heart and score////
void DisplayHeartsAndScore(GameState *gameState)
{
    // Display player's hearts
    for (int i = 0; i < gameState->spaceship.heart; i++)
    {
        SetBufferChar(gameState, i, 0, '+'); // Position the hearts at the top left corner
    }

    // Display the score (time passed)
    char scoreStr[50];
    sprintf(scoreStr, "SCORE: %u", gameState->spaceship.score);
    for (int i = 0; i < strlen(scoreStr); i++)
    {
        SetBufferChar(gameState, i, 1, scoreStr[i]); // Position the score below the hearts
    }
}

// Function to set a character in the buffer at a specific position
void SetBufferChar(GameState *gameState, int x, int y, char character)
{
    if (x >= 0 && x < SCREEN_WIDTH && y >= 0 && y < SCREEN_HEIGHT)
    {
        gameState->buffer[y][x] = character;
    }
}
///////////////////////
// to save the records//
//////////////////////
void saveRecord(GameRecord *record, GameState *score)
{

    FILE *file = fopen("Records.txt", "a"); // Open the file in append mode
    if (file != NULL)
    {
        time_t rawtime;
        struct tm *info;
        time(&rawtime);
        info = localtime(&rawtime);
        // Format the current time
        strftime(record->timeString, sizeof(record->timeString), "%Y-%m-%d %H:%M:%S", info);

        // Save record to the file
        fprintf(file, "UserName: %s | Time: %s | Score: %u | Time Spent: %d seconds\n", record->UserName, record->timeString, score->spaceship.score, record->timeSpent);

        fclose(file); // Close the file
    }
    else
    {
        printf("Something is wrong contact the develop...");
    }
}

void displayRecords()
{
    FILE *file = fopen("Records.txt", "r"); // Open the file in read mode

    if (file != NULL)
    {
        int c;
        while ((c = fgetc(file)) != EOF)
        {
            putchar(c); // Print character by character
        }
        printf("This is your records of the game");
        getchar();

        fclose(file); // Close the file
    }

    else
    {
        printf("Error opening file for records.\n");
    }
}
//------------------------ //
//---Drawing Characters---//
//-----------------------//
void DrawSpaceship(GameState *gameState)
{
    // ASCII art for the spaceship
    const char *spaceshipArt[] = {
        "      |",
        "     /*\\",
        "   /*/*\\*\\",
        " /*/*/*\\*\\*\\"};

    // Draw the spaceship position in the buffer
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 12; j++)
        { // Adjusted width to fit the ASCII art
            SetBufferChar(gameState, gameState->spaceship.X + j, gameState->spaceship.Y + i, spaceshipArt[i][j]);
        }
    }
}
void drawBomb(GameState *gameState)
{
    const char *bombArt[] = {"(*)"};

    // Draw the bomb position in the buffer
    if (gameState->Bomb.active)
        for (int i = 0; i < 1; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                SetBufferChar(gameState, gameState->Bomb.x + j, gameState->Bomb.y + i, bombArt[i][j]);
            }
        }
}

void DrawBomber(GameState *gameState)
{
    const char *bomber[] = {
        "       .---.",
        "     _/__~0_\\_",
        " = =(__________)"};

    if (gameState->Bomber.active)
        for (int i = 0; i < Bomber_h; i++)
        {
            for (int j = 0; j < Bomber_w; j++)
            {
                SetBufferChar(gameState, gameState->Bomber.x + j, gameState->Bomber.y + i, bomber[i][j]);
            }
        }
}
void DrawBoom(GameState *gameState)
{
    if (gameState->Bomber.boom.active)
    {
        SetBufferChar(gameState, gameState->Bomber.boom.x, gameState->Bomber.boom.y, 'O');
    }
}
void DrawWarrior(GameState *gameState)
{
    const char *WarriorArt[] = {
        "       (--------v--------)",
        "__        __ __|__",
        "(@)______(::(    |@\\_",
        "`------\\_\\___________)",
        "         __|___|__,"};
    if (gameState->Warrior.active)
        for (int i = 0; i < Warrior_h; i++)
        {
            for (int j = 0; j < Warrior_w; j++)
            {
                SetBufferChar(gameState, gameState->Warrior.x + j, gameState->Warrior.y + i, WarriorArt[i][j]);
            }
        }
}
void DrawHealth(GameState *gameState)
{

    const char *Health[] = {
        "       .---.",
        "     _/_+++ \\_",
        "    (__________)"};

    if (gameState->Health_Box.active)
        for (int i = 0; i < health_h; i++)
        {
            for (int j = 0; j < health_w; j++)
            {
                SetBufferChar(gameState, gameState->Health_Box.x + j, gameState->Health_Box.y + i, Health[i][j]);
            }
        }
}

////////////////////////
/// the follower enemy///
/// which is not still right//
void DrawEnemy(GameState *gameState)
{
    // ASCII art for the warrior spaceship
    const char *EnemyArt[] = {
        "    _!_",
        " ----o----",
        "     --"};

    // Draw the warrior spaceship position in the buffer
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 11; j++)
        {
            SetBufferChar(gameState, gameState->enemy.x + j, gameState->enemy.y + i, EnemyArt[i][j]);
        }
    }
}
void SpawnEnemy(GameState *gameState)
{
    if (gameState->gameRecord.timeSpent > Enemy_Time)
    {
        gameState->enemy.x = rand() % SCREEN_WIDTH;
        gameState->enemy.y = rand() % SCREEN_HEIGHT / 3;
        gameState->enemy.active = 1;
    }
}

void updateEnemy(GameState *gameState)
{
    if (gameState->enemy.active)
    {
        if (gameState->enemy.x == gameState->spaceship.X)
        {
            if (gameState->enemy.y < gameState->spaceship.Y)
                gameState->enemy.y++;
            else
                gameState->enemy.y--;
        }
        if (gameState->enemy.y == gameState->spaceship.Y)
        {
            if (gameState->enemy.x < gameState->spaceship.X)
                gameState->enemy.x++;
            else
                gameState->enemy.x--;
        }
        // if (gameState->enemy.x > SCREEN_WIDTH || gameState->enemy.x > SCREEN_HEIGHT || gameState->enemy.x == 0 || gameState->enemy.y == 0) // deactive
        //   gameState->enemy.active = false;
    }
}
///////////////
/// HEALTH/////
void SpawnHealth(GameState *gameState, GameRecord *gameRecord)
{
    if (gameRecord->timeSpent == Health_Time || gameRecord->timeSpent == Health_Time * 2 || gameRecord->timeSpent == Health_Time * 4 || gameRecord->timeSpent == Health_Time * 7)
    {
        gameState->Health_Box.x = rand() % SCREEN_WIDTH;
        gameState->Health_Box.y = 0;
        gameState->Health_Box.active = 1;
    }
}
void UpdateHealth(GameState *gameState, GameRecord *gameRecord)
{

    if (gameState->Health_Box.active)
    {
        gameState->Health_Box.y += Health_Speed; // Move down

        if (gameState->Health_Box.y >= SCREEN_HEIGHT)
        {
            gameState->Health_Box.active = false; // Deactivate the warrior
            SpawnHealth(gameState, gameRecord);
        }
    }
    else
        SpawnHealth(gameState, gameRecord);
}

////////////for space ship bullet////////////////////////
void DrawBulletS(GameState *gameState)
{
    if (gameState->spaceship.bullet.bulletActive)
    {
        SetBufferChar(gameState, gameState->spaceship.bullet.bulletX, gameState->spaceship.bullet.bulletY, '|');
    }
}
void DrawGun(GameState *gameState)
{
    if (gameState->Warrior.gun.active)
    {
        SetBufferChar(gameState, gameState->Warrior.gun.x, gameState->Warrior.gun.y, '*');
    }
}
//////////
// bullet//
void updateBullet(GameState *gameState)
{
    if (gameState->spaceship.bullet.bulletActive)
    {
        gameState->spaceship.bullet.bulletY -= Bullet_Speed; // Move the bullet upward
        if (gameState->spaceship.bullet.bulletY < 0)
        {
            gameState->spaceship.bullet.bulletActive = 0;
        }
    }
}
void SpawnBullet(GameState *gameState)
{
    // Initialize the bullet properties
    gameState->spaceship.bullet.bulletX = gameState->spaceship.X + 5; // Starting X position
    gameState->spaceship.bullet.bulletY = gameState->spaceship.Y - 1; // Starting Y position
    gameState->spaceship.bullet.bulletActive = 1;                     // Set the bullet to active
}

void HandleBulletFiring(GameState *gameState)
{
    clock_t currentTime = clock();
    double elapsedTime = ((double)(currentTime - lastShootTime)) / CLOCKS_PER_SEC * 1000;

    if (elapsedTime >= fireInterval || lastShootTime == 0)
    {
        if (!gameState->spaceship.bullet.bulletActive)

            SpawnBullet(gameState);
        lastShootTime = currentTime;
    }
}
//////////////////////
////Bomb movements///
void spawnBomb(GameState *gameState)
{

    gameState->Bomb.x = rand() % SCREEN_WIDTH;
    gameState->Bomb.y = 0;
    gameState->Bomb.active = 1;
}

void updateBomb(GameState *gameState, GameRecord *gameRecord)
{
    if (gameState->Bomb.active)
    {
        gameState->Bomb.y++; // the speed of the bomb is one block per secod
        if (gameState->Bomb.y >= SCREEN_HEIGHT)
        {

            gameState->Bomb.active = 0;
            spawnBomb(gameState);
        }
    }
    else // if the space ship hits the bomb it will respawn but after some seconds
        if (gameRecord->timeSpent >= Bomb_time)
            spawnBomb(gameState);
}

////////////////////
/// boom  movements//
void spawnboom(GameState *gameState)
{
    if (gameState->Bomber.active)
    {
        gameState->Bomber.boom.x = gameState->Bomber.x; // the same x pos as the top of the bomber
        gameState->Bomber.boom.y = gameState->Bomber.y + Bomber_h;
        gameState->Bomber.boom.active = true;
    }
}
void updateboom(GameState *gameState)
{
    // Check if the character is active
    if (gameState->Bomber.active)
    {
        // Move the bomb downward
        gameState->Bomber.boom.y++;

        // If the bomb has reached the bottom of the screen, deactivate it
        if (gameState->Bomber.boom.y >= SCREEN_HEIGHT)
        {
            gameState->Bomber.boom.active = false;
        }
    }
    else
    {
        // If the character is not active, deactivate the bomb
        gameState->Bomber.boom.active = false;
    }
}

///////////////////////
/// Bomber Movements////
void SpawnBomber(GameState *gameState, GameRecord *gameRecord)
{
    // if (gameRecord->timeSpent == Bomber_Time || gameRecord->timeSpent == Bomber_Time + 10 || gameRecord->timeSpent == Bomber_Time + 20 || gameRecord->timeSpent == Bomber_Time + 30 || gameRecord->timeSpent == Bomber_Time + 40)

    gameState->Bomber.y = rand() % SCREEN_HEIGHT / 3;
    gameState->Bomber.x = -2;
    gameState->Bomber.active = true;
    gameState->Bomber.lives = Bomber_lives;
}
bool shouldDropBomb(GameState *gameState)
{
    // if (gameState->spaceship.X - gameState->Bomber.x == 3 || gameState->spaceship.X - gameState->Bomber.x == -3)
    if (gameState->spaceship.Y == gameState->Bomber.y)
        return true;
    return false;
}
void UpdateBomber(GameState *gameState)
{
    // we want the user to think that the screen is going up
    if (gameState->Bomber.active)
    {
        gameState->Bomber.x += Bomber_Speed;
        if (gameState->Bomber.x % BOMBER_MOVE_THRESHOLD == 0)
        {
            gameState->Bomber.y += 1;
        }

        if (gameState->Bomber.x >= SCREEN_WIDTH)
        {
            gameState->Bomber.active = 0;
            SpawnBomber(gameState, &gameState->gameRecord);
        }
        if (shouldDropBomb(gameState))
        {
            spawnboom(gameState);
        }
    }
    else
        SpawnBomber(gameState, &gameState->gameRecord);
}
//////////////////////////////
/////warrior Gun/////////////

void spawngun(GameState *gameState)
{
    if (gameState->Warrior.active)
    {
        gameState->Warrior.gun.x = gameState->Warrior.x; // the same x pos as the top of the bomber
        gameState->Warrior.gun.y = gameState->Warrior.y; //
        gameState->Warrior.gun.active = true;
    }
}
void updategun(GameState *gameState)
{

    if (gameState->Warrior.active)
    {
        gameState->Warrior.gun.y -= Warrior_Gun_Speed;
        gameState->Warrior.gun.x -= Warrior_Gun_Speed;
        if (gameState->Warrior.gun.y == 0 || gameState->Warrior.gun.x == 0)
        {
            gameState->Warrior.gun.active = 0;
        }
    }
}

/////////////////////////
////////Warrior//////////
void SpawnWarrior(GameState *gameState, GameRecord *gameRecord)
{
    if (gameRecord->timeSpent == Warrior_time || gameRecord->timeSpent == Warrior_time * 2 || gameRecord->timeSpent == Warrior_time * 4 || gameRecord->timeSpent == Warrior_time * 5 || gameState->spaceship.score >= 300)

    {
        gameState->Warrior.active = true;
        gameState->Warrior.x = rand() % SCREEN_WIDTH; // Respawn from a random x position
        gameState->Warrior.y = 0;                     // At the top of the screen
        gameState->Warrior.lives = Warrior_lives;
    }
}
bool shootwarrior(GameState *gameState)
{

    return true;
}
void UpdateWarrior(GameState *gameState, GameRecord *gameRecord)
{

    if (gameState->Warrior.active) //&//& gameRecord->timeSpent >= Warrior_time)
    {
        gameState->Warrior.y += Warrior_Speed; // Move down

        if (gameState->Warrior.y >= SCREEN_HEIGHT)
        {
            gameState->Warrior.active = false; // Deactivate the warrior
            SpawnWarrior(gameState, gameRecord);
        }
        if (shootwarrior(gameState))
        {
            spawngun(gameState);
            updategun(gameState);
        }
    }
    else
        SpawnWarrior(gameState, gameRecord);
}
///////////////////////////
void spawnlife(GameState *gameState)
{
    gameState->Health_Box.activeH = true;
    gameState->Health_Box.x_h = gameState->Health_Box.x;
    gameState->Health_Box.y_H = gameState->Health_Box.y + health_h;
}
void updatelife(GameState *gameState)
{
    gameState->Health_Box.y_H++;
    if (gameState->Health_Box.y_H++ >= SCREEN_HEIGHT)
        gameState->Health_Box.activeH = false;
}

void droplife(GameState *gameState)
{
    spawnlife(gameState);
    updatelife(gameState);
}
void drawlife(GameState *gameState)
{
    const char *LifeArt[] =
        {
            "   |  ",
            " --o--",
            "   |  "};
    if (gameState->Health_Box.active)
        for (int i = 0; i < health_aid_h; i++)
        {
            for (int j = 0; j < health_aid_w; j++)
            {
                SetBufferChar(gameState, gameState->Health_Box.x_h + j, gameState->Health_Box.y_H + i, LifeArt[i][j]);
            }
        }
}
//////collision//////////////////////////
bool checkCollisionBullet_Bomb(const Bullet *bul, const Bomb *b)
{

    if (bul->bulletX >= b->x && bul->bulletX <= b->x + Bomb_w &&
        bul->bulletY >= b->y && bul->bulletY <= b->y + Bomb_h)
    {

        return true;
    }
    return false;
}
bool checkCollisionBullet_Bomber(const Bullet *bul, const Bomber *b)
{
    if (bul->bulletX >= b->x && bul->bulletX <= b->x + Bomber_w &&
        bul->bulletY >= b->y && bul->bulletY <= b->y + Bomber_h)
    {
        return true;
    }
    return false;
}
bool checkCollisionBullet_Warrior(const Bullet *bul, const Warrior *b)
{
    if (bul->bulletX >= b->x && bul->bulletX <= b->x + Warrior_w &&
        bul->bulletY >= b->y && bul->bulletY <= b->y + Warrior_h)
    {
        return true;
    }
    return false;
}
bool checkCollisionBullet_Healt(const Bullet *bul, const Health_Box *b)
{
    if (bul->bulletX >= b->x && bul->bulletX <= b->x + health_w &&
        bul->bulletY >= b->y && bul->bulletY <= b->y + health_h)
    {
        return true;
    }
    return false;
}

bool checkCollisionBomb(const SpaceShip *spaceship, const Bomb *b)
{

    if (b->x >= spaceship->X && b->x <= spaceship->X + 12 &&
        b->y >= spaceship->Y && b->y <= spaceship->Y + 5)
    {
        return true;
    }
    return false;
}
bool checkCollisionGun(const SpaceShip *spaceship, const GUN *b)
{

    if (b->x >= spaceship->X && b->x <= spaceship->X + ss_w &&
        b->y >= spaceship->Y && b->y <= spaceship->Y + ss_h)
    {
        return true;
    }
    return false;
}
bool checkCollisionBomber(const SpaceShip *spaceship, const Bomber *b)
{

    // Check if the bomber's right edge is to the right of the spaceship's left edge
    if (b->x < spaceship->X + ss_w)
    {
        // Check if the bomber's left edge is to the left of the spaceship's right edge
        if (b->x + Bomber_w > spaceship->X)
        {
            // Check if the bomber's bottom edge is below the spaceship's top edge
            if (b->y < spaceship->Y + ss_h)
            {
                // Check if the bomber's top edge is above the spaceship's bottom edge
                if (b->y + Bomber_h > spaceship->Y)
                {
                    // Collision detected
                    return true;
                }
            }
        }
    }
    // No collision detected
    return false;
}
bool checkCollisionWarrior(const SpaceShip *spaceship, const Warrior *w)
{

    // Check if the bomber's right edge is to the right of the spaceship's left edge
    if (w->x < spaceship->X + ss_w)
    {
        // Check if the bomber's left edge is to the left of the spaceship's right edge
        if (w->x + Warrior_w > spaceship->X)
        {
            // Check if the bomber's bottom edge is below the spaceship's top edge
            if (w->y < spaceship->Y + ss_h)
            {
                // Check if the bomber's top edge is above the spaceship's bottom edge
                if (w->y + Warrior_h > spaceship->Y)
                {
                    // Collision detected
                    return true;
                }
            }
        }
    }
    // No collision detected
    return false;
}
bool checkCollisionBoom(const SpaceShip *spaceship, const Boom *b)
{
    if (b->x >= spaceship->X && b->x <= spaceship->X + 12 &&
        b->y >= spaceship->Y && b->y <= spaceship->Y + 5)
    {
        return true;
    }
    return false;
}
bool checkCollisio_extralife(const SpaceShip *spaceship, const Health_Box *w)
{

    // Check if the bomber's right edge is to the right of the spaceship's left edge
    if (w->x_h < spaceship->X + ss_w)
    {
        // Check if the bomber's left edge is to the left of the spaceship's right edge
        if (w->x_h + health_aid_w > spaceship->X)
        {
            // Check if the bomber's bottom edge is below the spaceship's top edge
            if (w->y_H < spaceship->Y + ss_h)
            {
                // Check if the bomber's top edge is above the spaceship's bottom edge
                if (w->y_H + health_aid_h > spaceship->Y)
                {
                    // Collision detected
                    return true;
                }
            }
        }
    }
    // No collision detected
    return false;
}
void handleCollision(GameState *gameState)
{
    // Check for collision between the spaceship and the bomb
    if (checkCollisionBomb(&gameState->spaceship, &gameState->Bomb) && gameState->Bomb.active)
    {
        gameState->Bomb.active = false;
        gameState->spaceship.heart -= Bomb_Power;
        return;
    }
    if (checkCollisionBomber(&gameState->spaceship, &gameState->Bomber) && gameState->Bomber.active)
    {
        gameState->Bomber.lives = 0;
        gameState->spaceship.heart -= Bomber_Damage;
        gameState->Bomber.active = false;
        return;
    }
    if (checkCollisionBoom(&gameState->spaceship, &gameState->Bomber.boom) && gameState->Bomber.boom.active)
    {
        gameState->Bomber.boom.active = false;
        gameState->spaceship.heart -= Boom_Damage;
        return;
    }
    if (checkCollisionWarrior(&gameState->spaceship, &gameState->Warrior) && gameState->Warrior.active)
    {
        gameState->Warrior.lives = 0;
        gameState->spaceship.heart -= Warrior_Damage;
        gameState->Warrior.active = false;
        return;
    }
    if (checkCollisionGun(&gameState->spaceship, &gameState->Warrior.gun) && gameState->Warrior.gun.active)
    {
        gameState->spaceship.heart -= Warrior_Damage;
        gameState->Warrior.gun.active = false;
        return;
    }
    if (checkCollisio_extralife(&gameState->spaceship, &gameState->Health_Box) && gameState->Health_Box.activeH)
    {
        gameState->spaceship.score += heart_score;
        gameState->Health_Box.activeH = false;
        return;
    }
    if (gameState->Bomb.active && gameState->spaceship.bullet.bulletActive)
        if (checkCollisionBullet_Bomb(&gameState->spaceship.bullet, &gameState->Bomb))
        {
            gameState->Bomb.active = false;
            gameState->spaceship.bullet.bulletActive = false; // and dont print the bullet after that
            gameState->spaceship.score += Bomb_point;
            return;
        }
    if (gameState->Bomber.active && gameState->spaceship.bullet.bulletActive)
    {
        if (checkCollisionBullet_Bomber(&gameState->spaceship.bullet, &gameState->Bomber))
        {
            gameState->Bomber.lives--;
            gameState->spaceship.score += Bomber_point;

            // Check if the bomber's lives have reached zero
            if (gameState->Bomber.lives <= 0)
            {
                gameState->Bomber.active = false; // Deactivate the bomber
            }
            gameState->spaceship.bullet.bulletActive = false; // and dont print the bullet after that
            return;
        }
    }
    if (gameState->Warrior.active && gameState->spaceship.bullet.bulletActive)
    {
        if (checkCollisionBullet_Warrior(&gameState->spaceship.bullet, &gameState->Warrior))
        {
            gameState->Warrior.lives--;
            gameState->spaceship.score += Warrior_Point;

            // Check if the bomber's lives have reached zero
            if (gameState->Warrior.lives <= 0)
            {
                gameState->Warrior.active = false; // Deactivate the bomber
            }
            gameState->spaceship.bullet.bulletActive = false; // and dont print the bullet after that
            return;
        }
    }
    if (gameState->Health_Box.active && gameState->spaceship.bullet.bulletActive)
    {
        if (checkCollisionBullet_Healt(&gameState->spaceship.bullet, &gameState->Health_Box))
        {
            gameState->Health_Box.active = false;             // Deactivate the bomber
            gameState->spaceship.bullet.bulletActive = false; // and dont print the bullet after that
            droplife(gameState);
            return;
        }
    }
}
///////////////////////////////////////////////
// Function to move the player (spaceship)
void MovePlayer(GameState *gameState)
{

    // Move the spaceship based on user input
    if (_kbhit())
    {
        char ch = _getch();
        int oldCursorX = gameState->cursor.X;
        int oldCursorY = gameState->cursor.Y;

        switch (ch)
        {
        case 'H':
        case 'w': // Up arrow key
            if (gameState->spaceship.Y > 0)
            {
                gameState->spaceship.Y--;
            }
            break;
        case 'P':
        case 's': // Down arrow key
            if (gameState->spaceship.Y < SCREEN_HEIGHT - 5)
            {
                gameState->spaceship.Y++;
            }
            break;
        case 'K':
        case 'a': // Left arrow key
            if (gameState->spaceship.X > 0)
            {
                gameState->spaceship.X--;
            }
            break;
        case 'M':
        case 'd': // Right arrow key
            if (gameState->spaceship.X < SCREEN_WIDTH - 12)
            {
                gameState->spaceship.X++;
            }
            break;
        case ' ': // Space key for shooting
            HandleBulletFiring(gameState);
            break;
        case 27: // ESC key
            exit(0);
        }

        // Update cursor position based on spaceship movement
        gameState->cursor.X = gameState->spaceship.X + 6;
        gameState->cursor.Y = gameState->spaceship.Y + 2;

        // Clear the old cursor position in the buffer
        SetBufferChar(gameState, oldCursorX, oldCursorY, EMPTY_SPACE);
    }
}

// Function to render the background (clears the buffer)
void RenderBackground(GameState *gameState)
{
    for (int i = 0; i < SCREEN_HEIGHT; i++)
    {
        for (int j = 0; j < SCREEN_WIDTH; j++)
        {
            gameState->buffer[i][j] = EMPTY_SPACE;
        }
    }
}

// Function to render the game state
void RenderGame(GameState *gameState, GameRecord *gameRecord)
{
    drawBomb(gameState);
    DrawSpaceship(gameState);
    DrawBulletS(gameState);
    DisplayHeartsAndScore(gameState);
    DrawBomber(gameState);
    DrawBoom(gameState);
    DrawWarrior(gameState);
    DrawGun(gameState);
    // DrawEnemy(gameState);
    DrawHealth(gameState);
    drawlife(gameState);

    // Add functions for rendering enemies, score, lives, etc. as needed
}
// Function to hide the console cursor
void HideCursor()
{
    CONSOLE_CURSOR_INFO info;
    GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);
    info.bVisible = FALSE;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);
}

void PresentScreen(GameState *gameState)
{
    // Hide the cursor while updating the screen
    HideCursor();

    gotoXY(0, 0);

    // Print the buffer to the console
    for (int i = 0; i < SCREEN_HEIGHT; i++)
    {
        for (int j = 0; j < SCREEN_WIDTH; j++)
        {
            putchar(gameState->buffer[i][j]);
        }
        putchar('\n');
    }
}

// Function to clear the console screen
void ClearScreen()
{
    system("cls");
}

// Function to free the memory allocated for the game state
void FreeGame(GameState *gameState)
{
    for (int i = 0; i < SCREEN_HEIGHT; i++)
    {
        free(gameState->buffer[i]);
    }
    free(gameState->buffer);
}
////////MENU DISPLAY/////////////////
void gotoXY(int x, int y)
{
    COORD coord = {x, y};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void DisplayMenu(int selection)
{
    int x = SCREEN_WIDTH / 2;
    int y = SCREEN_HEIGHT / 2;

    // Set the console text color to white for the title
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    gotoXY(x - 10, y - 4);
    printf("=======================\n");
    gotoXY(x - 10, y - 3);
    printf("|| WELCOME TO MY GAME! ||\n");
    gotoXY(x - 10, y - 2);
    printf("=======================\n");

    // Set the console text color to yellow for the menu options
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN);
    gotoXY(x - 4, y - 1);
    printf("   Play\n");
    gotoXY(x - 4, y);
    printf("   Records\n");
    gotoXY(x - 4, y + 1);
    printf("   Exit\n");

    // Set the console text color to green for the selected option
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN);
    switch (selection)
    {
    case 0:
        gotoXY(x - 4, y - 1);
        printf("-> Play\n");
        break;
    case 1:
        gotoXY(x - 4, y);
        printf("-> Records\n");
        break;
    case 2:
        gotoXY(x - 4, y + 1);
        printf("-> Exit\n");
        break;
    }

    // Reset the console text color to white for subsequent output
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}

int displayMenu()
{
    int selection = 0;

    while (1)
    {
        DisplayMenu(selection);

        if (GetAsyncKeyState(VK_DOWN) & 0x8000)
        {
            selection++;
            if (selection > 2)
                selection = 0;

            // Wait until the key is released
            while (GetAsyncKeyState(VK_DOWN) & 0x8000)
            {
                // Do nothing
            }
        }
        else if (GetAsyncKeyState(VK_UP) & 0x8000)
        {
            selection--;
            if (selection < 0)
                selection = 2;

            // Wait until the key is released
            while (GetAsyncKeyState(VK_UP) & 0x8000)
            {
                // Do nothing
            }
        }
        else if (GetAsyncKeyState(VK_RETURN) & 0x8000)
        {
            return selection;
        }
        Sleep(50);
    }
}
bool gameover(GameState *boo)
{
    if (boo->spaceship.heart <= 0)
        return true;
    return false;
}
void gameOver(GameState *gameState, GameRecord *gameRecord)
{
    if (gameState->spaceship.heart <= 0)
    {
        int x = SCREEN_WIDTH / 2;
        int y = SCREEN_HEIGHT / 2;
        ClearScreen();
        gotoXY(x - 5, y - 2);
        printf("GAME OVER");
        gotoXY(x - 10, y - 1);
        printf("|Your Score Was: %d|", gameState->spaceship.score);
        gotoXY(x - 14, y);
        printf("Please enter you name(name_familyname) ");
        gotoXY(x - 5, y + 1);
        scanf("%s", gameRecord->UserName);
        saveRecord(gameRecord, gameState);
        getchar(); // Wait for the user to press Enter
        exit(0);
    }
}
// int displayGameOverOptions()
// {
//     int c;
//     ClearScreen();
//     printf("What is your choice?\n");
//     printf("1.play again\n");
//     printf("2.Main Menu\n");
//     scanf("%d", c);
//     return c;
// }

// All the update functions
void UpdateGame(GameState *gameState, GameRecord *gameRecord)
{
    MovePlayer(gameState);
    updateBullet(gameState);
    updateBomb(gameState, gameRecord);
    UpdateBomber(gameState);
    UpdateWarrior(gameState, gameRecord);
    updateboom(gameState);
    UpdateHealth(gameState, gameRecord);
    updategun(gameState);
    handleCollision(gameState);
}

int main()
{
    srand(time(NULL));
    SetWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    HideCursor();
    GameState *gameState = (GameState *)malloc(sizeof(GameState));
    GameRecord *gameRecord = (GameRecord *)malloc(sizeof(GameRecord));
    initializeGame(gameState);
    initializeGameRecord(gameRecord);

    int menuOption = displayMenu();
    // Handle the user's selection
    switch (menuOption)
    {
    case 0:
        ClearScreen();
        clock_t start_time = clock(); // the start time
        int point = 0;
        /// THE HOLY GAME LOOP
        while (1)
        {
            clock_t current_time = clock();
            double elapsed_time = ((double)(current_time - start_time)) / CLOCKS_PER_SEC;

            if (elapsed_time >= 1.0)
            {
                start_time = current_time; // Reset the start time
            }
            UpdateGame(gameState, gameRecord);
            updateGameRecord(gameState, elapsed_time, gameRecord);
            RenderBackground(gameState);
            RenderGame(gameState, gameRecord);
            PresentScreen(gameState);
            point++;
            if (point % 50 == 0)
                gameState->spaceship.score++;
            if (gameover(gameState))
                break;
        }

        gameOver(gameState, gameRecord);
    case 1:
        ClearScreen();
        displayRecords();
        printf("\nPress any key to exit...\n");
        getchar(); // Wait for the user to press Enter
        break;

    case 2:
        ClearScreen();
        printf("Created by Saba Seyed tabaei");
        Sleep(1000);
        break;
    }

    FreeGame(gameState);
    free(gameState);

    return 0;
}