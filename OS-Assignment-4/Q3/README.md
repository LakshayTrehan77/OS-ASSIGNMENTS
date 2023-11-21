# CONTRIBUTORS
1. LAKSHAY TREHAN (2022267)
2. NISHANT SINGH (2022595)

# CAR-BRIDGE MANAGEMENT PROBLEM USING SEMAPHORES

    - This code is based on the problem of bridge connecting two sides of a river. 
    - The bridge helps the cars to cross the river. The use of semaphores enables us to keep a constraint such that only 5 cars at a time cross the bridge. 
    - The program preserves synchronization by allowing one car from each side to cross the bridge at a time, avoiding conflicts that would arise if cars from opposite sides sought to pass at the same time. 
    - Each vehicle thread proceeds in the same way: it acquires the proper semaphore, crosses the bridge by a delay with the help of sleep() function, and releases the semaphore to allow the next car to proceed. 
    - The execution of the code guarantees the safety conditions and restricted access to the bridge, allowing automobiles from either side to cross safely without stalemate or accident hazards.

# LOGIC OF THE CODE

1. Threads
    - They represent cars on the both sides of the river that are intended to cross the river.
2. Semaphores
    - This acts as a constraint that restricts and allows maximum of 5 cars at a time on the bridge.
3. Crossing the Bridge
    - This is to prevent the collision between the cars. Therefore, only one car in a particular direction is allowed to cross the bridge.
4. User Input
    - The number of cars are determined by taking an inout from the user for the cars waiting on the either sides of the bridge.

# Avoiding Bugs & Error Handling

1. The use of semaphore manages the access to the bridge that ensures the number of cars cross are kept under control. This also prevents overcrowding and any kind of collision.

2. As the cars from each side are made to cross the bridge one by one, with a limit ofnot more than 5 cars, the access of the cars is controlled and a state of race is prevented.

3. The feature of allowing only one car at a time from either side to cross the bridge avoids the state of potential deadlock or collision.

4. The structural integrety of the code is maintained with the help of thread sequences for acquiring, releasing, crossing and systematic order of traversal.
