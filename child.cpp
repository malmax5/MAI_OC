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
#include <sys/mman.h>

#define SHARED_MEMORY_SIZE 1024

static volatile sig_atomic_t sigflag;
static sigset_t newmask, oldmask, zeromask;

void handle_sigusr1(int sig) {
    sigflag = 1;
    printf("Дочерний процесс получил SIGUSR1\n");
}

void WAIT_PARENT(void)
{
    while (sigflag == 0)
        sigsuspend(&zeromask);
    sigflag = 0;
}

void TELL_PARENT(pid_t pid)
{
    kill(pid, SIGUSR2);
}

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

    int fd = open("/tmp/shared_memory_file", O_RDWR, 0666);
    if (fd == -1) {
        log_error("open");
        exit(EXIT_FAILURE);
    }

    void *shared_mem_child = mmap(NULL, SHARED_MEMORY_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shared_mem_child == MAP_FAILED) {
        log_error("mmap");
        exit(EXIT_FAILURE);
    }

    close(fd);

    // Открытие файла для записи
    int file = open(argv[1], O_WRONLY | O_APPEND | O_CREAT, 0666);  // Открываем файл для записи
    if (file == -1) {
        log_error("open");
        exit(EXIT_FAILURE);
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

    sigemptyset(&newmask);
    sigaddset(&newmask, SIGUSR1);
    sigemptyset(&zeromask);

    if (sigprocmask(SIG_BLOCK, &newmask, &oldmask) < 0) {
        log_error("sigprocmask");
        exit(EXIT_FAILURE);
    }

    char str[SHARED_MEMORY_SIZE] = {0};  // Инициализация нулями
    char letter;
    int i = 0;
    int errorCode;

    while (1) {
        WAIT_PARENT();

        strncpy(str, (char *)shared_mem_child, SHARED_MEMORY_SIZE);

        if (strcmp(str, "quit") == 0) {
            TELL_PARENT(parentPID);
            break;
        }

        if (isupper(str[0])) {
            write(file, str, strlen(str));
            write(file, "\n", 1);
            errorCode = 0;
        } else {
            errorCode = 1;
        }

        *((int *)((char *)shared_mem_child + SHARED_MEMORY_SIZE - sizeof(int))) = errorCode;

        TELL_PARENT(parentPID);

        i = 0;
    }

    if (sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0) {
        log_error("sigprocmask");
        return 1;
    }

    close(file);  // Закрытие файла

    exit(EXIT_SUCCESS);
}