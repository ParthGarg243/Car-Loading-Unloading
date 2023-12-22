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
int low;
int high;

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
    printf("\nCar is ready for loading passengers.\n");
    //ADD WAIT BEFORE LOADING HASNT FINISHED
    
    // Loading passengers
    load();
    sem_post(&waitLoading);

    for (int i = low; i < high; ++i) {
        sem_post(&boardingSem); // Signal that a passenger can board
        // board(i);
    }

    // Waiting for all passengers to board
    sem_wait(&rideSem);

    // Simulating the ride
    printf("Car is running...\n");      // won't work correctly in the 2nd iteration
    
    sleep(2); // Simulating the ride duration

    // Unloading passengers
    unload();
    sem_post(&waitUnloading);
     //ADD WAIT BEFORE UNLOADING HASNT FINISHED
    
    for (int i = low; i < high; ++i) {
        sem_post(&unboardingSem); // Signal that a passenger can unboard
        // offboard(i);
    }
    sem_wait(&rideSem);
    pthread_exit(NULL);
}

// Passenger thread function
void* passenger(void* arg) {
    int id = *((int*)arg);

    // Waiting for the car to complete loading before boarding
    sem_wait(&waitLoading);
    sem_wait(&boardingSem);
    sem_wait(&boardMutex);      // why is this not working...?
    board(id);
    boardedCount++;
    sem_post(&boardMutex);
    sem_post(&waitLoading);

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
    sem_post(&waitUnloading);

    // If all passengers have unboarded, signal the ride is over
    if (unboardedCount == carCapacity) {
        sem_post(&rideSem);
    }

    pthread_exit(NULL);
}

int main() {
    // Get user input for the number of passengers and car capacity
    printf("Enter the number of passengers: ");
    scanf("%d", &numPassengers);

    printf("Enter the car capacity: ");
    scanf("%d", &carCapacity);

    int maxCars = ceil(numPassengers / carCapacity);
    if (numPassengers % carCapacity != 0) {
        maxCars++;
    }

    pthread_t carThread[maxCars];
    pthread_t passengerThreads[numPassengers];

    // Initialize semaphores
    int passengerCount = numPassengers;
    sem_init(&boardingSem, 0, 0);
    sem_init(&unboardingSem, 0, 0);
    sem_init(&waitLoading, 0, 0);
    sem_init(&waitUnloading, 0, 0);
    sem_init(&rideSem, 0, 0);       // set to 0 when logical error is fixed
    sem_init(&loadCompleteSem, 0, 0);
    sem_init(&loadMutex, 0, 1);
    sem_init(&unloadMutex, 0, 1);
    sem_init(&boardMutex, 0, 1);
    sem_init(&offboardMutex, 0, 1);

    for (int bloop = 0; bloop < maxCars; bloop++) {
        boardedCount = 0;
        unboardedCount = 0;
        low = bloop * carCapacity;
        if (low + carCapacity > numPassengers) {
            high = numPassengers;
        } 
        else {
            high = low + carCapacity;
        }

        if (carCapacity > passengerCount) {
            printf("\nCar capacity is greater than the number of passengers left\n");
            return 0;
        }

        // Create car thread inside the loop after calculating low and high
        pthread_create(&carThread[bloop], NULL, car, NULL);

        // Create and join passenger threads
        for (int passengerIndex = low; passengerIndex < high; ++passengerIndex) {
            int* id = malloc(sizeof(int));
            *id = passengerIndex;
            pthread_create(&passengerThreads[passengerIndex], NULL, passenger, id);
            passengerCount--;
        }

        // Join passenger threads
        for (int passengerIndex = low; passengerIndex < high; ++passengerIndex) {
            pthread_join(passengerThreads[passengerIndex], NULL);
        }

        // Join car thread
        pthread_join(carThread[bloop], NULL);
    }

    return 0;
}
