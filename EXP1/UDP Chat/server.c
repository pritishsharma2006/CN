#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int server_fd;
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;

    char buffer[BUFFER_SIZE];

    socklen_t client_len = sizeof(client_addr);

    // Create UDP socket
    server_fd = socket(AF_INET, SOCK_DGRAM, 0);

    if (server_fd < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Configure server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind socket
    if (bind(server_fd,
             (struct sockaddr *)&server_addr,
             sizeof(server_addr)) < 0) {
        perror("bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("UDP server listening on port %d...\n", PORT);

    // Receive data
    int bytes_received = recvfrom(
        server_fd,
        buffer,
        BUFFER_SIZE - 1,
        0,
        (struct sockaddr *)&client_addr,
        &client_len
    );

    if (bytes_received < 0) {
        perror("recvfrom failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    buffer[bytes_received] = '\0';

    // Print received message
    printf("Received: %s\n", buffer);

    // Close socket
    close(server_fd);

    return 0;
}