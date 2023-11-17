# PASSENGER-CAR RIDE SIMULATION USING SEMAPHORES

This question is based on the idea of monitoring a car's passenger simulation. The car waits until it is fully loaded with the passengers. Then only the ride of the car can start

The things we are handling in this question are:
    - Passenger Capacity
    - Limitted Passengers Boarding
    - Passengers Deboarding the car
    - Running and Unloading procedures



### LOGIC OF THE CODE ###

# 1. THREAD CREATION
Two threads, namely "carThread" and "passengerThread" are being created. The "carThread" represents the roller caoster car. The "passengerThread" represents the individual passengers.

# 2. SEMAPHORES
Semaphores used to establish a co-ordination between the car and the passengers are:
    - board_queue
    - all_boarded
    - unboard_queue
    - all_unboarded

# 3. WORKING OF THE CAR
The "carThread" manages the loading, running, unloading processes using semaphores by giving signal to the passengers for the loading and unloading.

# 4. WORKING OF THE PASSENGER
The behaviour of the passenger is stimulated by the "passengerThread" by signals from the car to board and unboard. It also ensures about the synchronization with the car's ride phase.

# 5. USE OF MUTEX
Mutexes "check_in_lock" and "riding_lock" are used  to ensure access to shared variables to prevent the condition of race. The variables used are "boared" and "unboarded".



### CONCURRENCY AND AVOIDING BUGS ###

# 1. USE OF SEMAPHORE
The proper signaling and waiting among threads is maintained with the semaphore synchronization. It allows baording and unboarding activities of the passengers based on their car's status.

# 2. MUTEX FOR SHARED VARIABLES
Mutexes provide mutual exclusion of the shared variables. It prevents data races among the threads.

# 3. RESORCE ALLOCATION CONTROL
The controlled access is ensured with the use of semaphores and mutextes. It prevents multiple threads from accessing critical sections.

# 4. RIDE SEQUENCE EXECUTION
The sequence of the rides ensures that there is a particular pattern followed by the cars and passengers. This avoids confilcts and concurrency difficulties during the ride phase.
