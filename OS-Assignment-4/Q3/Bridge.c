#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define EAST 1
#define WEST 2

typedef struct car
{
    int id;
    int direction;
} car_t;

typedef struct bridge
{
    int direction;
    int max_cars;
    sem_t mutex;
    sem_t max_cars_sem;
    car_t *car_list;
} bridge_t;

bridge_t shared_bridge = {
    .direction = EAST,
    .max_cars = 5,
    .mutex = PTHREAD_MUTEX_INITIALIZER,
    .max_cars_sem = PTHREAD_MUTEX_INITIALIZER,
};

int count;
int left_print_statement;
int right_print_statement;
int left_cars, right_cars;


void arrive(bridge_t *bridge, int direction, int car_id)
{
    sem_wait(&bridge->mutex);

    while ((bridge->direction != direction) && (bridge->car_list[car_id - 1].direction != 0))
    {
        sem_post(&bridge->mutex);
        sem_wait(&bridge->mutex);
    }

    bridge->car_list[car_id - 1].direction = direction;

    sem_post(&bridge->mutex);
}

void cross(bridge_t *bridge, int car_id)
{


    sem_wait(&bridge->max_cars_sem); // Wait if maximum cars are already on the bridge

    sem_wait(&bridge->mutex);
    

    if (((bridge->direction == EAST && count < 5 && right_print_statement < right_cars)))
    {
        
        printf("Right Car %d Crossing the bridge\n", car_id);
        right_print_statement++;
        
    }
    else if ((bridge->direction == WEST && count < 5 && left_print_statement < left_cars )|| left_print_statement<left_cars)
    {
        printf("Left Car %d Crossing the bridge\n", car_id);
        left_print_statement++;
    }
    else if (right_print_statement<right_cars){
        printf("Right Car %d Crossing the bridge\n", car_id);
    }

    count++;

    if (count == 5)
    {
        count = 0;
        bridge->direction = (bridge->direction == EAST) ? WEST : EAST;
    }

    sem_post(&bridge->mutex);

    sleep(1);

    sem_post(&bridge->max_cars_sem); // Release the semaphore after crossing
}

void leave(bridge_t *bridge, int car_id)
{
    sem_wait(&bridge->mutex);

    bridge->car_list[car_id - 1].direction = 0;

    sem_post(&bridge->mutex);
}

void drive(bridge_t *bridge, int direction, int car_id)
{
    arrive(bridge, direction, car_id);
    cross(bridge, car_id);
    leave(bridge, car_id);
}

void *left(void *data)
{
    car_t *car = (car_t *)data;
    drive(&shared_bridge, WEST, car->id);
    return NULL;
}

void *right(void *data)
{
    car_t *car = (car_t *)data;
    drive(&shared_bridge, EAST, car->id);
    return NULL;
}

int run(int nw, int ne)
{
    int i, n = nw + ne;
    pthread_t thread[n];
    shared_bridge.car_list = malloc(n * sizeof(car_t));

    sem_init(&shared_bridge.mutex, 0, 1);
    sem_init(&shared_bridge.max_cars_sem, 0, shared_bridge.max_cars);

    for (i = 0; i < n; i++)
    {
        shared_bridge.car_list[i].id = i + 1;
        shared_bridge.car_list[i].direction = 0;
        if (pthread_create(&thread[i], NULL, i < nw ? left : right, &shared_bridge.car_list[i]))
        {
            printf("thread creation failed\n");
            return EXIT_FAILURE;
        }
    }

    for (i = 0; i < n; i++)
        if (thread[i])
            pthread_join(thread[i], NULL);

    sem_destroy(&shared_bridge.mutex);
    sem_destroy(&shared_bridge.max_cars_sem);
    free(shared_bridge.car_list);

    return EXIT_SUCCESS;
}

int main(int argc, char **argv)
{
    printf("Enter the number of Left cars: ");
    scanf("%d", &left_cars);

    printf("Enter the number of Right cars: ");
    scanf("%d", &right_cars);

    // Set the direction of the shared bridge to the left if left_cars are greater than right_cars
    
    

    return run(left_cars, right_cars);
}

