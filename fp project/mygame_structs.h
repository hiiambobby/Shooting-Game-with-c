#include <stdbool.h>
#include <stdio.h>

////////////
// structs //
////////////
#define MAX_BULLETS 3
typedef struct
{
    int bulletY;
    int bulletX;
    bool bulletActive;
} Bullet;
typedef struct
{
    int heart;
    unsigned score;
    int X;
    int Y;
    Bullet bullet;

} SpaceShip;
typedef struct
{
    int x, y;
    bool active;
} GUN;
typedef struct
{
    int lives; // 3
    int x, y;
    bool active;
    GUN gun;
} Warrior;

typedef struct
{
    bool active; // 1;
    int power;   // 2;
    int x, y;
} Bomb;

typedef struct
{
    int x, y;
    bool active;
} Boom;
typedef struct
{
    bool active;
    int lives; // 2;
    Boom boom;
    int x, y;
} Bomber;
typedef struct
{
    bool activeH; // 1;
    int x_h, y_H;
} health_aid;

typedef struct
{
    bool active;
    health_aid;
    int x, y;
} Health_Box;

typedef struct
{
    int X;
    int Y;
} Cursor;
// idk why i devided this two i kinda thought it's cooler :_)
typedef struct
{
    char timeString[80];
    int timeSpent;
    unsigned score;
    char UserName[50];
} GameRecord;
typedef struct
{
    bool active;
    int x, y;
    int speed; // when the enemy see the player it would increase it's speed
} Enemy;

// the game stuffs all in a struct
typedef struct
{
    char **buffer;
    SpaceShip spaceship;
    Bomb Bomb;
    Warrior Warrior;
    Bomber Bomber;
    Health_Box Health_Box;
    Cursor cursor;
    GameRecord gameRecord;
    Enemy enemy;
    // static boolionHandled;
} GameState;