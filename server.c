#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zmq.h>
#include <pthread.h>
#include <errno.h>

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
    if (!new_client) {
        fprintf(stderr, "Memory allocation failed.\n");
        return;
    }
    strncpy(new_client->identity, identity, 255);
    new_client->identity[255] = '\0';
    strncpy(new_client->username, username, 99);
    new_client->username[99] = '\0';
    new_client->next = clients;
    clients = new_client;
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

void save_message(const char *sender, const char *recipient, const char *content) {
    if (strcmp(sender, recipient) < 0) {
        char filename[256];
        snprintf(filename, sizeof(filename), "db/%s_%s.txt", sender, recipient);
        FILE *fp = fopen(filename, "a");
        if (!fp) {
            perror("Failed to open file for writing");
            return;
        }
        if (fprintf(fp, "%s: %s\n", sender, content) < 0)
            perror("Failed to write to file");
        fclose(fp);
    } else {
        char filename[256];
        snprintf(filename, sizeof(filename), "db/%s_%s.txt", recipient, sender);
        FILE *fp = fopen(filename, "a");
        if (!fp) {
            perror("Failed to open file for writing");
            return;
        }
        if (fprintf(fp, "%s: %s\n", sender, content) < 0)
            perror("Failed to write to file");
        fclose(fp);
    }
}

int main() {
    void *context = zmq_ctx_new();
    void *router = zmq_socket(context, ZMQ_ROUTER);
    if (!router) {
        fprintf(stderr, "Failed to create router socket.\n");
        zmq_ctx_destroy(context);
        return -1;
    }
    if (zmq_bind(router, "tcp://*:5555") == -1) {
        fprintf(stderr, "Binding failed: %s\n", zmq_strerror(zmq_errno()));
        zmq_close(router);
        zmq_ctx_destroy(context);
        return -1;
    }

    printf("Server is running on port %s\n", PORT);

    while (1) {
        char identity[256];
        size_t identity_len = zmq_recv(router, identity, sizeof(identity)-1, 0);
        if (identity_len == -1) {
            fprintf(stderr, "Recv identity failed: %s\n", zmq_strerror(zmq_errno()));
            continue;
        }
        identity[identity_len] = '\0';

        char message[256];
        size_t message_len = zmq_recv(router, message, sizeof(message)-1, 0);
        if (message_len == -1) {
            fprintf(stderr, "Recv message failed: %s\n", zmq_strerror(zmq_errno()));
            continue;
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
                char error_msg[] = "Invalid message format\n";
                zmq_send(router, identity, strlen(identity), ZMQ_SNDMORE);
                zmq_send(router, error_msg, sizeof(error_msg)-1, 0);
                continue;
            }
            Client *recipient_client = find_client_by_username(recipient);
            if (recipient_client) {
                Client *sender_client = find_client_by_username(identity);
                if (sender_client) {
                    char sendbuf[256];
                    snprintf(sendbuf, sizeof(sendbuf), "RECEIVED:%s:%s", sender_client->username, content);
                    if (zmq_send(router, recipient_client->identity, strlen(recipient_client->identity), ZMQ_SNDMORE) == -1 ||
                        zmq_send(router, sendbuf, strlen(sendbuf), 0) == -1) {
                        fprintf(stderr, "Send message failed: %s\n", zmq_strerror(zmq_errno()));
                    }
                    save_message(sender_client->username, recipient, content);
                }
            } else {
                char error_msg[] = "Error: Recipient not found\n";
                zmq_send(router, identity, strlen(identity), ZMQ_SNDMORE);
                zmq_send(router, error_msg, sizeof(error_msg)-1, 0);
            }
        } else if (strncmp(message, "CHAT:", 5) == 0) {
            char *username = message + 5;
            Client *sender_client = find_client_by_username(identity);
            if (sender_client) {
                char filename[256];
                if (strcmp(sender_client->username, username) < 0) {
                    snprintf(filename, sizeof(filename), "db/%s_%s.txt", sender_client->username, username);
                } else {
                    snprintf(filename, sizeof(filename), "db/%s_%s.txt", username, sender_client->username);
                }
                FILE *fp = fopen(filename, "r");
                if (fp) {
                    char line[256];
                    while (fgets(line, sizeof(line), fp)) {
                        zmq_send(router, identity, strlen(identity), ZMQ_SNDMORE);
                        zmq_send(router, line, strlen(line), 0);
                    }
                    fclose(fp);
                } else {
                    char error_msg[] = "No history found.\n";
                    zmq_send(router, identity, strlen(identity), ZMQ_SNDMORE);
                    zmq_send(router, error_msg, sizeof(error_msg)-1, 0);
                }
            }
        } else if (strcmp(message, "QUIT") == 0) {
            remove_client(identity);
            printf("Client %s disconnected\n", identity);
        } else {
            char error_msg[] = "Invalid message format\n";
            zmq_send(router, identity, strlen(identity), ZMQ_SNDMORE);
            zmq_send(router, error_msg, sizeof(error_msg)-1, 0);
        }
    }

    zmq_close(router);
    zmq_ctx_destroy(context);
    return 0;
}