#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int factorial(int n) {
    if (n == 0 || n == 1) {
        return 1;
    } else {
        return n * factorial(n - 1);
    }
}

int main() {
    pid_t child_pid;

   

    child_pid = fork();

    if (child_pid < 0) {
        perror("Fork failed");
        exit(1);
    } else if (child_pid == 0) { // Child process
        printf("Child is having ID %d\n", getpid());

        // Print the parent's ID in the child
        printf("Child: My Parent ID is %d\n", getppid());

        // Wait for parent to complete Fibonacci calculation
      

       

        _exit(0); // Terminate child process
    } else { // Parent process

        printf("Parent (P) is having ID %d\n", getpid());
        wait(NULL);
        printf("ID of P's Child is %d\n", child_pid);

        // Wait for child process to complete
       
    }

    

    return 0;
    // The code ends
}
