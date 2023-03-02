#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "fs.h"
#include "http.h"

#define PORT 3110
#define MAX_REQUEST_SIZE 2048

int main()
{
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[MAX_REQUEST_SIZE] = {0};

    
    // Create a socket for the server
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    
    // Set socket options
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt failed");
        exit(EXIT_FAILURE);
    }
    
    // Bind the socket to a port and address
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    
    // Start listening for incoming connections
    if (listen(server_fd, 3) < 0) {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }
    
    printf("Server is running on port %d...\n", PORT);

    string_t content = read_file_contents("routes/index.html");
    string_t response = generate_response(OK, "Content-Type: text/html", &content);
    
    // Accept incoming connections
    while (1) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("accept failed");
            exit(EXIT_FAILURE);
        }
        
        // Read the request from the client
        if (read(new_socket, buffer, MAX_REQUEST_SIZE) < 0) {
            perror("read failed");
            exit(EXIT_FAILURE);
        }
        
        // Send the response to the client
        if (write(new_socket, response.buffer, response.length) < 0) {
            perror("write failed");
            exit(EXIT_FAILURE);
        }
        
        // Clean up
        memset(buffer, 0, MAX_REQUEST_SIZE);
        close(new_socket);
    }
    
    return 0;
}
