#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>
#include "display.h"
#include "queue.h"

#define NB_THREADS 3

volatile int elevatorFloor = 0;
volatile bool shouldExit = false;

void *displayUpdater(void *arg)
{
    Queue *elevatorQueue = (Queue *)arg;
    bool firstTime = true;
    while (!shouldExit)
    {
        printElevator(elevatorFloor);
        clearLine();
        printQueue(elevatorQueue);

        moveCursorToLine(INPUT_DISPLAY_HEIGHT);
        clearLine();
        printf("\rEnter the floor you want to go to (0-%d): ", ELEVATOR_SIZE_LIMIT - 1);
        fflush(stdout);
        sleep(1);
    }
    return NULL;
}

void *inputForRequest(void *arg)
{
    Queue *elevatorQueue = (Queue *)arg;
    int floor;
    char input[10];
    while (!shouldExit)
    {
        if (fgets(input, sizeof(input), stdin) != NULL)
        {
            if (input[0] == 'q' || input[0] == 'Q')
            {
                shouldExit = true;
                break;
            }

            if (sscanf(input, "%d", &floor) == 1 &&
                (floor >= 0 && floor < ELEVATOR_SIZE_LIMIT))
            {
                enqueue(elevatorQueue, floor);
            }
        }
    }
    return NULL;
}

void *elevator(void *arg)
{
    Queue *elevatorQueue = (Queue *)arg;
    int previousFloor = 0, requestedFloor = 0;
    while (!shouldExit)
    {
        previousFloor = elevatorFloor;
        if (!isEmpty(elevatorQueue))
        {
            requestedFloor = peek(elevatorQueue);
            if (requestedFloor == -999)
            {
                continue;
            }

            if (elevatorFloor < requestedFloor)
            {
                elevatorFloor++;
            }
            else if (elevatorFloor > requestedFloor)
            {
                elevatorFloor--;
            }
            else
            {
                dequeue(elevatorQueue);
            }
        }
        sleep(1);
    }
    return NULL;
}

int main()
{
    Queue elevatorQueue;
    pthread_t elevatorThreads[NB_THREADS];

    initQueue(&elevatorQueue);

    pthread_create(&elevatorThreads[0], NULL, displayUpdater, &elevatorQueue);
    pthread_create(&elevatorThreads[1], NULL, inputForRequest, &elevatorQueue);
    pthread_create(&elevatorThreads[2], NULL, elevator, &elevatorQueue);

    for (int i = 0; i < NB_THREADS; i++)
    {
        pthread_join(elevatorThreads[i], NULL);
    }

    pthread_mutex_destroy(&elevatorQueue.mutex);
    pthread_cond_destroy(&elevatorQueue.not_full);
    pthread_cond_destroy(&elevatorQueue.not_empty);

    return 0;
}