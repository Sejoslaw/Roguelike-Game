/**
        Created by: Krzysztof Dobrzynski - k.dobrzynski94@gmail.com
                https://github.com/Sejoslaw


        This is a classic Roguelike Game.
        This game should be OS independent.

        If anyone need it:
        char	        1 byte	        -128 to 127 OR 0 to 255
        unsigned char	1 byte	        0 to 255
        signed char	    1 byte	        -128 to 127
        int	            2 or 4 bytes	-32,768 to 32,767 OR -2,147,483,648 to 2,147,483,647
        unsigned int	2 or 4 bytes	0 to 65,535 OR 0 to 4,294,967,295
        short	        2 bytes	        -32,768 to 32,767
        unsigned short	2 bytes	        0 to 65,535
        long	        4 bytes	        -2,147,483,648 to 2,147,483,647
        unsigned long	4 bytes	        0 to 4,294,967,295
        float	        4 byte	        1.2E-38 to 3.4E+38	    6 decimal places
        double	        8 byte	        2.3E-308 to 1.7E+308	15 decimal places
        long double	    10 byte	        3.4E-4932 to 1.1E+4932	19 decimal places
*/


/// ======================================================================== Libraries and defines
#include <stdio.h>
#include <stdlib.h>

/// Default World size
#define WORLD_SIZE 10
/// Set Player position
#define PLAYER_SET_POS(x, y) PLAYER->posX = x; PLAYER->posY = y;
/// (x, y) position on World
#define WORLD_POS(x, y) WORLD->Tile[x * WORLD_SIZE + y]


/// ======================================================================== Utilities
/// Returns a pseudo random number
int random(int min, int max)
{
    int tmp;
    if (max >= min)
        max -= min;
    else
    {
        tmp = min - max;
        min = max;
        max = tmp;
    }
    return max ? (rand() % max + min) : min;
}


/// ====================================================== ASCII Characters - which character to print in console
const unsigned char CHAR_EMPTY = 0;
const unsigned char CHAR_WALL = 219;
const unsigned char CHAR_TUNNEL = 35;
const unsigned char CHAR_FLOOR = 46;
const unsigned char CHAR_GOLD = 36;
const unsigned char CHAR_DOORS = 43;
const unsigned char CHAR_POTION = 33;
const unsigned char CHAR_PLAYER = 64;
const unsigned char CHAR_ENEMY = 69;
const unsigned char CHAR_LEVEL_NEXT = 62;
const unsigned char CHAR_LEVEL_BEFORE = 60;
const unsigned char CHAR_MYSTIC_SCROLL = 63;


/// ================================================================================= Player
/// Player definition
typedef struct Player
{
    int posX, posY;
} Player;
/// Player itself
Player* PLAYER;
/// Create new player
Player* NewPlayer()
{
    Player *new_player = malloc(sizeof(Player));
    new_player->posX = random(1, WORLD_SIZE - 1);
    new_player->posY = random(1, WORLD_SIZE - 1);

    return new_player;
}


/// =================================================================== World
/// World definition
typedef struct World
{
    unsigned char Size; /// Size of the World
    unsigned char Tile[WORLD_SIZE * WORLD_SIZE]; /// All World tiles / chars.
} World;
/// World itself
World* WORLD;
/// Create new world
World* NewWorld()
{
    World *new_world = malloc(sizeof(World));
    new_world->Size = WORLD_SIZE;

    return new_world;
}

/// TODO: Add various World types
/// Generate World
void GenerateWorld()
{
    /// Generate Walls
    for (int x = 0; x < WORLD_SIZE; ++x)
        for (int y = 0; y < WORLD_SIZE; ++y)
            WORLD_POS(x, y) = CHAR_WALL;

    /// Clear everything inside - floor inside
    for (int x = 1; x < WORLD_SIZE - 1; ++x)
        for (int y = 1; y < WORLD_SIZE - 1; ++y)
            WORLD_POS(x, y) = CHAR_FLOOR;

    WORLD_POS(PLAYER->posX, PLAYER->posY) = CHAR_PLAYER;
}

/// Print World to screen
void PrintWorld()
{
    for (int i = 0; i < WORLD->Size; ++i)
    {
        for (int j = 0; j < WORLD->Size; ++j)
            printf("%c", WORLD_POS(i, j));
        printf("\n");
    }
}


/// ===================================================================================== Main
/// Main function
int main(int argc, char *argv[])
{
    /// Start random generator
    srand(time(NULL));
    /// Set new world
    WORLD = NewWorld();
    /// Set new player
    PLAYER = NewPlayer();

    GenerateWorld();
    PrintWorld();

    /// Ending
    return 0;
}
