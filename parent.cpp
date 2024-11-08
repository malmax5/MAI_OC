#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/mman.h>

#define SHARED_MEMORY_SIZE 1024

static volatile sig_atomic_t sigflag;
static sigset_t newmask, oldmask, zeromask;

void handle_sigusr2(int sig) {
    sigflag = 1;
    printf("Родительский процесс получил SIGUSR2\n");
}

void handle_child_ready(int sig) {
    sigflag = 1;
    printf("Родительский процесс получил SIGUSR1\n");
}

void handle_sigpipe(int sig) {
    perror("SIGPIPE, bad in child\n");
}

void WAIT_CHILD(void)
{
    while (sigflag == 0)
        sigsuspend(&zeromask);
    sigflag = 0;
}

void TELL_CHILD(pid_t pid)
{
    kill(pid, SIGUSR1);
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

    sigemptyset(&newmask);
    sigaddset(&newmask, SIGUSR1);
    sigemptyset(&zeromask);

    if (sigprocmask(SIG_BLOCK, &newmask, &oldmask) < 0) {
        perror("sigprocmask");
        exit(EXIT_FAILURE);
    }

    char *filename = argv[1];

    int fd = open("/tmp/shared_memory_file", O_RDWR | O_CREAT, 0666);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    if (ftruncate(fd, SHARED_MEMORY_SIZE) == -1) {
        perror("ftruncate");
        exit(EXIT_FAILURE);
    }

    void *shared_memory = mmap(NULL, SHARED_MEMORY_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shared_memory == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    close(fd);

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        kill(getppid(), SIGUSR1);

        char *args[] = {"./child", filename, NULL};
        if (execv(args[0], args) == -1) {
            perror("execv error");
            return 1;
        }
    } else {
        WAIT_CHILD();

        char buffer[1024];

        while (1) {
            printf("Введите строку (для завершения введите 'quit'): ");
            if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
                perror("fgets");
                exit(EXIT_FAILURE);
            }

            buffer[strcspn(buffer, "\n")] = '\0';

            snprintf((char *)shared_memory, SHARED_MEMORY_SIZE, "%s", buffer);

            TELL_CHILD(pid);

            if (strcmp(buffer, "quit") == 0) 
                break;

            WAIT_CHILD();

            int errorCode = *((int *)((char *)shared_memory + SHARED_MEMORY_SIZE - sizeof(int)));

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

        munmap(shared_memory, SHARED_MEMORY_SIZE);
        wait(NULL);
        exit(EXIT_SUCCESS);
    }
}