#include <stdio.h>
#include <stdint.h>
#include <sched.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

int main() {
    struct sched_param param;
    param.sched_priority = 50; // Set the priority for SCHED_RR scheduling
    sched_setscheduler(0, SCHED_FIFO, &param);

    // Print the current scheduling policy
    int current_policy = sched_getscheduler(0);
    char *policy_str;

    switch (current_policy) {
        case SCHED_OTHER:
            policy_str = "SCHED_OTHER (Default)";
            break;
        case SCHED_FIFO:
            policy_str = "SCHED_FIFO (FIFO)";
            break;
        case SCHED_RR:
            policy_str = "SCHED_RR (Round Robin)";
            break;
        default:
            policy_str = "Unknown";
    }

    printf("Current scheduling policy of this process: %s\n", policy_str);

    struct timespec start_time, end_time;

    // Record the start time
    if (clock_gettime(CLOCK_MONOTONIC, &start_time) == -1) {
        perror("clock_gettime");
        return 1;
    }

    pid_t pid = fork();

    if (pid == -1) {
        perror("Fork failed");
        return 1;
    } else if (pid == 0) {
        // Child process

        // Use execvp to execute the ./wordcount executable
        char *argv[] = {"./wordcount", NULL}; // Modify this array to pass arguments if needed
        execvp(argv[0], argv);

        // If execvp fails, it will reach here
        perror("execvp");
        exit(1);
    } else {
        // Parent process
        int status;

        // Wait for the child process to finish
        waitpid(pid, &status, 0);

        // Record the end time
        if (clock_gettime(CLOCK_MONOTONIC, &end_time) == -1) {
            perror("clock_gettime");
            return 1;
        }

        if (WIFEXITED(status)) {
            // Child process exited normally
            double execution_time = (end_time.tv_sec - start_time.tv_sec) +
                                    (end_time.tv_nsec - start_time.tv_nsec) / 1e9;

           

            // Save the execution time as an integer in a text file
            FILE *file = fopen("execution_time.txt", "a");
            if (file != NULL) {
                fprintf(file, "%f\n", execution_time);
                fclose(file);
            } else {
                perror("Failed to open the file for writing");
            }
        } else {
            // Child process did not exit normally
            printf("Child process did not exit normally\n");
        }
    }

    return 0;
}
