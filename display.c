#include "display.h"
#include <stdio.h>
#include <stdlib.h>

void clearScreen()
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void moveCursorToStart()
{
    printf("\033[H");
}

void clearLine()
{
    printf("\033[K");
}

void moveCursorToColumn(int col)
{
    printf("\033[%dG", col);
}

void moveCursorToLine(int line)
{
    printf("\033[%d;0H", line);
}

/*
lighthouse
   ^
  /o\ top
  | | last floor
 |===|
  | |  1st floor to n-1 floor so print the number of floors between top and ground
 =====
_||_||_ ground floor
*/
static const char *BUILDING_TEMPLATE[] = {
    "   ^",    // roof top
    "  /o\\",  // roof top
    "  |%c|",  // top floor
    " |===|",  // separator
    "  |%c|",  // regular floors
    " |===|",  // separator
    "_||%c||_" // ground floor
};

void printElevator(int floor)
{
    static char building[BUILDING_HEIGHT][BUFFER_WIDTH];
    int current_line = 0;

    // Add roof (no elevator here)
    sprintf(building[current_line++], "%s", BUILDING_TEMPLATE[0]);
    sprintf(building[current_line++], "%s", BUILDING_TEMPLATE[1]);

    // Add top floor
    sprintf(building[current_line++], BUILDING_TEMPLATE[2],
            floor == ELEVATOR_SIZE_LIMIT - 1 ? ELEVATOR_CHAR : ' ');

    // Separator
    sprintf(building[current_line++], "%s", BUILDING_TEMPLATE[3]);

    // Add middle floors (from top to bottom)
    for (int i = ELEVATOR_SIZE_LIMIT - 2; i > 0; i--)
    {
        sprintf(building[current_line++], BUILDING_TEMPLATE[4],
                floor == i ? ELEVATOR_CHAR : ' ');
    }

    // Add ground separator
    sprintf(building[current_line++], "%s", BUILDING_TEMPLATE[5]);

    // Add ground floor
    sprintf(building[current_line], BUILDING_TEMPLATE[6],
            floor == 0 ? ELEVATOR_CHAR : ' ');

    // clearScreen();
    moveCursorToStart();
    for (int i = 0; i < BUILDING_HEIGHT; i++)
    {
        printf("%s\n", building[i]);
    }
    fflush(stdout);
}