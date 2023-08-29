#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
typedef struct {
    const char *phrase;
    int adjustment;
    const char *unit;
    int isNumeric;
} TimePhrase;

TimePhrase timePhrases[] = {
    {"yesterday", -1, "days", 0},
    {"today", 0, "days", 0},
    {"tomorrow", 1, "days", 0},
    {"day_after_tomorrow", 2, "days", 0},
    {"hour_ago", -1, "hours", 1},
    {"minute_ago", -1, "minutes", 1},
    {"next_hour", 1, "hours", 0},
    {"next_minute", 1, "minutes", 0},
    {"next_second", 1, "seconds", 0},
    {"next_week", 7, "days", 0},
    {"next_month", 30, "days", 0},
    {"next_year", 365, "days", 0},
};

int findTimePhrase(const char *input) {
    char modifiedInput[50];
    strcpy(modifiedInput, input);

    for (int i = 0; modifiedInput[i]; i++) {
        if (modifiedInput[i] == ' ') {
            modifiedInput[i] = '_';
        }
    }

    for (int i = 0; i < sizeof(timePhrases) / sizeof(TimePhrase); i++) {
        if (strcmp(modifiedInput, timePhrases[i].phrase) == 0) {
            return i;
        }
    }
    return -1;
}

void displayDateTime(char *fileName, int displayTimeString, int displayRFCFormat, int numericAdjustment, int adjustment, const char *unit) {
    struct stat st;
    if (stat(fileName, &st) != 0) {
        perror("Error getting file information");
        exit(EXIT_FAILURE);
    }

    // Apply numeric adjustment if provided
    if (numericAdjustment != 0) {
        adjustment = numericAdjustment * adjustment;
    }
    

    // Apply adjustments based on time phrases
    if (strcmp(unit, "days") == 0) {
        st.st_mtime += adjustment * 86400; // Add days in seconds
    } else if (strcmp(unit, "hours") == 0) {
        st.st_mtime += adjustment * 3600; // Add hours in seconds
    } else if (strcmp(unit, "minutes") == 0) {
        st.st_mtime += adjustment * 60; // Add minutes in seconds
    } else if (strcmp(unit, "seconds") == 0) {
        st.st_mtime += adjustment; // Add seconds
    }

    

    if (displayTimeString) {
        printf("Last modified time: %s", ctime(&st.st_mtime));
    }
    if (!displayTimeString && !displayRFCFormat) {
        printf("Last modified time: %s", ctime(&st.st_mtime));
    }

    if (displayRFCFormat) {
        char rfcTime[40];
        struct tm *localTime = localtime(&st.st_mtime);
        strftime(rfcTime, sizeof(rfcTime), "%a, %d %b %Y %T %z", localTime);
        printf("RFC 5322 Format: %s\n", rfcTime);
    }
}

int main(int argc, char *argv[]) {
    int displayTimeString = 0;
    int displayRFCFormat = 0;
    int numericAdjustment = 0;
    int adjustment = 0;
    const char *unit = "days";

    struct stat st; 

    if (argc < 2) {
        fprintf(stderr, "Usage: %s [-d] [-R] phrase filename\n", argv[0]);
        return EXIT_FAILURE;
    }

    int i = 1;
    while (i < argc - 1) {
        if (strcmp(argv[i], "-d") == 0) {
            displayTimeString = 1;
        } else if (strcmp(argv[i], "-R") == 0) {
            displayRFCFormat = 1;
        } else {
            break;
        }
        i++;
    }

    if (i == argc) {
        fprintf(stderr, "Missing phrase and filename arguments\n");
        return EXIT_FAILURE;
    }

    int phraseIndex = findTimePhrase(argv[i]);
    if (phraseIndex != -1) {
        adjustment = timePhrases[phraseIndex].adjustment;
        unit = timePhrases[phraseIndex].unit;
        i++;
    }

    if (i == argc) {
        int pid = fork();
        
        if (pid < 0) {
            perror("Fork error");
            return EXIT_FAILURE;
        } else if (pid == 0) {  // Child process
            if (displayTimeString) {
                printf("Last modified time: %s", ctime(&st.st_mtime));
            }
            exit(0);
        } else {  // Parent process
            wait(NULL);
        }
        
        return 0;
    }

    int pid = fork();

    if (pid < 0) {
        perror("Fork error");
        return EXIT_FAILURE;
    } else if (pid == 0) {  // Child process
        displayDateTime(argv[i], displayTimeString, displayRFCFormat, numericAdjustment, adjustment, unit);
        execlp("/bin/true", "true", NULL);
        exit(0);
    } else {  // Parent process
        if (displayRFCFormat) {
            wait(NULL);  // Wait for the child process to finish
            
        }
    }

    return 0;
    // The code ends
}