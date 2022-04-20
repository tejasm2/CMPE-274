// Server side C/C++ program to demonstrate Socket programming
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <pwd.h>
#define PORT 8080

int main(int argc, char const *argv[])
{
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    char *hello = "Hello from server";
    char *user = "nobody";
    struct passwd *pwd;
    pid_t pid;

    // Show ASLR
    printf("execve=0x%p\n", execve);

    if(strcmp(argv[0],"socket")==0){
        int newSoc = atoi(argv[1]);
        valread = read(newSoc, buffer, 1024);
        printf("%s\n", buffer);
        send(newSoc, hello, strlen(hello),0);
        printf("Hello Message sent!!! \n");
        exit(0);
    }

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Attaching socket to port 80
    // if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
    //                &opt, sizeof(opt)))
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR , &opt, sizeof(opt)))

    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Forcefully attaching socket to the port 80
    if (bind(server_fd, (struct sockaddr *)&address,
             sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                             (socklen_t *)&addrlen)) < 0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    pid = fork();
    if (pid == 0)
    {
        if ((pwd = getpwnam(user)) == NULL)
        {
            perror("Cannot find UID");
        }
        setuid(pwd->pw_uid);
        valread = read(new_socket, buffer, 1024);
        printf("%s\n", buffer);
        send(new_socket, hello, strlen(hello), 0);
        printf("Hello message sent\n");

        // New Socket will be created
        int duplicateSocket = dup(new_socket);
        if(duplicateSocket==-1)
            perror("Duplicate failed");

        // convert socket to string
        char socketString[10];
        sprintf(socketString, "%d", duplicateSocket);
        char *args[] = {"socket", socketString, NULL};
        execvp(argv[0], args);
        exit(0);
    }
    wait(NULL);
    return 0;
}