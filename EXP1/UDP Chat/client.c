#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 8080

int main() {
    int client_fd;
    struct sockaddr_in server_addr;

    // Create UDP socket
    client_fd = socket(AF_INET, SOCK_DGRAM, 0);

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
        close(client_fd);
        exit(EXIT_FAILURE);
    }

    // Message to send
    char *message = "Hello, world!";

    // Send UDP datagram
    sendto(
        client_fd,
        message,
        strlen(message),
        0,
        (struct sockaddr *)&server_addr,
        sizeof(server_addr)
    );

    printf("Hello message sent.\n");

    // Close socket
    close(client_fd);

    return 0;
}