#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zmq.h>
#include <pthread.h>

#define SERVER_ADDRESS "tcp://localhost:5555"

// Function prototype for receive_messages
void *receive_messages(void *socket_ptr);

int main() {
    void *context = zmq_ctx_new();
    void *socket = zmq_socket(context, ZMQ_DEALER);

    // Register login
    char username[256];
    printf("Enter your username: ");
    fgets(username, sizeof(username), stdin);
    username[strcspn(username, "\n")] = '\0'; // Remove newline

    // Prepare the LOGIN message
    char login[512];
    snprintf(login, sizeof(login), "LOGIN:%s", username);

    zmq_setsockopt(socket, ZMQ_IDENTITY, username, strlen(username));
    zmq_connect(socket, SERVER_ADDRESS);
    zmq_send(socket, login, strlen(login), 0);

    // Start a thread to receive messages
    pthread_t receive_thread;
    pthread_create(&receive_thread, NULL, receive_messages, socket);

    // Send messages
    char message[256];
    while (fgets(message, sizeof(message), stdin) != NULL) {
        if (strncmp(message, "SEND:", 5) == 0) {
            message[strcspn(message, "\n")] = '\0';
            zmq_send(socket, message, strlen(message), 0);
        } else {
            printf("Invalid command. Use SEND:recipient:message\n");
        }
    }

    pthread_join(receive_thread, NULL);
    zmq_close(socket);
    zmq_ctx_destroy(context);
    return 0;
}

// Function to receive messages
void *receive_messages(void *socket_ptr) {
    void *socket = socket_ptr;
    char buffer[256];
    while (1) {
        size_t bytes = zmq_recv(socket, buffer, sizeof(buffer)-1, 0);
        if (bytes == -1) {
            break; // Handle error
        }
        buffer[bytes] = '\0';
        if (strncmp(buffer, "RECEIVED:", 9) == 0) {
            char *msg = buffer + 9;
            printf("Received message: %s\n", msg);
        } else {
            printf("Server response: %s\n", buffer);
        }
    }
    return NULL;
}