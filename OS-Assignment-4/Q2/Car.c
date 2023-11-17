#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

#define MAX_PASSENGERS 10

pthread_mutex_t car_lock; // mutex to control access of the 'boarded' and 'unboarded' variables
sem_t board_queue; // semaphore to ensure boarding of passengers
sem_t all_boarded; // binary semaphore to signal car to start the ride
sem_t unboard_queue; // semaphore to ensure unboarding of passengers
sem_t all_unboarded; // binary semaphore to signal car to allow boarding

volatile int boarded = 0; // current number of passengers that have boarded
volatile int unboarded = 0; // current number of passengers that have unboarded
volatile int capacity = 0; // current capacity of the car thread

/* Helper functions */
void load() {
    printf("Loading passengers...\n");
    sleep(2);
}

void run() {
    printf("The car is now riding the roller coaster!\n");
    sleep(5);
}

void unload() {
    printf("Unloading passengers...\n");
    sleep(2);
}

void board(int passenger_id) {
    printf("Passenger %d has boarded the car...\n", passenger_id);
    sleep(rand() % 2);
}

void offboard(int passenger_id) {
    printf("Passenger %d has unboarded the car...\n", passenger_id);
    sleep(rand() % 2);
}

/* Thread Functions */
void* car(void* args) {
    int i;
    // Run the roller coaster indefinitely
    while (1) {
        load();

        for (i = 0; i < capacity; i++) sem_post(&board_queue); // Signal passengers to board
        sem_wait(&all_boarded); // Wait for all passengers to board

        run();
        unload();

        for (i = 0; i < capacity; i++) sem_post(&unboard_queue); // Signal passengers to unboard
        sem_wait(&all_unboarded); // Wait for all passengers to unboard
        printf("The car is now empty!\n\n");
    }
    return NULL;
}

void* passenger(void* args) {
    int passenger_id = *((int*)args);

    // Run indefinitely, threads will be destroyed when the main process exits
    while (1) {
        sem_wait(&board_queue); // Wait for the car thread signal to board

        pthread_mutex_lock(&car_lock); // Lock access to shared variable before incrementing
        boarded++;
        board(passenger_id);
        if (boarded == capacity) {
            sem_post(&all_boarded); // If this is the last passenger to board, signal the car to run
            boarded = 0;
        }
        pthread_mutex_unlock(&car_lock); // Unlock access to shared variable

        sem_wait(&unboard_queue); // Wait for the ride to end

        pthread_mutex_lock(&car_lock); // Lock access to shared variable before incrementing
        unboarded++;
        offboard(passenger_id);
        if (unboarded == capacity) {
            sem_post(&all_unboarded); // If this is the last passenger to unboard, signal the car to allow boarding
            unboarded = 0;
        }
        pthread_mutex_unlock(&car_lock); // Unlock access to shared variable
    }
    return NULL;
}

/* Main program */
int main() {
    srand(time(NULL));
    int passengers;
    printf("Enter the number of passengers: ");
    scanf("%d", &passengers);
    if (passengers > MAX_PASSENGERS) {
        printf("Number of passengers exceeds the maximum allowed.\n");
        return 1;
    }

    printf("Enter the capacity of the car: ");
    scanf("%d", &capacity);

    pthread_t car_thread;
    pthread_t passenger_threads[passengers];
    int passenger_ids[passengers];

    pthread_mutex_init(&car_lock, NULL);
    sem_init(&board_queue, 0, 0);
    sem_init(&all_boarded, 0, 0);
    sem_init(&unboard_queue, 0, 0);
    sem_init(&all_unboarded, 0, 0);

    printf("The roller coaster will run indefinitely!\n");
    printf("There are %d passengers waiting in the roller coaster queue!\n\n", passengers);

    pthread_create(&car_thread, NULL, car, NULL);

    for (int i = 0; i < passengers; i++) {
        passenger_ids[i] = i + 1; // Passenger IDs start from 1
        pthread_create(&passenger_threads[i], NULL, passenger, (void*)&passenger_ids[i]);
    }

    pthread_join(car_thread, NULL);

    printf("The roller coaster is shutting down.\n");

    pthread_mutex_destroy(&car_lock);
    sem_destroy(&board_queue);
    sem_destroy(&all_boarded);
    sem_destroy(&unboard_queue);
    sem_destroy(&all_unboarded);

    return 0;
}
