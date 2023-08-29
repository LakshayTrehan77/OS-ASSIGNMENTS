#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_WORD_LENGTH 100 // Change this to your desired maximum word length

int countWords(const char *filename, int ignoreNewLine)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        perror("Unable to open file");
        return -1;
    }

    int wordCount = 0;
    char word[MAX_WORD_LENGTH];

    while (fscanf(file, "%s", word) != EOF)
    {
        if (ignoreNewLine && strcmp(word, "\\n") == 0)
            continue;
        
        wordCount++;
    }

    fclose(file);
    return wordCount;
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Usage: %s [-n | -d] [file_name]\n", argv[0]);
        return 1;
    }

    pid_t child_pid = fork();

    if (child_pid == -1)
    {
        perror("Fork failed");
        return 1;
    }
    else if (child_pid == 0) // Child process
    {
        if (strcmp(argv[1], "-n") == 0)
        {
            if (argc != 3)
            {
                printf("Usage: %s -n [file_name]\n", argv[0]);
                return 1;
            }

            int wordCount = countWords(argv[2], 1);
            if (wordCount != -1)
                printf("Word Count Without New Line Character: %d\n", wordCount);
        }
        else if (strcmp(argv[1], "-d") == 0)
        {
            if (argc != 4)
            {
                printf("Usage: %s -d [file1_name] [file2_name]\n", argv[0]);
                return 1;
            }

            int wordCount1 = countWords(argv[2], 0);
            int wordCount2 = countWords(argv[3], 0);

            if (wordCount1 != -1 && wordCount2 != -1)
                printf("Difference in Word Counts: %d\n", wordCount1 - wordCount2);
        }
        else
        {
            int wordCount = countWords(argv[1], 0);
            if (wordCount != -1)
                printf("Word Count: %d\n", wordCount);
        }

        return 0;
    }
    else // Parent process
    {
        int status;
        waitpid(child_pid, &status, 0);
    }

    return 0;

    // The code ends
}