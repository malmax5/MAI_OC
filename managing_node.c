#include <zmq.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

typedef struct {
    char identity[256];
    size_t identity_len;
    int id;
} Node;

Node nodes[100];
int node_count = 0;

void add_node(const char *identity, size_t identity_len, int id) {
    strcpy(nodes[node_count].identity, identity);
    nodes[node_count].identity_len = identity_len;
    nodes[node_count].id = id;
    node_count++;
}

Node* find_node_by_id(int id) {
    for (int i = 0; i < node_count; i++) {
        if (nodes[i].id == id) {
            return &nodes[i];
        }
    }
    return NULL;
}

void process_command(const char *command, void *router, int stdin_fd) {
    if (strncmp(command, "create ", 7) == 0) {
        int id;
        if (sscanf(command, "create %d -1", &id) != 1) {
            printf("Invalid command format.\n");
            return;
        }
        pid_t pid = fork();
        if (pid == 0) {
            char id_str[20];
            sprintf(id_str, "%d", id);
            execl("./computing_node", "computing_node", id_str, NULL);
            perror("execl");
            exit(1);
        } else if (pid > 0) {
            printf("Computing node %d created with PID %d\n", id, pid);
        } else {
            perror("fork");
        }
    } else if (strncmp(command, "ping ", 5) == 0) {
        int id;
        if (sscanf(command, "ping %d", &id) != 1) {
            printf("Invalid command format.\n");
            return;
        }
        Node *node = find_node_by_id(id);
        if (node == NULL) {
            printf("Error: Not found\n");
        } else {
            zmq_send(router, node->identity, node->identity_len, ZMQ_SNDMORE);
            zmq_send(router, "ping", 4, 0);

            zmq_pollitem_t items[] = {
                { router, 0, ZMQ_POLLIN, 0 }
            };
            int rc = zmq_poll(items, 1, 1000); // 1 second timeout
            if (rc == -1) {
                perror("zmq_poll");
                return;
            }
            if (rc == 1 && items[0].revents & ZMQ_POLLIN) {
                char identity[256];
                size_t identity_len = zmq_recv(router, identity, sizeof(identity)-1, 0);
                identity[identity_len] = '\0';
                char response[256];
                size_t response_len = zmq_recv(router, response, sizeof(response)-1, 0);
                response[response_len] = '\0';
                if (strcmp(response, "pong") == 0) {
                    printf("Ok: 1\n");
                } else {
                    printf("Ok: 0\n");
                }
            } else {
                printf("Ok: 0\n");
            }
        }
    } else if (strncmp(command, "exec ", 5) == 0) {
        int id;
        if (sscanf(command, "exec %d", &id) != 1) {
            printf("Invalid command format.\n");
            return;
        }
        Node *node = find_node_by_id(id);
        if (node == NULL) {
            printf("Error: Not found\n");
            return;
        }
    
        // Switch stdin to blocking mode
        int flags = fcntl(stdin_fd, F_GETFL, 0);
        fcntl(stdin_fd, F_SETFL, flags & ~O_NONBLOCK);
    
        char text[256];
        printf("Enter text_string: ");
        if (!fgets(text, sizeof(text), stdin)) {
            printf("Error reading text_string.\n");
            fcntl(stdin_fd, F_SETFL, flags);
            return;
        }
        text[strcspn(text, "\n")] = 0;
    
        char pattern[256];
        printf("Enter pattern_string: ");
        if (!fgets(pattern, sizeof(pattern), stdin)) {
            printf("Error reading pattern_string.\n");
            fcntl(stdin_fd, F_SETFL, flags);
            return;
        }
        pattern[strcspn(pattern, "\n")] = 0;
    
        // Switch stdin back to non-blocking mode
        fcntl(stdin_fd, F_SETFL, flags);
    
        // Send "exec" command to the node
        zmq_send(router, node->identity, node->identity_len, ZMQ_SNDMORE);
        zmq_send(router, "exec", 4, ZMQ_SNDMORE);
        zmq_send(router, text, strlen(text), ZMQ_SNDMORE);
        zmq_send(router, pattern, strlen(pattern), 0);
    
        // Receive response
        zmq_pollitem_t items[] = {
            { router, 0, ZMQ_POLLIN, 0 }
        };
        int rc = zmq_poll(items, 1, 5000); // 5 second timeout
        if (rc == -1) {
            perror("zmq_poll");
            return;
        }
        if (rc == 0) {
            printf("Timeout: No response from node %d\n", id);
            return;
        }
    
        // Receive empty frame
        char empty_frame[1];
        size_t empty_len = zmq_recv(router, empty_frame, sizeof(empty_frame)-1, 0);
    
        // Receive result
        char result[256];
        size_t result_len = zmq_recv(router, result, sizeof(result)-1, 0);
        result[result_len] = '\0';
    
        printf("Ok:%s\n", result);
    } else {
        printf("Unknown command.\n");
    }
}

int main() {
    void *context = zmq_ctx_new();
    void *router = zmq_socket(context, ZMQ_ROUTER);
    zmq_bind(router, "tcp://*:5555");

    int stdin_fd = fileno(stdin);
    int flags = fcntl(stdin_fd, F_GETFL, 0);
    fcntl(stdin_fd, F_SETFL, flags | O_NONBLOCK);

    char command_buffer[256];
    size_t buffer_pos = 0;
    size_t buffer_size = sizeof(command_buffer);

    zmq_pollitem_t items[] = {
        { router, 0, ZMQ_POLLIN, 0 },
        { stdin_fd, 0, ZMQ_POLLIN, 0 }
    };

    while (1) {
        int rc = zmq_poll(items, 2, -1);
        if (rc == -1) {
            perror("zmq_poll");
            continue;
        }

        if (items[0].revents & ZMQ_POLLIN) {
            char identity[256];
            size_t identity_len = zmq_recv(router, identity, sizeof(identity)-1, 0);
            identity[identity_len] = '\0';
            char message[256];
            size_t message_len = zmq_recv(router, message, sizeof(message)-1, 0);
            message[message_len] = '\0';

            if (strncmp(message, "register ", 9) == 0) {
                int id;
                if (sscanf(message, "register %d", &id) == 1) {
                    add_node(identity, identity_len, id);
                    printf("Registered node %d with identity %s\n", id, identity);
                }
            }
        }

        if (items[1].revents & ZMQ_POLLIN) {
            char c;
            ssize_t bytes_read;
            while ((bytes_read = read(stdin_fd, &c, 1)) > 0) {
                if (buffer_pos < buffer_size - 1) {
                    command_buffer[buffer_pos++] = c;
                }
                if (c == '\n') {
                    // Null-terminate the command
                    command_buffer[buffer_pos] = '\0';
                    buffer_pos = 0;
                    // Process the command
                    process_command(command_buffer, router, stdin_fd);
                }
            }
            if (bytes_read == -1 && errno != EAGAIN) {
                perror("read from stdin");
                break;
            }
        }
    }

    zmq_close(router);
    zmq_ctx_destroy(context);
    return 0;
}