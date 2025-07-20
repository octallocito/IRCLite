#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

int clients[MAX_CLIENTS];
int client_count = 0;
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

void *handle_client(void *arg) {
    int client_sock = *(int *)arg;
    char buffer[BUFFER_SIZE];
    ssize_t read_size;

    while ((read_size = recv(client_sock, buffer, BUFFER_SIZE - 1, 0)) > 0) {
        buffer[read_size] = '\0';
        
        printf("Received: %s", buffer);
        
        pthread_mutex_lock(&clients_mutex);
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (clients[i] != 0 && clients[i] != client_sock) {
                send(clients[i], buffer, strlen(buffer), 0);
            }
        }
        pthread_mutex_unlock(&clients_mutex);
    }

    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i] == client_sock) {
            clients[i] = 0;
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);
    
    printf("Client disconnected\n");
    close(client_sock);
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int port = atoi(argv[1]);
    int server_fd, client_sock, *new_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    pthread_t thread_id;

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 5) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", port);

    while ((client_sock = accept(server_fd, (struct sockaddr *)&client_addr, &client_len))) {
        pthread_mutex_lock(&clients_mutex);
        
        if (client_count >= MAX_CLIENTS) {
            printf("Max clients reached. Connection rejected.\n");
            close(client_sock);
            pthread_mutex_unlock(&clients_mutex);
            continue;
        }

        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (clients[i] == 0) {
                clients[i] = client_sock;
                client_count++;
                break;
            }
        }
        pthread_mutex_unlock(&clients_mutex);

        printf("New client connected\n");

        new_sock = malloc(1);
        *new_sock = client_sock;

        if (pthread_create(&thread_id, NULL, handle_client, (void *)new_sock) < 0) {
            perror("Could not create thread");
            exit(EXIT_FAILURE);
        }

        pthread_detach(thread_id);
    }

    close(server_fd);
    return 0;
}

