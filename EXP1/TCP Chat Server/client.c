#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int client_fd;
    struct sockaddr_in server_addr;

    char buffer[BUFFER_SIZE];

    // Create socket
    client_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (client_fd < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Configure server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1",
                  &server_addr.sin_addr) <= 0) {
        perror("Invalid address");
        exit(EXIT_FAILURE);
    }

    // Connect to server
    if (connect(client_fd,
                (struct sockaddr *)&server_addr,
                sizeof(server_addr)) < 0) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    printf("Connected to server.\n");

    // Chat loop
    while (1) {

        // Get client message
        printf("Client: ");
        fgets(buffer, BUFFER_SIZE, stdin);

        // Send message
        send(client_fd, buffer, strlen(buffer), 0);

        if (strncmp(buffer, "exit", 4) == 0) {
            break;
        }

        // Receive server message
        memset(buffer, 0, BUFFER_SIZE);

        int bytes_read = read(client_fd, buffer, BUFFER_SIZE - 1);

        if (bytes_read <= 0) {
            printf("Server disconnected.\n");
            break;
        }

        printf("Server: %s", buffer);

        if (strncmp(buffer, "exit", 4) == 0) {
            break;
        }
    }

    close(client_fd);

    return 0;
}