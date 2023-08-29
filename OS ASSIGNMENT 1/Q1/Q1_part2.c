#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int factorial(int n) {
    if (n <= 1)
        return 1;
    else
        return n * factorial(n - 1);
}

void fibonacci(int n) {
    int a = 0, b = 1, c;
    printf("Fibonacci Series: ");
    for (int i = 0; i < n; ++i) {
        printf("%d ", a);
        c = a + b;
        a = b;
        b = c;
    }
    printf("\n");
}

void executeParentChild(int ptr) {
    pid_t child_pid;

    child_pid = fork();  // Use vfork

    if (child_pid == -1) {
        perror("vfork");
        exit(EXIT_FAILURE);
    } else if (child_pid == 0) {
        // Child process
        int fact = factorial(4);
        printf("Child Process: Factorial of 4 is %d\n", fact);
        
        _exit(EXIT_SUCCESS);
    } else {
        // Parent process
       
        fibonacci(16);
        wait(NULL);  // Wait for the child process to finish
        
    }
}

int main() {
    // Call the function with ptr = 1 to execute child first, then parent
    executeParentChild(0);



    return 0;
     // The code ends
}
