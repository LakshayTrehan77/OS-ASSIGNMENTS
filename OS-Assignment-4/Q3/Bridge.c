#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define EAST 1
#define WEST 2

typedef struct bridge
{
    int cars;
    int direction;
    sem_t mutex;
    sem_t max_cars_sem;
} bridge_t;

bridge_t shared_bridge = {
    .cars = 0,
    .direction = WEST,
    .mutex = PTHREAD_MUTEX_INITIALIZER,
    .max_cars_sem = PTHREAD_MUTEX_INITIALIZER,
};

void arrive(bridge_t *bridge, int direction)
{
    sem_wait(&bridge->mutex);

    while (bridge->cars > 0 && (bridge->cars > 2 || bridge->direction != direction))
    {
        sem_post(&bridge->mutex);
        sem_wait(&bridge->mutex);
    }

    if (bridge->cars == 0)
        bridge->direction = direction;

    bridge->cars++;
    sem_post(&bridge->mutex);
}

void cross(bridge_t *bridge)
{
    sem_wait(&bridge->max_cars_sem); // Wait if maximum cars are already on the bridge

    sem_wait(&bridge->mutex);

    if (bridge->direction == 1)
        printf("Crossing East ... number of Cars on bridge = %d \n", bridge->cars);
    else
        printf("Crossing West ... number of Cars on bridge = %d \n", bridge->cars);

    sem_post(&bridge->mutex);

    sleep(1);

    sem_post(&bridge->max_cars_sem); // Release the semaphore after crossing
}

void leave(bridge_t *bridge)
{
    sem_wait(&bridge->mutex);

    bridge->cars--;

    sem_post(&bridge->mutex);
}

void drive(bridge_t *bridge, int direction)
{
    arrive(bridge, direction);
    cross(bridge);
    leave(bridge);
}

void *east(void *data)
{
    drive((bridge_t *)data, EAST);
    return NULL;
}

void *west(void *data)
{
    drive((bridge_t *)data, WEST);
    return NULL;
}

int run(int nw, int ne)
{
    int i, n = nw + ne;
    pthread_t thread[n];

    sem_init(&shared_bridge.mutex, 0, 1);
    sem_init(&shared_bridge.max_cars_sem, 0, 5);

    for (i = 0; i < n; i++)
        if (pthread_create(&thread[i], NULL, i < nw ? east : west, &shared_bridge))
        {
            printf("thread creation failed\n");
            return EXIT_FAILURE;
        }

    for (i = 0; i < n; i++)
        if (thread[i])
            pthread_join(thread[i], NULL);

    sem_destroy(&shared_bridge.mutex);
    sem_destroy(&shared_bridge.max_cars_sem);

    return EXIT_SUCCESS;
}

// after compiling use ./bridge -e X -w Y
// X: number of cars going to east
// Y: number of cars going to west
// use ./bridge -h for help
int main(int argc, char **argv)
{
    int c, nw = 1, ne = 1;
    while ((c = getopt(argc, argv, "e:w:h")) >= 0)
    {
        switch (c)
        {
        case 'e':
            if ((ne = atoi(optarg)) <= 0)
            {
                fprintf(stderr, "Number of cars going east must be greater than 0\n");
                exit(EXIT_FAILURE);
            }
            break;
        case 'w':
            if ((nw = atoi(optarg)) <= 0)
            {
                fprintf(stderr, "Number of cars going west must be greater than 0\n");
                exit(EXIT_FAILURE);
            }
            break;
        case 'h':
            printf("Usage: bridge [-e east] [-w west] [-h]\n");
            exit(EXIT_SUCCESS);
        }
    }
    return run(nw, ne);
}
