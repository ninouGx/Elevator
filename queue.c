#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>
#include "queue.h"

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