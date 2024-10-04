#include <windows.h>
#include <stdio.h>
#include <ctype.h>

#define BUFSIZE 4096

void Log(const char* message)
{
    FILE *logFile = fopen("log_child.txt", "a");
    if (logFile != NULL)
    {
        fprintf(logFile, "%s\n", message);
        fclose(logFile);
    }
}

BOOL ReadFromHandle(HANDLE hHandle, char* buffer, DWORD bufferSize, DWORD* bytesRead)
{
    if (!ReadFile(hHandle, buffer, bufferSize, bytesRead, NULL))
    {
        char errorMsg[256];
        snprintf(errorMsg, sizeof(errorMsg), "ReadFile failed, error %d", GetLastError());
        Log(errorMsg);
        return FALSE;
    }
    return TRUE;
}

BOOL WriteToHandle(HANDLE hHandle, const char* buffer, DWORD bufferSize, DWORD* bytesWritten)
{
    if (!WriteFile(hHandle, buffer, bufferSize, bytesWritten, NULL))
    {
        char errorMsg[256];
        snprintf(errorMsg, sizeof(errorMsg), "WriteFile failed, error %d", GetLastError());
        Log(errorMsg);
        return FALSE;
    }
    return TRUE;
}

BOOL Update(HANDLE hStdIn, HANDLE hStdOut, FILE* file, DWORD* dwRead, DWORD* dwWritten, char* buffer)
{
    if (!ReadFromHandle(hStdIn, buffer, BUFSIZE, dwRead))
    {
        const char* response = "Failed to read from input handle.\n";
        WriteToHandle(hStdOut, response, strlen(response) + 1, dwWritten);
        return FALSE;
    }

    if (strcmp(buffer, "exit\n") == 0)
    {
        return FALSE;
    }

    if (isupper(buffer[0]))
    {
        if (fprintf(file, "%s", buffer) < 0)
        {
            const char* response = "fprintf failed.\n";
            WriteToHandle(hStdOut, response, strlen(response) + 1, dwWritten);
            return FALSE;
        }

        const char* response = "Accepted";
        WriteToHandle(hStdOut, response, strlen(response) + 1, dwWritten);
    }
    else
    {
        const char* response = "Denied";
        WriteToHandle(hStdOut, response, strlen(response) + 1, dwWritten);
    }

    return TRUE;
}

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        Log("No file argument to write");
        return 1;
    }

    if (strcmp(argv[1], "exit") == 0)
    {
        return 0;
    }

    HANDLE hStdIn = GetStdHandle(STD_INPUT_HANDLE);
    if (hStdIn == INVALID_HANDLE_VALUE)
    {
        char errorMsg[256];
        snprintf(errorMsg, sizeof(errorMsg), "GetStdHandle (STD_INPUT_HANDLE) failed, error %d", GetLastError());
        Log(errorMsg);
        return 1;
    }

    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hStdOut == INVALID_HANDLE_VALUE)
    {
        char errorMsg[256];
        snprintf(errorMsg, sizeof(errorMsg), "GetStdHandle (STD_OUTPUT_HANDLE) failed, error %d", GetLastError());
        Log(errorMsg);
        return 1;
    }

    char buffer[BUFSIZE];
    DWORD dwRead, dwWritten;

    FILE *file = fopen(argv[1], "a");
    if (!file)
    {
        const char* response = "Failed to open file for writing.\n";
        WriteToHandle(hStdOut, response, strlen(response) + 1, &dwWritten);
        return 1;
    }

    while (Update(hStdIn, hStdOut, file, &dwRead, &dwWritten, buffer)) {}

    if (fclose(file) != 0)
    {
        const char* response = "fclose failed.\n";
        WriteToHandle(hStdOut, response, strlen(response) + 1, &dwWritten);
    }

    return 0;
}