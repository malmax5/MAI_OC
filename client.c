#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zmq.h>
#include <pthread.h>
#include <errno.h>

#define SERVER_ADDRESS "tcp://localhost:5555"

void *receive_messages(void *socket_ptr);

int main() {
    void *context = zmq_ctx_new();
    if (!context) {
        fprintf(stderr, "Failed to create ZMQ context.\n");
        return -1;
    }
    void *socket = zmq_socket(context, ZMQ_DEALER);
    if (!socket) {
        fprintf(stderr, "Failed to create dealer socket.\n");
        zmq_ctx_destroy(context);
        return -1;
    }

    char username[256];
    printf("Enter your username: ");
    if (!fgets(username, sizeof(username), stdin)) {
        fprintf(stderr, "Failed to read username.\n");
        zmq_close(socket);
        zmq_ctx_destroy(context);
        return -1;
    }
    username[strcspn(username, "\n")] = '\0';

    char login[512];
    snprintf(login, sizeof(login), "LOGIN:%s", username);

    if (zmq_setsockopt(socket, ZMQ_IDENTITY, username, strlen(username)) == -1) {
        fprintf(stderr, "Setting socket identity failed: %s\n", zmq_strerror(zmq_errno()));
    }
    if (zmq_connect(socket, SERVER_ADDRESS) == -1) {
        fprintf(stderr, "Connection failed: %s\n", zmq_strerror(zmq_errno()));
        zmq_close(socket);
        zmq_ctx_destroy(context);
        return -1;
    }
    if (zmq_send(socket, login, strlen(login), 0) == -1) {
        fprintf(stderr, "Send login failed: %s\n", zmq_strerror(zmq_errno()));
    }

    pthread_t receive_thread;
    if (pthread_create(&receive_thread, NULL, receive_messages, socket)) {
        perror("Failed to create receive thread");
        zmq_close(socket);
        zmq_ctx_destroy(context);
        return -1;
    }

    char message[256];
    while (fgets(message, sizeof(message), stdin) != NULL) {
        if (strncmp(message, "SEND:", 5) == 0) {
            message[strcspn(message, "\n")] = '\0';
            if (zmq_send(socket, message, strlen(message), 0) == -1) {
                fprintf(stderr, "Send message failed: %s\n", zmq_strerror(zmq_errno()));
            }
        } else if (strncmp(message, "CHAT:", 5) == 0) {
            message[strcspn(message, "\n")] = '\0';
            if (zmq_send(socket, message, strlen(message), 0) == -1) {
                fprintf(stderr, "Send chat request failed: %s\n", zmq_strerror(zmq_errno()));
            }
        } else if (strcmp(message, "QUIT.\n") == 0) {
            if (zmq_send(socket, "QUIT", 4, 0) == -1) {
                fprintf(stderr, "Send quit failed: %s\n", zmq_strerror(zmq_errno()));
            }
            break;
        } else {
            printf("Invalid command. Use SEND:recipient:message, CHAT:username, or QUIT.\n");
        }
    }

    zmq_close(socket);
    zmq_ctx_destroy(context);
    if (pthread_join(receive_thread, NULL)) {
        perror("Failed to join receive thread");
    }
    return 0;
}

void *receive_messages(void *socket_ptr) {
    void *socket = socket_ptr;
    char buffer[256];
    while (1) {
        size_t bytes = zmq_recv(socket, buffer, sizeof(buffer)-1, 0);
        if (bytes == -1) {
            fprintf(stderr, "Recv failed: %s\n", zmq_strerror(zmq_errno()));
            break;
        }
        buffer[bytes] = '\0';
        if (strncmp(buffer, "RECEIVED:", 9) == 0) {
            char *msg = buffer + 9;
            printf("Received message: %s\n", msg);
        } else if (strncmp(buffer, "No history found.", 17) == 0) {
            printf("No history found.\n");
        } else {
            printf("%s", buffer);
        }
    }
    return NULL;
}