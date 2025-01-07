#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>
#include "display.h"

#define NB_THREADS 3

volatile int elevatorFloor = 0;
volatile bool shouldExit = false;

typedef struct
{
    int items[ELEVATOR_SIZE_LIMIT];
    int front;
    int count;
    pthread_mutex_t mutex;
    pthread_cond_t not_full;
    pthread_cond_t not_empty;
} Queue;

void initQueue(Queue *q)
{
    q->front = 0;
    q->count = 0;
    pthread_mutex_init(&q->mutex, NULL);
    pthread_cond_init(&q->not_full, NULL);
    pthread_cond_init(&q->not_empty, NULL);
}

bool isEmpty(Queue *q) { return (q->count == 0); }

bool isFull(Queue *q) { return (q->count == ELEVATOR_SIZE_LIMIT); }

bool contains(Queue *q, int floor)
{
    for (int i = 0; i < q->count; i++)
    {
        int pos = (q->front + i) % ELEVATOR_SIZE_LIMIT;
        if (q->items[pos] == floor)
        {
            return true;
        }
    }
    return false;
}

// Add an element to the queue
void enqueue(Queue *q, int value)
{
    pthread_mutex_lock(&q->mutex);
    if (contains(q, value))
    {
        pthread_mutex_unlock(&q->mutex);
        return;
    }

    while (isFull(q))
    {
        pthread_cond_wait(&q->not_full, &q->mutex);
    }
    int index = (q->front + q->count) % ELEVATOR_SIZE_LIMIT;
    q->items[index] = value;
    q->count++;

    pthread_cond_signal(&q->not_empty);
    pthread_mutex_unlock(&q->mutex);
}

// Remove first element of the queue
void dequeue(Queue *q)
{
    pthread_mutex_lock(&q->mutex);
    while (isEmpty(q))
    {
        pthread_cond_wait(&q->not_empty, &q->mutex);
    }
    q->front = (q->front + 1) % ELEVATOR_SIZE_LIMIT;
    q->count--;
    pthread_cond_signal(&q->not_full);
    pthread_mutex_unlock(&q->mutex);
}

// Get first element of the queue
int peek(Queue *q)
{
    pthread_mutex_lock(&q->mutex);
    int value = -999;
    if (!isEmpty(q))
    {
        value = q->items[q->front];
    }
    pthread_mutex_unlock(&q->mutex);
    return value;
}

void printQueue(Queue *q)
{
    pthread_mutex_lock(&q->mutex);
    if (isEmpty(q))
    {
        printf("No pending requests\n");
        pthread_mutex_unlock(&q->mutex);
        return;
    }

    printf("Next floors: ");
    for (int i = 0; i < q->count; i++)
    {
        int pos = (q->front + i) % ELEVATOR_SIZE_LIMIT;
        printf("%s%d", (i == 0) ? "" : "->", q->items[pos]);
    }
    pthread_mutex_unlock(&q->mutex);
}

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