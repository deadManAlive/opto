#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <libgen.h>

#include "fs.h"
#include "http.h"

#define PORT 3110
#define MAX_REQUEST_SIZE 2048

int main(int argc, char** argv) {
    // checking work dir/exec dir
    char exec_path[PATH_MAX];
    char* exe_dir;
    ssize_t len = readlink("/proc/self/exe", exec_path, sizeof(exec_path) - 1);
    if (len != -1) {
        exec_path[len] = '\0';
        exe_dir = dirname(exec_path);
        printf("exec. dir.: [%s]\n", exe_dir);
    }
    else {
        perror("error in reading executable directory");
    }

    // initiate socket vars.
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
    response_t response = generate_response(OK, "Content-Type: text/html", &content);
    
    // Accept incoming connections
    char response_path[PATH_MAX];
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

        string_t request_path = get_path_from_request(buffer);
        // snprintf(response_path, PATH_MAX, "%s/routes%s", exe_dir, request_path.buffer);
        if (verify_path(response_path, exe_dir, request_path.buffer)) {
            printf("> valid path [%s]\n", response_path);
            content = read_file_contents(response_path);
            response = generate_response(OK, NULL, &content);
        }
        else {
            printf("> invalid path [%s]\n", response_path);
            response = generate_response(NOT_FOUND, NULL, NULL);
        }

        printf("Response: \n%s\n", response.buffer);
        
        // Send the response to the client
        if (write(new_socket, response.buffer, response.length) < 0) {
            perror("write failed");
            exit(EXIT_FAILURE);
        }
        
        // Clean up
        free(request_path.buffer);
        memset(response_path, 0, PATH_MAX);

        memset(buffer, 0, MAX_REQUEST_SIZE);
        close(new_socket);
    }
    
    return 0;
}
