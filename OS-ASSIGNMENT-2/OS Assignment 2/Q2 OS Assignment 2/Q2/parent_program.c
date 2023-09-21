#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h> // For open
#include <string.h> // For strerror
#include <errno.h>

void clearExecutionTimeFile() {
    FILE *file = fopen("execution_time.txt", "w");
    if (file != NULL) {
        fclose(file);
    } else {
        perror("Failed to open the file for writing");
        exit(1);
    }
}

int main() {
    // Clear the previous entries in execution_time.txt
    clearExecutionTimeFile();

    // Initialize an array to store child process IDs
    pid_t child_pids[3];

    // Array of policies
    char *policies[] = {"SCHED_OTHER", "SCHED_RR", "SCHED_FIFO"};

    // Create and open the "policies.txt" file for writing
    FILE *policies_file = fopen("policies.txt", "w");
    if (policies_file == NULL) {
        perror("Failed to open policies.txt for writing");
        exit(1);
    }

    // Write policies to the "policies.txt" file
    for (int i = 0; i < 3; i++) {
        fprintf(policies_file, "%s\n", policies[i]);
    }

    // Close the "policies.txt" file
    fclose(policies_file);

    for (int i = 0; i < 3; i++) {
        // Create a child process
        child_pids[i] = fork();

        if (child_pids[i] == 0) {
            // Child process
            char process_name[20];
            snprintf(process_name, sizeof(process_name), "./process%d", i + 1);

            execl(process_name, process_name, NULL);
            // If execl fails, handle the error here
            perror("Exec failed");
            exit(1);
        } else if (child_pids[i] < 0) {
            // Error handling for fork
            perror("Fork failed");
            exit(1);
        }
    }

    // Parent process waits for all child processes to complete
    for (int i = 0; i < 3; i++) {
        int status;
        waitpid(child_pids[i], &status, 0);
    }

    return 0;
}
