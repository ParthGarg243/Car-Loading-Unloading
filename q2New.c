#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <math.h>

#define MAX_PASSENGERS 10

// Semaphore declarations
sem_t boardingSem;
sem_t unboardingSem;
sem_t rideSem;
sem_t loadCompleteSem; // Semaphore to signal loading completion
sem_t unloadCompleteSem; // Semaphore to signal unloading completion
sem_t loadMutex;
sem_t unloadMutex;
sem_t boardMutex;
sem_t offboardMutex;
sem_t waitLoading;
sem_t waitUnloading;

// Number of passengers and car capacity
int numPassengers;
int carCapacity;

// Counter to keep track of boarded passengers
int boardedCount = 0;

// Counter to keep track of unboarded passengers
int unboardedCount = 0;

// Function to load passengers onto the car
void load() {
    sem_wait(&loadMutex);
    printf("Loading passengers...\n");
    sem_post(&waitLoading);
    sem_post(&loadMutex);
}

// Function to unload passengers from the car
void unload() {
    sem_wait(&unloadMutex);
    printf("Unloading passengers...\n");
    sem_post(&waitUnloading);
    sem_post(&unloadMutex);
}

// Function for a passenger boarding the car
void board(int id) {
    printf("Passenger %d boards the ride.\n", id);
}

// Function for a passenger getting off the car
void offboard(int id) {
    printf("Passenger %d gets off the ride.\n", id);
}

// Car thread function
void* car(void* arg) {
    printf("Car is ready for loading passengers.\n");
    //ADD WAIT BEFORE LOADING HASNT FINISHED
    
    // Loading passengers
    load();
    sem_wait(&waitLoading);


    for (int i = 0; i < carCapacity; ++i) {
        sem_post(&boardingSem); // Signal that a passenger can board
        board(i);
    }

    // Waiting for all passengers to board
    sem_wait(&rideSem);

    // Simulating the ride
    printf("Car is running...\n");
    sleep(2); // Simulating the ride duration

    // Unloading passengers
    unload();
    sem_wait(&waitUnloading);
     //ADD WAIT BEFORE UNLOADING HASNT FINISHED
    

    for (int i = 0; i < carCapacity; ++i) {
        sem_post(&unboardingSem); // Signal that a passenger can unboard
        offboard(i);
    }

    pthread_exit(NULL);
}

// Passenger thread function
void* passenger(void* arg) {
    int id = *((int*)arg);

    // Waiting for the car to complete loading before boarding
    sem_wait(&waitLoading);
    sem_wait(&boardingSem);

    sem_wait(&boardMutex);
    board(id);
    boardedCount++;
    sem_post(&boardMutex);

    // If all passengers have boarded, signal the car to start the ride
    if (boardedCount == carCapacity) {
        sem_post(&rideSem);
    }

    // Waiting for the ride to finish
    sem_wait(&waitUnloading);
    sem_wait(&unboardingSem);

    sem_wait(&offboardMutex);
    offboard(id);
    unboardedCount++;
    sem_post(&offboardMutex);

    // If all passengers have unboarded, signal the ride is over
    if (unboardedCount == carCapacity) {
        sem_post(&rideSem);
    }

    pthread_exit(NULL);
}