#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

void handle_sigusr2(int sig)
{}

void handle_child_ready(int sig)
{}

void handle_sigpipe(int sig)
{
    perror("SIGPIPE, bad in child\n");
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Использование: %s <имя_файла>\n", argv[0]);
        return 1;
    }

    struct sigaction sa;
    sa.sa_handler = handle_sigusr2;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGUSR2, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    sa.sa_handler = handle_child_ready;
    if (sigaction(SIGUSR1, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    sa.sa_handler = handle_sigpipe;
    if (sigaction(SIGPIPE, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    sigset_t newmask, oldmask, zeromask;
    sigemptyset(&newmask);
    sigaddset(&newmask, SIGUSR1);
    sigemptyset(&zeromask);

    if (sigprocmask(SIG_BLOCK, &newmask, &oldmask) < 0) {
        perror("sigprocmask");
        exit(EXIT_FAILURE);
    }

    char *filename = argv[1];
    int file = open(filename, O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (file == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    int pipe1[2];
    int pipe2[2];

    if (pipe(pipe1) == -1 || pipe(pipe2) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        close(pipe1[1]);
        close(pipe2[0]);

        if (dup2(pipe1[0], STDIN_FILENO) == -1) {
            perror("dup2");
            exit(EXIT_FAILURE);
        }

        if (dup2(pipe2[1], STDERR_FILENO) == -1) {
            perror("dup2");
            exit(EXIT_FAILURE);
        }

        close(pipe1[0]);
        close(pipe2[1]);
        close(file);

        kill(getppid(), SIGUSR1);

        char *args[] = {"./child", filename, NULL};
        if (execv(args[0], args) == -1) {
            perror("execv error");
            return 1;
        }
    } else {
        close(pipe1[0]);
        close(pipe2[1]);
        close(file);

        sigsuspend(&zeromask);

        char buffer[1024];

        while (1) {
            printf("Введите строку (для завершения введите 'quit'): ");
            if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
                perror("fgets");
                exit(EXIT_FAILURE);
            }

            buffer[strcspn(buffer, "\n")] = '\0'; 


            if (write(pipe1[1], buffer, strlen(buffer) + 1) == -1) {
                perror("write");
                exit(EXIT_FAILURE);
            }

            kill(pid, SIGUSR1);

            if (strcmp(buffer, "quit") == 0) 
            {
                break;
            }

            sigsuspend(&zeromask);

            int errorCode;

            if (read(pipe2[0], &errorCode, sizeof(errorCode)) == -1) {
                perror("read");
                exit(EXIT_FAILURE);
            }

            if (errorCode == 1) {
                printf("Строка не начинается с заглавной буквы: %s\n", buffer);
            } else {
                printf("Строка принята: %s\n", buffer);
            }
        }

        if (sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0) {
            perror("sigprocmask");
            return 1;
        }

        close(pipe1[1]);
        close(pipe2[0]);

        wait(NULL);
        exit(EXIT_SUCCESS);
    }
}