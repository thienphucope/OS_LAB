#include <stdio.h>   // For input/output functions like printf
#include <unistd.h>  // For pipe, fork, read, and write functions

int main() {
    int pipe_fds[2];    // Array to hold the file descriptors for the pipe
    int return_status;  // To store the return status of pipe function
    int pid;            // To store process ID
    char write_message[20] = "Hello";   // Message to write to the pipe
    char read_message[20];              // Buffer to store the message read from the pipe

    // Creating a pipe, returns -1 if there's an error
    return_status = pipe(pipe_fds);   
    if (return_status == -1) {
        printf("Unable to create pipe\n"); // Error message
        return 1;                          // Exit if pipe creation fails
    }

    pid = fork();  // Fork a new process
    if (pid == 0) {
        // Child process: Reads from the pipe
        read(pipe_fds[0], read_message, sizeof(read_message));  // Read from pipe
        printf("Child Process: Reading, message is '%s'\n", read_message);  // Print message
        return 0;   // Child process exits
    } else {
        // Parent process: Writes to the pipe
        printf("Parent Process: Writing, message is '%s'\n", write_message);  // Print what it's writing
        write(pipe_fds[1], write_message, sizeof(write_message));  // Write to pipe
        return 0;   // Parent process exits
    }
}
