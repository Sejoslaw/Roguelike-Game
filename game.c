/**
        Created by: Krzysztof Dobrzynski - k.dobrzynski94@gmail.com
                https://github.com/Sejoslaw


		Variable meaning:
			- GLOBAL_VALUE / DEFINE_VALUE / DEFINE_METHOD
			- MethodName / StructureVariableValue
			- local_variable / variable_which_is_used_by_local_function

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

/// Coordinate type - used for position manipulation
/// For example: used to specified Entity's position
typedef unsigned char Coordinate;

/// Unique identifier
typedef unsigned char Id;

/// Type which defines character in ASCII table
typedef unsigned char ASCII;

/// OS-independent operations on TRUE / FALSE values.
typedef unsigned char RG_BOOL;
#define RG_TRUE 1
#define RG_FALSE 0

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
#define COMMAND_LENGTH 7
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


/// ============================================================= ASCII Characters - which character to print in console
#define CHAR_EMPTY 0
#define CHAR_SPACE 160

#define CHAR_WALL 219
#define CHAR_TUNNEL 35
#define CHAR_FLOOR 46
#define CHAR_PLAYER 64
#define CHAR_ENEMY 69
#define CHAR_MOB 77
#define CHAR_STAIRS_UP 62
#define CHAR_STAIRS_DOWN 60
/*
#define CHAR_GOLD 36
#define CHAR_DOORS 43
#define CHAR_POTION 33
#define CHAR_MYSTIC_SCROLL 63
*/

/// Length of all registered chars
#define REGISTERED_CHAR_LENGTH 8
/// All currently registered chars array - except CHAR_EMPTY and CHAR_SPACE
const ASCII REGISTERED_CHAR[REGISTERED_CHAR_LENGTH] =
{
    CHAR_WALL,
    CHAR_TUNNEL,
    CHAR_FLOOR,
    CHAR_PLAYER,
    CHAR_ENEMY,
    CHAR_MOB,
    CHAR_STAIRS_UP,
    CHAR_STAIRS_DOWN
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


/// ======================================================================== Types
/// World definition
typedef struct World
{
    unsigned char Size; /// Size of the World
    ASCII Tile[WORLD_SIZE][WORLD_SIZE]; /// All World tiles / chars
    Id WorldId;
} World;
/// World itself
World* WORLD;

/// Entity / Player definition
typedef struct Entity
{
    Coordinate PosX, PosY;
} Entity;
/// Player itself
Entity* PLAYER;


/// ======================================================================== Mallocs - global makers
/// Create new World
World* NewWorld(Id worldId)
{
    World *new_world = malloc(sizeof(World));
    new_world->Size = WORLD_SIZE;
    new_world->WorldId = worldId;

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

    /// FIX - check where stairs are placed when World is generated
    /// FIX - check if stairs and not on top of each other
    /// Add stairs to next and previous World
    WORLD_POS(random(1, WORLD_SIZE - 1), random(1, WORLD_SIZE - 1)) = CHAR_STAIRS_UP;
    WORLD_POS(random(1, WORLD_SIZE - 1), random(1, WORLD_SIZE - 1)) = CHAR_STAIRS_DOWN;
}

/// Print current view of the Camera to the screen
void PrintCameraView()
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
                printf("%c", CHAR_EMPTY);
            }
            else if (CAMERA_POS(x, y) == CHAR_ENEMY)
            {
                PRINT_COLOR_CHAR(CHAR_ENEMY, COLOR_RED);
            }
            else if (CAMERA_POS(x, y) == CHAR_MOB)
            {
                PRINT_COLOR_CHAR(CHAR_MOB, COLOR_YELLOW);
            }
            else if (CAMERA_POS(x, y) == CHAR_STAIRS_UP)
                printf("%c", CHAR_STAIRS_UP);
            else if (CAMERA_POS(x, y) == CHAR_STAIRS_DOWN)
                printf("%c", CHAR_STAIRS_DOWN);
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
}

/// Array which holds data of all used Tiles - used when Origin is printing
/// This array is too big to hold worst case scenario which is that all tiles are different (not possible).
ASCII tiles_to_print[CAMERA_SIZE * CAMERA_SIZE];

/// Returns 1 if given tile is in tiles_to_print array
RG_BOOL IsTileInArray(ASCII tile)
{
    for (int ut = 0; ut < CAMERA_SIZE * CAMERA_SIZE; ++ut) /// Check in array
        if (tiles_to_print[ut] == tile)
            return RG_TRUE;
    return RG_FALSE;
}

/// Add specified tile to tiles_to_print array
void AddTileToArray(ASCII tile)
{
    /// Search for free space in array
    for (int i = 1; i < CAMERA_SIZE * CAMERA_SIZE; ++i) /// start from 1 because Player is in space 0
    {
        if (tile == tiles_to_print[i]) /// Tile is already in array
            return;
        else if (tiles_to_print[i] == CHAR_EMPTY || tiles_to_print[i] == CHAR_SPACE)
        {
            tiles_to_print[i] = tile;
            return;
        }
    }
}

/// Print origin of the Tiles of World
void PrintOrigin()
{
    for (int i = 0; i < CAMERA_SIZE * CAMERA_SIZE; ++i) /// Set all array to 0 - empty space
        tiles_to_print[i] = CHAR_EMPTY;

    /// Add Player to Origin
    tiles_to_print[0] = CHAR_PLAYER;

    /// FIX - Origin is not always correct
    /// Search all region
    for (int y = 0; y < CAMERA_SIZE; ++y)
        for (int x = 0; x < CAMERA_SIZE; ++x)
        {
            if (!(CAMERA_POS_X_ON_WORLD(x) < 0) || !(CAMERA_POS_Y_ON_WORLD(y) < 0))
            {
                ASCII tile = CAMERA_POS(x, y); /// Currently checking tile
                if (tile != CHAR_EMPTY && tile != CHAR_SPACE) /// If checking tile is an empty space - don't add it
                    for (int i = 0; i < REGISTERED_CHAR_LENGTH; ++i)
                        if (tile == REGISTERED_CHAR[i])
                            AddTileToArray(tile);
            }
        }

    /// Print tiles
    printf("\t Origin: \n");
    /// Player will be always print
    printf("\t" COLOR_CYAN " %c" COLOR_RESET " - Player \n", CHAR_PLAYER);
    for (int i = 1; i < CAMERA_SIZE * CAMERA_SIZE; ++i)
    {
        if (tiles_to_print[i] == CHAR_WALL)
            printf("\t %c - Wall \n", CHAR_WALL);
        else if (tiles_to_print[i] == CHAR_TUNNEL)
            printf("\t %c - Tunnel \n", CHAR_TUNNEL);
        else if (tiles_to_print[i] == CHAR_FLOOR)
            printf("\t %c - Floor \n", CHAR_FLOOR);
        else if (tiles_to_print[i] == CHAR_ENEMY)
            printf("\t" COLOR_RED " %c" COLOR_RESET " - Enemy \n", CHAR_ENEMY);
        else if (tiles_to_print[i] == CHAR_MOB)
            printf("\t" COLOR_YELLOW " %c" COLOR_RESET " - Mob \n", CHAR_MOB);
        else if (tiles_to_print[i] == CHAR_STAIRS_UP)
            printf("\t %c - Stairs Up \n", CHAR_STAIRS_UP);
        else if (tiles_to_print[i] == CHAR_STAIRS_DOWN)
            printf("\t %c - Stairs Down \n", CHAR_STAIRS_DOWN);
    }
}

/// Prints additional data
void PrintAdditionalData()
{
    printf("\n");
    printf("\t Additional Data: \n");
    printf("\t World Id = %i \n", WORLD->WorldId);
    printf("\t Player Pos X = %i\n", PLAYER->PosX);
    printf("\t Player Pos Y = %i\n", PLAYER->PosY);
    printf("\n");
}

/// Move specified Entity to specified position
void MoveEntity(Entity* entity, Coordinate x, Coordinate y)
{
    if (WORLD_POS(x, y) != CHAR_WALL)
    {
        entity->PosX = x;
        entity->PosY = y;
    }
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

    WORLD = NewWorld(0); /// Set new World
    PLAYER = NewPlayer(); /// Set new Player

    char input; /// User input action

    GenerateWorld(); /// Generate World

    /// Game loop
    while (1)
    {
        system("cls"); /// Clear screen

        PrintCameraView(); /// Print current Camera stage
        PrintOrigin(); /// Print chars meaning - origin
        PrintAdditionalData(); /// Print additional data

        printf("Action > ");
        scanf(" %c", &input);

        /// TODO: Fix crashing when Player move
        /// Execute right command
        if (input == 'q') /// Quit the game
            return 0;
        else if (input == 'w')
            MoveEntity(PLAYER, PLAYER->PosX - 1, PLAYER->PosY);
        else if (input == 's')
            MoveEntity(PLAYER, PLAYER->PosX + 1, PLAYER->PosY);
        else if (input == 'a')
            MoveEntity(PLAYER, PLAYER->PosX, PLAYER->PosY - 1);
        else if (input == 'd')
            MoveEntity(PLAYER, PLAYER->PosX, PLAYER->PosY + 1);
    }

    /// Ending
    return 0;
}
