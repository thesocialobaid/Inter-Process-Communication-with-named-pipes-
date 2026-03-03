//client 
// Takes operator and operates from user, sends to server, recieves and prints result 

#include<stdio.h> 
#include<stdlib.h> 
#include<fcntl.h>
#include <unistd.h>
#include <string.h> 

#define FIFO_CLIENT_TO_SERVER "fifo_request"
#define FIFO_SERVER_TO_CLIENT "fifo_response"

int main(){ 
    int fd_write, fd_read; 
    char op; 
    double operand1, operand2; 
    char msg[50];
    char response[100]; 

    printf("=== Calculator Client===\n"); 
    printf("Enter Expression");
    if(scanf("%c %lf %lf", &op, &operand1, &operand2)!=3){ 
        printf("Error: Invalid input format. Use operator num1 num2"); 
        exit(1); 
    }  

    if (op != '+' && op != '-' && op != '*' && op != '/') {
        printf("Error: Invalid operator '%c'. Use + - * /\n", op);
        exit(1);
    }


    printf("\nConnecting to server...\n"); 
    fd_write = open(FIFO_CLIENT_TO_SERVER,O_WRONLY); 
    if(fd_write <0){ 
        perror("Client: open request pipe filed. Is the server running?"); 
        exit(1); 
    }

    snprintf(msg, sizeof(msg), "%c %lf %lf", op, operand1, operand2);
    write(fd_write, msg, strlen(msg) + 1);
    close(fd_write);


    printf("Request sent     : %c %.2f %.2f\n", op, operand1, operand2);

    fd_read = open(FIFO_SERVER_TO_CLIENT, O_RDONLY); 
    if(fd_read<0){ 
        perror("Client: open response pipe failed"); 
        exit(1); 
    }

    read(fd_read,response,sizeof(response)); 
    close(fd_read); 

    printf("Server responded : %s\n", response);
    
    return 0; 
}