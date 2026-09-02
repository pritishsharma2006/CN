#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 8080

int main() {
    int client_fd;
    struct sockaddr_in serv_addr;

    // Create socket
    client_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (client_fd < 0) {
        perror("Socket creation failed");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IP address from text to binary
    if (inet_pton(AF_INET, "127.0.0.1",
                  &serv_addr.sin_addr) <= 0) {
        printf("Invalid address / Address not supported\n");
        return -1;
    }

    // Connect to server
    if (connect(client_fd,
                (struct sockaddr *)&serv_addr,
                sizeof(serv_addr)) < 0) {
        printf("Connection Failed\n");
        return -1;
    }

    // Send message to server
    char *hello = "Hello, world!";

    send(client_fd, hello, strlen(hello), 0);

    printf("Hello message sent\n");

    // Close connection
    close(client_fd);

    return 0;
}