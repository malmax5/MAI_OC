#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zmq.h>
#include <pthread.h>

#define PORT "5555"

typedef struct Client {
    char identity[256];
    char username[100];
    struct Client *next;
} Client;

Client *clients = NULL;
pthread_mutex_t client_list_mutex = PTHREAD_MUTEX_INITIALIZER;

void add_client(const char *identity, const char *username) {
    Client *new_client = (Client *)malloc(sizeof(Client));
    if (new_client) {
        strncpy(new_client->identity, identity, 255);
        new_client->identity[255] = '\0';
        strncpy(new_client->username, username, 99);
        new_client->username[99] = '\0';
        new_client->next = clients;
        clients = new_client;
    }
}

void remove_client(const char *identity) {
    pthread_mutex_lock(&client_list_mutex);
    Client *current = clients;
    Client *prev = NULL;
    while (current) {
        if (strcmp(current->identity, identity) == 0) {
            if (prev)
                prev->next = current->next;
            else
                clients = current->next;
            free(current);
            break;
        }
        prev = current;
        current = current->next;
    }
    pthread_mutex_unlock(&client_list_mutex);
}

Client *find_client_by_username(const char *username) {
    pthread_mutex_lock(&client_list_mutex);
    Client *current = clients;
    while (current) {
        if (strcmp(current->username, username) == 0)
            break;
        current = current->next;
    }
    pthread_mutex_unlock(&client_list_mutex);
    return current;
}

int main() {
    void *context = zmq_ctx_new();
    void *router = zmq_socket(context, ZMQ_ROUTER);
    zmq_bind(router, "tcp://*:5555");

    printf("Server is running on port %s\n", PORT);

    while (1) {
        char identity[256];
        size_t identity_len = zmq_recv(router, identity, sizeof(identity)-1, 0);
        if (identity_len == -1) {
            continue; // Handle error
        }
        identity[identity_len] = '\0';

        char message[256];
        size_t message_len = zmq_recv(router, message, sizeof(message)-1, 0);
        if (message_len == -1) {
            continue; // Handle error
        }
        message[message_len] = '\0';

        if (strncmp(message, "LOGIN:", 6) == 0) {
            char *username = message + 6;
            add_client(identity, username);
            printf("Client %s connected with identity %s\n", username, identity);
        } else if (strncmp(message, "SEND:", 5) == 0) {
            char *msg = message + 5;
            char *recipient = strtok(msg, ":");
            char *content = strtok(NULL, "\n");
            if (!recipient || !content) {
                // Invalid message format
                continue;
            }
            Client *recipient_client = find_client_by_username(recipient);
            if (recipient_client) {
                printf("Find %s", recipient_client->username);
                printf("\n");
                // Get sender's username
                Client *sender_client = find_client_by_username(identity);
                if (sender_client) {
                    char sendbuf[256];
                    snprintf(sendbuf, sizeof(sendbuf), "RECEIVED:%s:%s", sender_client->username, content);
                    zmq_send(router, recipient_client->identity, strlen(recipient_client->identity), ZMQ_SNDMORE);
                    zmq_send(router, sendbuf, strlen(sendbuf), 0);
                }
            } else {
                // Recipient not found
                char error_msg[] = "Error: Recipient not found\n";
                zmq_send(router, identity, strlen(identity), ZMQ_SNDMORE);
                zmq_send(router, error_msg, sizeof(error_msg)-1, 0);
            }
        } else {
            // Invalid message
            char error_msg[] = "Invalid message format\n";
            zmq_send(router, identity, strlen(identity), ZMQ_SNDMORE);
            zmq_send(router, error_msg, sizeof(error_msg)-1, 0);
        }
    }

    zmq_close(router);
    zmq_ctx_destroy(context);
    return 0;
}