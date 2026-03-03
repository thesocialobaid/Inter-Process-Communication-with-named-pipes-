#include <stdio.h> 
#include<fcntl.h>
#include <string.h> 
#include <unistd.h> 

int main(){ 
    int fd; 
    char * message = "I want to become a Product Manager at Microsoft"; 
    fd = open("myfifo", O_WRONLY); 
    write(fd, message, strlen(message)+1); 
    close (fd); 

    return 0; 
}
