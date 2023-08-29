#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <dirent.h>

int removeDirectory(const char *dirName) {
    DIR *dir = opendir(dirName);
    if (dir == NULL) {
        return 0; // Directory doesn't exist
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            // Build the path to the item in the directory
            char itemPath[PATH_MAX];
            snprintf(itemPath, sizeof(itemPath), "%s/%s", dirName, entry->d_name);

            if (entry->d_type == DT_DIR) {
                // Recursively remove subdirectories
                removeDirectory(itemPath);
            } else {
                // Remove regular files
                unlink(itemPath);
            }
        }
    }

    closedir(dir);

    // Remove the directory itself
    if (rmdir(dirName) != 0) {
        perror("rmdir");
        return 1; // Return an error code
    }

    return 0; // Return 0 to indicate success
}

int dir(const char *dirName, int removeIfExists) {
    // Check if the directory already exists
    struct stat st;
    if (stat(dirName, &st) == 0) {
        if (removeIfExists) {
            // Remove the existing directory
            if (removeDirectory(dirName) != 0) {
                return 1; // Return an error code
            }
            printf("Removed existing directory: %s\n", dirName);
        } else {
            fprintf(stderr, "Directory already exists: %s\n", dirName);
            return 1; // Return an error code
        }
    }

    // Create the directory
    if (mkdir(dirName, 0777) != 0) {
        perror("mkdir");
        return 1; // Return an error code
    }

    // Change the working directory to the new directory
    if (chdir(dirName) != 0) {
        perror("chdir");
        return 1; // Return an error code
    }

    return 0; // Return 0 to indicate success
}

void createDirectory(const char *dirName, int removeIfExists, int verbose) {
    int result = dir(dirName, removeIfExists);

    if (result == 0) {
        pid_t childPid = fork();

        if (childPid == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        } else if (childPid == 0) {
            // Child process: Remove directory if needed
            if (removeIfExists) {
                removeDirectory(dirName);
            }
            execlp("/bin/true", "true", NULL);
        } else {
            // Parent process: Print verbose messages
            if (verbose) {
                printf("Starting the process\n");
                sleep(1);
                printf("Creating a new directory: %s\n", dirName);
                sleep(1);
                printf("The Directory has been created: %s\n", dirName);
                sleep(1);
                printf("The path to the directory is navigated into: %s\n", dirName);
                sleep(1);
                printf("Successfully completed the task\n");
            }
            wait(NULL); // Wait for the child process to finish
        }
    } else {
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[]) {
    int removeIfExists = 0;
    int verbose = 0;
    const char *dirName = NULL;

    // Parse command line arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-r") == 0) {
            removeIfExists = 1;
        } else if (strcmp(argv[i], "-v") == 0) {
            verbose = 1;
        } else {
            dirName = argv[i];
        }
    }

    if (dirName == NULL) {
        fprintf(stderr, "Usage: %s [-r] [-v] dir_name\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    createDirectory(dirName, removeIfExists, verbose);

    return 0;
     // The code ends
}