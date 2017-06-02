/**
        Created by: Krzysztof Dobrzynski - k.dobrzynski94@gmail.com
                https://github.com/Sejoslaw


		Variable meaning:
			- GLOBAL_VALUE / DEFINE_VALUE
			- MethodName / StructureVariableValue
			- local_variable / variable_which_is_used_locally_by_function

		Goals:
			- a classic Roguelike Game
			- OS independent (written only in C with only standard libraries)
			- single-file project
			- multi-world + infinite world generation
			- single-player
			- multi-player + real-time action
			- items (weapons, armor, potions, scrolls, spells)
			- player inventory
			- save / load
			- enemies + bosses
			- quests

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


/// ======================================================================== Libraries and global defines
#include <stdio.h>
#include <stdlib.h>

/// Add additional OS-specified libraries
#ifdef _WIN32 || __WIN32 /// Windows
    #define OS_WINDOWS
    #include <windows.h>
#elif __linux__ /// Linux
#elif __sun /// Solaris
#elif __FreeBSD__ /// FreeBSD
#elif __NetBSD__ /// NetBSD
#elif __OpenBSD__ /// OpenBSD
#elif __APPLE__ /// Mac OS X
#elif __hpux /// HP-UX
#elif __osf__ /// Tru64 UNIX (formerly DEC OSF1)
#elif __sgi /// Irix
#elif _AIX /// AIX
#endif

/// Debug is used to print additional information and custom stuff
#define DEBUG 1

/// Default World size
#define WORLD_SIZE 20

/// (x, y) position on World
#define WORLD_POS(x, y) WORLD->Tile[x * WORLD_SIZE + y]

/// Commands array length
const unsigned char COMMAND_LENGTH = 2;
/// Commands definition
const char *COMMAND[] =
{
    "Commands keys:",
    "exit / quit - exit the game"
};


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


/// ============================================================= ASCII Characters - which character to print in console
const unsigned char CHAR_EMPTY = 0;
const unsigned char CHAR_WALL = 219;
const unsigned char CHAR_TUNNEL = 35;
const unsigned char CHAR_FLOOR = 46;
const unsigned char CHAR_PLAYER = 64;
/*
const unsigned char CHAR_GOLD = 36;
const unsigned char CHAR_DOORS = 43;
const unsigned char CHAR_POTION = 33;
const unsigned char CHAR_ENEMY = 69;
const unsigned char CHAR_LEVEL_NEXT = 62;
const unsigned char CHAR_LEVEL_BEFORE = 60;
const unsigned char CHAR_MYSTIC_SCROLL = 63;
*/


/// ======================================================================== Types
/// World definition
typedef struct World
{
    unsigned char Size; /// Size of the World
    unsigned char Tile[WORLD_SIZE * WORLD_SIZE]; /// All World tiles / chars
} World;
/// World itself
World* WORLD;

/// Entity / Player definition
typedef struct Entity
{
    int PosX, PosY;
} Entity;
/// Player itself
Entity* PLAYER;


/// ======================================================================== Mallocs - static makers
/// Create new World
World* NewWorld()
{
    World *new_world = malloc(sizeof(World));
    new_world->Size = WORLD_SIZE;

    return new_world;
}

/// Create new Player
Entity* NewPlayer()
{
    Entity *new_player = malloc(sizeof(Entity));
    new_player->PosX = random(1, WORLD_SIZE - 1);
    new_player->PosY = random(1, WORLD_SIZE - 1);

    return new_player;
}


/// ======================================================================== Functions
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
}

/// Array which holds data of all used Tiles - used when Origin is printing
unsigned char tiles_to_print[WORLD_SIZE];

/// Returns 1 if given tile is on tiles_to_print list
unsigned char IsTileOnList(unsigned char tile)
{
    for (int ut = 0; ut < WORLD_SIZE; ++ut) /// Check on list
        if (tiles_to_print[ut] == tile)
            return 1;
    return 0;
}

/// TODO: Remake it for Camera
/// Print origin of the Tiles of World
void PrintWorldTileOrigin()
{
    for (int i = 0; i < WORLD_SIZE; ++i) /// Set all array to 0 - empty space
        tiles_to_print[i] = CHAR_EMPTY;

    /// Add Player to Origin
    tiles_to_print[0] = CHAR_PLAYER;

    /// Search all region
    for (int x = 0; x < WORLD_SIZE; ++x)
        for (int y = 0; y < WORLD_SIZE; ++y)
        {
            unsigned char tile = WORLD_POS(x, y); /// Currently checking tile
            if (!IsTileOnList(tile))
            {
                for (int i = 0; i < WORLD_SIZE; ++i) /// Add new tile char to be print
                {
                    if (tiles_to_print[i] == tile)
                        break;
                    else if (tiles_to_print[i] == CHAR_EMPTY)
                    {
                        tiles_to_print[i] = tile;
                        break;
                    }
                }
            }
        }

    /// Print tiles
    printf("\t Origin: \n");
    for (int i = 0; i < WORLD_SIZE; ++i)
    {
        if (tiles_to_print[i] == CHAR_WALL)
            printf("\t %c - Wall \n", CHAR_WALL);
        else if (tiles_to_print[i] == CHAR_TUNNEL)
            printf("\t %c - Tunnel \n", CHAR_TUNNEL);
        else if (tiles_to_print[i] == CHAR_FLOOR)
            printf("\t %c - Floor \n", CHAR_FLOOR);
        else if (tiles_to_print[i] == CHAR_PLAYER)
            printf("\t %c - Player \n", CHAR_PLAYER);
    }
}

/// TODO: Remake it for Camera
/// Print World and everything to screen
void PrintWorld()
{
    /// Print World from each Tile and add other Tiles and Entities
    for (int x = 0; x < WORLD->Size; ++x)
    {
        for (int y = 0; y < WORLD->Size; ++y)
            if (x == PLAYER->PosX && y == PLAYER->PosY)
                printf("%c", CHAR_PLAYER);
            else
                printf("%c", WORLD_POS(x, y));

        /// Print Movement
        if (x < COMMAND_LENGTH)
            printf("\t %s", COMMAND[x]);
        printf("\n");
    }
    printf("\n");

    /// Print chars meaning
    PrintWorldTileOrigin();
}


/// ======================================================================== Player movement
/// Move Player Down
void PlayerMoveDown()
{
    PLAYER->PosX += 1;
}

/// Move Player Up
void PlayerMoveUp()
{
    PLAYER->PosX -= 1;
}

/// Move Player Left
void PlayerMoveLeft()
{
    PLAYER->PosY -= 1;
}

/// Move Player Right
void PlayerMoveRight()
{
    PLAYER->PosY += 1;
}

/// ======================================================================== Main
/// Main function
int main(int argc, char *argv[])
{
    /// Run additional (pre-run) things based on OS
    #ifdef OS_WINDOWS
        SetConsoleTitle("KD Roguelike Game");
    #endif // OS_WINDOWS

    srand(time(NULL)); /// Start random generator

    WORLD = NewWorld(); /// Set new World
    PLAYER = NewPlayer(); /// Set new Player

    /// User input action
    const action_length = 10;
    char action[action_length];
    char *input;

    GenerateWorld(); /// Generate World

    /// Game loop
    while (1)
    {
        system("cls"); /// Clear screen

        PrintWorld(); /// Print current World stage

        printf("Action > ");
        input = fgets(action, action_length, stdin); /// Read user input

        /// TODO: Fix crashing when Player move
        /// Execute right command
        if (strcmp(input, "exit") == 1 || strcmp(input, "quit") == 1) /// Quit the game
            break;
        else if (strcmp(input, "down") == 1 || (strcmp(input, "d") == 1))
            PlayerMoveDown();
        else if (strcmp(input, "up") == 1 || (strcmp(input, "u") == 1))
            PlayerMoveUp();
        else if (strcmp(input, "left") == 1 || strcmp(input, "l") == 1)
            PlayerMoveLeft();
        else if (strcmp(input, "right") == 1 || strcmp(input, "r") == 1)
            PlayerMoveRight();
        else if (strcmp(input, "debug") == 1)
            printf("DEBUG = %i", DEBUG);
    }

    /// Ending
    return 0;
}
