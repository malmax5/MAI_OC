#include <zmq.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

// Function to find positions of pattern in text
char* find_positions(const char* text, const char* pattern) {
    int text_len = strlen(text);
    int pattern_len = strlen(pattern);
    if (pattern_len == 0 || text_len == 0 || pattern_len > text_len) {
        return strdup("-1");
    }

    int positions[108];
    int count = 0;
    for (int i = 0; i <= text_len - pattern_len; i++) {
        if (strncmp(text + i, pattern, pattern_len) == 0) {
            positions[count++] = i;
        }
    }

    if (count == 0) {
        return strdup("-1");
    }

    char* result = malloc(256);
    result[0] = '\0';
    for (int i = 0; i < count; i++) {
        if (i > 0) {
            strcat(result, ";");
        }
        char pos[10];
        sprintf(pos, "%d", positions[i]);
        strcat(result, pos);
    }
    return result;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: computing_node <id>\n");
        return 1;
    }
    int id = atoi(argv[1]);

    void *context = zmq_ctx_new();
    void *dealer = zmq_socket(context, ZMQ_DEALER);

    char identity[20];
    snprintf(identity, sizeof(identity)-1, "%d", id);
    zmq_setsockopt(dealer, ZMQ_IDENTITY, identity, strlen(identity));
    zmq_connect(dealer, "tcp://localhost:5555");

    char registration[256];
    snprintf(registration, sizeof(registration)-1, "register %d", id);
    zmq_send(dealer, registration, strlen(registration), 0);

    while (1) {
        char message[256];
        size_t message_len = zmq_recv(dealer, message, sizeof(message)-1, 0);
        message[message_len] = '\0';

        if (strncmp(message, "ping", 4) == 0) {
            zmq_send(dealer, "pong", 4, 0);
        } else if (strncmp(message, "exec", 4) == 0) {
            // Receive text_string and pattern_string
            char text[256];
            size_t text_len = zmq_recv(dealer, text, sizeof(text)-1, 0);
            text[text_len] = '\0';

            char pattern[256];
            size_t pattern_len = zmq_recv(dealer, pattern, sizeof(pattern)-1, 0);
            pattern[pattern_len] = '\0';

            // Perform substring search
            char* result = find_positions(text, pattern);

            // Send back the result
            zmq_send(dealer, "", 0, ZMQ_SNDMORE); // Empty frame
            zmq_send(dealer, result, strlen(result), 0);

            free(result);
        } else {
            // Handle other commands
            zmq_send(dealer, message, message_len, 0);
        }
    }

    zmq_close(dealer);
    zmq_ctx_destroy(context);
    return 0;
}