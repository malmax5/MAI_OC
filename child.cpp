#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <ctype.h>
#include <signal.h>
#include <cerrno>

void handle_sigusr1(int sig)
{}

void log_error(const char *message) {
    FILE *log_file = fopen("log_child.txt", "a");
    if (log_file == NULL) {
        fprintf(stderr, "Ошибка открытия файла log_child.txt: %s\n", strerror(errno));
        return;
    }
    fprintf(log_file, "%s: %s\n", message, strerror(errno));
    fclose(log_file);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Использование: %s <имя_файла>\n", argv[0]);
        return 1;
    }

    struct sigaction sa;
    sa.sa_handler = handle_sigusr1;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGUSR1, &sa, NULL) == -1) {
        log_error("sigaction");
        exit(EXIT_FAILURE);
    }

    pid_t parentPID = getppid();

    sigset_t newmask, oldmask, zeromask;
    sigemptyset(&newmask);
    sigaddset(&newmask, SIGUSR1);
    sigemptyset(&zeromask);

    if (sigprocmask(SIG_BLOCK, &newmask, &oldmask) < 0) {
        log_error("sigprocmask");
        exit(EXIT_FAILURE);
    }

    char *filename = argv[1];
    int file = open(filename, O_WRONLY | O_APPEND);
    if (file == -1) {
        log_error("open");
        exit(EXIT_FAILURE);
    }

    char str[1024];
    char letter;
    int i = 0;
    int errorCode;

    while (1) {
        sigsuspend(&zeromask);

        while (read(STDIN_FILENO, &letter, sizeof(letter))) {
            str[i++] = letter;
            if (letter == '\0') break;
        }

        if (strcmp(str, "quit") == 0) {
            kill(parentPID, SIGUSR2);
            break;
        }

        if (isupper(str[0])) {
            write(file, str, i);
            write(file, "\n", sizeof(char));
            errorCode = 0;
        } else {
            errorCode = 1;
        }

        if (write(STDERR_FILENO, &errorCode, sizeof(errorCode)) == -1) {
            log_error("write");
            exit(EXIT_FAILURE);
        }
        kill(parentPID, SIGUSR2);

        i = 0;
    }

    if (sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0) {
        log_error("sigprocmask");
        return 1;
    }

    close(file);

    exit(EXIT_SUCCESS);
}