/**
        Created by: Krzysztof Dobrzynski - k.dobrzynski94@gmail.com
                https://github.com/Sejoslaw


		Variable meaning:
			- GLOBAL_VALUE / DEFINE_VALUE / DEFINE_METHOD
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
#include <ctype.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>

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

/// Colors
#define COLOR_RED "\x1B[31m"
#define COLOR_GREEN "\x1B[32m"
#define COLOR_YELLOW "\x1B[33m"
#define COLOR_BLUE "\x1B[34m"
#define COLOR_MAGENTA "\x1B[35m"
#define COLOR_CYAN "\x1B[36m"
#define COLOR_WHITE "\x1B[37m"
#define COLOR_RESET "\x1B[0m"

/// Print colored char
#define PRINT_COLOR_CHAR(text_char, color) printf(color "%c" COLOR_RESET, text_char);
/// Print colored string
#define PRINT_COLOR_TEXT(text, color) printf(color "%s" COLOR_RESET, text);

/// Debug is used to print additional information and custom stuff
#define DEBUG 1

/// Default World size
#define WORLD_SIZE 20
/// (x, y) position on World
#define WORLD_POS(x, y) WORLD->Tile[x][y]

/// Value of Camera position on World on X-Axis
#define CAMERA_POS_X_ON_WORLD(x) PLAYER->PosX - CAMERA_DISTANCE + x
/// Value of Camera position on World on Y_Axis
#define CAMERA_POS_Y_ON_WORLD(y) PLAYER->PosY - CAMERA_DISTANCE + y
/// Camera display distance - always odd number for Player to be in the middle (eg. 9, 11, 13, 123, etc.)
/// Count from each side from Player in the middle
#define CAMERA_DISTANCE 7
/// Camera position based on Player coordinate
/// Player position is for Camera a (playerX/2; playerY/2) point
#define CAMERA_POS(x, y) WORLD_POS(CAMERA_POS_X_ON_WORLD(x), CAMERA_POS_Y_ON_WORLD(y))
/// Full size of the camera
#define CAMERA_SIZE (CAMERA_DISTANCE * 2) + 1

/// Commands array length
const unsigned char COMMAND_LENGTH = 7;
/// Commands definition
const char *COMMAND[] =
{
    "",
    "Commands keys:",
    "q - Exit / Quit game",
    "w - Move Player Up",
    "s - Move Player Down",
    "a - Move Player Left",
    "d - Move Player Right"
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
const unsigned char CHAR_ENEMY = 69;
/*
const unsigned char CHAR_GOLD = 36;
const unsigned char CHAR_DOORS = 43;
const unsigned char CHAR_POTION = 33;
const unsigned char CHAR_LEVEL_NEXT = 62;
const unsigned char CHAR_LEVEL_BEFORE = 60;
const unsigned char CHAR_MYSTIC_SCROLL = 63;
*/


/// ======================================================================== Types
/// World definition
typedef struct World
{
    unsigned char Size; /// Size of the World
    unsigned char Tile[WORLD_SIZE][WORLD_SIZE]; /// All World tiles / chars
} World;
/// World itself
World* WORLD;

/// Entity / Player definition
typedef struct Entity
{
    unsigned char PosX, PosY;
} Entity;
/// Player itself
Entity* PLAYER;


/// ======================================================================== Mallocs - global makers
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
unsigned char tiles_to_print[CAMERA_SIZE];

/// Returns 1 if given tile is on tiles_to_print list
unsigned char IsTileOnList(unsigned char tile)
{
    for (int ut = 0; ut < CAMERA_SIZE; ++ut) /// Check on list
        if (tiles_to_print[ut] == tile)
            return 1;
    return 0;
}

/// Print origin of the Tiles of World
void PrintWorldTileOrigin()
{
    for (int i = 0; i < CAMERA_SIZE; ++i) /// Set all array to 0 - empty space
        tiles_to_print[i] = CHAR_EMPTY;

    /// Add Player to Origin
    tiles_to_print[0] = CHAR_PLAYER;

    /// Search all region
    for (int x = 0; x < CAMERA_SIZE; ++x)
        for (int y = 0; y < CAMERA_SIZE; ++y)
        {
            unsigned char tile = CAMERA_POS(x, y); /// Currently checking tile
            if (!IsTileOnList(tile))
            {
                for (int i = 0; i < CAMERA_SIZE; ++i) /// Add new tile char to be print
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
    for (int i = 0; i < CAMERA_SIZE; ++i)
    {
        if (tiles_to_print[i] == CHAR_WALL)
            printf("\t %c - Wall \n", CHAR_WALL);
        else if (tiles_to_print[i] == CHAR_TUNNEL)
            printf("\t %c - Tunnel \n", CHAR_TUNNEL);
        else if (tiles_to_print[i] == CHAR_FLOOR)
            printf("\t %c - Floor \n", CHAR_FLOOR);
        else if (tiles_to_print[i] == CHAR_PLAYER)
            printf("\t" COLOR_CYAN " %c" COLOR_RESET " - Player \n", CHAR_PLAYER);
    }
}

/// Prints data about Player
void PrintPlayerData()
{
    printf("\n");
    printf("\t Player Data: \n");
    printf("\t Player Pos X = %i\n", PLAYER->PosX);
    printf("\t Player Pos Y = %i\n", PLAYER->PosY);
    printf("\n");
}

/// Print World and everything to screen
void PrintWorld()
{
    /// Print World from each Tile and add other Tiles and Entities
    for (int x = 0; x < CAMERA_SIZE; ++x)
    {
        for (int y = 0; y < CAMERA_SIZE; ++y)
        {
            if (x == CAMERA_DISTANCE && y == CAMERA_DISTANCE)
            {
                PRINT_COLOR_CHAR(CHAR_PLAYER, COLOR_CYAN); /// Print Player in color
            }
            /// If outside World bounds - don't print / print empty char
            else if (CAMERA_POS_X_ON_WORLD(x) < 0 ||
                     CAMERA_POS_Y_ON_WORLD(y) < 0 ||
                     CAMERA_POS_X_ON_WORLD(x) > WORLD_SIZE - 1 ||
                     CAMERA_POS_Y_ON_WORLD(y) > WORLD_SIZE - 1)
            {
                printf(" ");
            }
            else
                printf("%c", CAMERA_POS(x, y));
        }

        /// Print Movement
        if (x < COMMAND_LENGTH)
            printf("\t %s", COMMAND[x]);

        /// End Line
        printf("\n");
    }
    printf("\n");

    /// Print chars meaning
    PrintWorldTileOrigin();

    /// Print Player data
    PrintPlayerData();
}


/// ======================================================================== Player movement
/// Move Player Down
void PlayerMoveDown()
{
    if (WORLD_POS(PLAYER->PosX + 1, PLAYER->PosY) != CHAR_WALL)
        PLAYER->PosX += 1;
}

/// Move Player Up
void PlayerMoveUp()
{
    if (WORLD_POS(PLAYER->PosX - 1, PLAYER->PosY)  != CHAR_WALL)
        PLAYER->PosX -= 1;
}

/// Move Player Left
void PlayerMoveLeft()
{
    if (WORLD_POS(PLAYER->PosX, PLAYER->PosY - 1)  != CHAR_WALL)
        PLAYER->PosY -= 1;
}

/// Move Player Right
void PlayerMoveRight()
{
    if (WORLD_POS(PLAYER->PosX, PLAYER->PosY + 1)  != CHAR_WALL)
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

    char input; /// User input action

    GenerateWorld(); /// Generate World

    /// Game loop
    while (1)
    {
        system("cls"); /// Clear screen

        PrintWorld(); /// Print current World stage

        printf("Action > ");
        scanf(" %c", &input);

        /// TODO: Fix crashing when Player move
        /// Execute right command
        if (input == 'q') /// Quit the game
            return 0;
        else if (input == 's')
            PlayerMoveDown();
        else if (input == 'w')
            PlayerMoveUp();
        else if (input == 'a')
            PlayerMoveLeft();
        else if (input == 'd')
            PlayerMoveRight();
        else
            {/** Just repaint if something wrong was given as input */}
    }

    /// Ending
    return 0;
}
