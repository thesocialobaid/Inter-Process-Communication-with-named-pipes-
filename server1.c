// server.c - Student Record Lookup System
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>

#define FIFO_REQ "fifo_req"
#define FIFO_RES "fifo_res"

// Student record structure
typedef struct {
    int roll_no;
    char name[50];
    int marks;
} Student;

// Predefined student database
Student db[] = {
    {101, "Ali",   85},
    {102, "Sara",  90},
    {103, "Ahmed", 78}
};
int db_size = 3;

int main() {
    int fd_read, fd_write;
    char request[20];
    char response[100];
    int roll;

    // Create both FIFOs
    mkfifo(FIFO_REQ, 0666);
    mkfifo(FIFO_RES, 0666);

    printf("=== Student Record Server Running ===\n");
    printf("Database loaded with %d records.\n\n", db_size);

    while (1) {
        // Open request FIFO for reading (blocks until client connects)
        fd_read = open(FIFO_REQ, O_RDONLY);
        if (fd_read < 0) {
            perror("Server: failed to open fifo_req");
            exit(1);
        }

        int bytes = read(fd_read, request, sizeof(request));
        close(fd_read);

        if (bytes <= 0) continue;

        // Parse roll number from request string
        roll = atoi(request);

        printf("--------------------------------------------------\n");
        printf("Server received roll number: %d\n", roll);

        // Open response FIFO for writing
        fd_write = open(FIFO_RES, O_WRONLY);
        if (fd_write < 0) {
            perror("Server: failed to open fifo_res");
            exit(1);
        }

        // Search the database
        int found = 0;
        for (int i = 0; i < db_size; i++) {
            if (db[i].roll_no == roll) {
                snprintf(response, sizeof(response),
                    "Found! Roll No: %d | Name: %s | Marks: %d",
                    db[i].roll_no, db[i].name, db[i].marks);
                found = 1;
                break;
            }
        }

        if (!found) {
            snprintf(response, sizeof(response),
                "Error: No student found with Roll No %d", roll);
        }

        write(fd_write, response, strlen(response) + 1);
        close(fd_write);

        printf("Server responded  : %s\n\n", response);
    }

    return 0;
}