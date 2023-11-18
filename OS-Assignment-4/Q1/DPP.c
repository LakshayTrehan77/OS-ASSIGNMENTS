#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

#define NUMPHIL 5
#define LEFT id
#define RIGHT (id + 1) % NUMPHIL
#define CENTER_BOWL_1 0
#define CENTER_BOWL_2 1

enum { THINKING, HUNGRY, EATING } state[NUMPHIL];
int identity[NUMPHIL] = {0, 1, 2, 3, 4};
int forks[NUMPHIL];
// Use a struct to represent a bowl with availability status
struct Bowl {
    int available;
};
struct Bowl bowls[2] = {{1}, {1}}; // Two bowls at the center, initially available
int current_bowl ;

pthread_mutex_t lock;
pthread_cond_t cond[NUMPHIL];
pthread_t phil[NUMPHIL];


void thinking(int id);
void eating(int id);
void pickup_forks_and_bowl(int id);
void return_forks_and_bowl(int id);
void* philosopher(void* num);

int main() {
    int i = 0;

    if (pthread_mutex_init(&lock, NULL) != 0) {
        printf("\n mutex init has failed\n");
        return 1;
    }

    for (i = 0; i < NUMPHIL; i++) {
        if (pthread_cond_init(&cond[i], NULL) != 0) {
            printf("\n cond init has failed\n");
            return 1;
        }
    }

    for (i = 0; i < NUMPHIL; i++) {
        pthread_create(&phil[i], NULL, philosopher, &identity[i]);
    }

    for (i = 0; i < NUMPHIL; i++) {
        pthread_join(phil[i], NULL);
    }

    pthread_mutex_destroy(&lock);
    for (i = 0; i < NUMPHIL; i++) {
        pthread_cond_destroy(&cond[i]);
    }

    return 0;
}

void thinking(int id) {
    int thinkTime = (rand() % 3) + 1;

    printf("Philosopher %d is thinking for %d seconds\n", id, thinkTime);
    sleep(thinkTime);
    
}

void eating(int id) {
    int eatingTime = (rand() % 3) + 1;

    printf("Philosopher %d is eating for %d seconds\n", id, eatingTime);
    sleep(eatingTime);
    
}

void pickup_forks_and_bowl(int id) {
    pthread_mutex_lock(&lock);
    state[id] = HUNGRY;

    while ((state[id] == HUNGRY) && (state[LEFT] == EATING || state[RIGHT] == EATING || forks[RIGHT] == 1 || forks[LEFT] == 1 || (bowls[CENTER_BOWL_1].available == 0 && bowls[CENTER_BOWL_2].available == 0))) {
       
        pthread_cond_wait(&cond[id], &lock);
    }

    forks[LEFT] = 1;
    forks[RIGHT] = 1;

    // Choose the available bowl
    if (bowls[CENTER_BOWL_1].available) {
        current_bowl = CENTER_BOWL_1;
    } else if (bowls[CENTER_BOWL_2].available) {
        current_bowl = CENTER_BOWL_2;
    }

    bowls[current_bowl].available = 0; // Set the bowl as not available
    state[id] = EATING;
   
    pthread_mutex_unlock(&lock);
}


void return_forks_and_bowl(int id) {
    pthread_mutex_lock(&lock);
    state[id] = THINKING;

    forks[LEFT] = 0;
    forks[RIGHT] = 0;
        // Choose the available bowl
         // Choose the available bowl
    //Setting the value of the current bowl to be returned by the PHISLOSPHER
    if (current_bowl == 1){
        current_bowl--;
    }
    else {
        current_bowl++;
    }
    bowls[current_bowl].available = 1; // Set the bowl as available
    pthread_cond_signal(&cond[LEFT]);
    pthread_cond_signal(&cond[RIGHT]);
    pthread_cond_signal(&cond[CENTER_BOWL_1]);
    pthread_cond_signal(&cond[CENTER_BOWL_2]);
    pthread_mutex_unlock(&lock);
}

void* philosopher(void* num) {
    int id = *((int*)num);

    while (1) {
        thinking(id);
        pickup_forks_and_bowl(id);
        eating(id);
        return_forks_and_bowl(id);
    }
}
