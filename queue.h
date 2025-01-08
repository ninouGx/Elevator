#ifndef QUEUE_H
#define QUEUE_H

#include <pthread.h>
#include <stdbool.h>
#include "display.h"

typedef struct
{
    int items[ELEVATOR_SIZE_LIMIT];
    int front;
    int count;
    pthread_mutex_t mutex;
    pthread_cond_t not_full;
    pthread_cond_t not_empty;
} Queue;

void initQueue(Queue *q);

bool isEmpty(Queue *q);

bool isFull(Queue *q);

bool contains(Queue *q, int floor);

void enqueue(Queue *q, int value);

void dequeue(Queue *q);

int peek(Queue *q);

void printQueue(Queue *q);

#endif