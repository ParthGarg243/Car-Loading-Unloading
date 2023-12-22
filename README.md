##Multithreaded Simulation: Car and Passengers

This C program simulates a scenario where a car, with limited capacity, takes multiple passengers for a ride. Passengers eagerly await their turn to board, and the car can only depart when fully occupied. The simulation involves modeling the car and passengers as threads, with tasks such as loading, running, and unloading.

### Functionality:
- The `car` thread loads a specified number of passengers, waits for all to board, runs the ride, and unloads passengers.
- Passenger threads board when the car is available and get off after the ride.

### Key Features:
- Utilizes C's multithreading capabilities.
- Synchronization between car and passenger threads is achieved using semaphores.
- Avoids deadlock with careful synchronization.

### Usage:
1. Enter the total number of passengers and the car's capacity.
2. Simulates loading, running, and unloading procedures.
3. Prints each action to the console with appropriate passenger identification.

### Code Structure:
- `car` and `passenger` threads are defined with specified actions.
- Functions include `load()`, `unload()`, `board()`, and `offboard()`.

Ensure a seamless and deadlock-free simulation experience with this well-organized and synchronized C program. Explore multithreading and synchronization in a simulated ride-sharing scenario."
