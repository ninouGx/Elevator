#ifndef ELEVATOR_DISPLAY_H
#define ELEVATOR_DISPLAY_H

#define ELEVATOR_SIZE_LIMIT 5
#define ELEVATOR_CHAR '#'
#define BUILDING_HEIGHT (ELEVATOR_SIZE_LIMIT + 4) // Including roof and ground
#define BUFFER_WIDTH 15
#define INPUT_DISPLAY_HEIGHT (BUILDING_HEIGHT + 2)

void clearScreen();
void moveCursorToStart();
void clearLine();
void moveCursorToColumn(int col);
void moveCursorToLine(int line);
void printElevator(int floor);

#endif