#include "../includes/exceptions/exceptions.hpp"
#include <windows.h>
#include <stdio.h>
#include <mutex>
#include <string>

#define BUFSIZE 4096

class ProcessInfoSingle
{
public:
    static ProcessInfoSingle& GetInstance()
    {
        if (instance == nullptr)
        {
            std::lock_guard<std::mutex> lock(mutex);
            if (instance == nullptr)
                instance = new ProcessInfoSingle();
        }
        return *instance;
    }

    void Set_SECURITY_ATTRIBUTES(DWORD nLength, LPVOID lpSecurityDescriptor, BOOL bInheritHandle)
    {
        saAttr.nLength = nLength;
        saAttr.lpSecurityDescriptor = lpSecurityDescriptor;
        saAttr.bInheritHandle = bInheritHandle;
    }

    void Set_STARTUPINFO(DWORD cb, HANDLE hStdInput, HANDLE hStdOutput, DWORD dwFlags)
    {
        ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));
        siStartInfo.cb = cb;
        siStartInfo.hStdInput = hStdInput;
        siStartInfo.hStdOutput = hStdOutput;
        siStartInfo.dwFlags |= dwFlags;
    }

    void SetHandles(HANDLE hStdIn, HANDLE hStdOut)
    {
        hChildStdOutRead = hStdOut;
        hChildStdInWrite = hStdIn;
    }

    void CreateChildProcess(char commandLine[BUFSIZE])
    {
        BOOL bSuccess;
        bSuccess = CreateProcess(
            NULL,
            commandLine,
            NULL,
            NULL,
            TRUE,
            0,
            NULL,
            NULL,
            &siStartInfo,
            &piProcInfo
        );

        if (!bSuccess)
        {
            throw CreateException("Can't create a child proccess");
        }
    }

    static void CloseHandle(HANDLE handle1)
    {
        ::CloseHandle(handle1);
    }

    void CloseChild()
    {
        WaitForSingleObject(piProcInfo.hProcess, INFINITE);
        CloseHandle(piProcInfo.hProcess);
        CloseHandle(piProcInfo.hThread);
    }

    void CreatePipe(HANDLE* handle1, HANDLE* handle2)
    {
        BOOL bSuccess;
        bSuccess = ::CreatePipe(handle1, handle2, &saAttr, 0);

        if (!bSuccess)
        {
            throw CreateException("Can't create a pipe");
        }
    }

    void ReadFromHandle()
    {
        if (!ReadFile(hChildStdOutRead, bufferOut, BUFSIZE, &dwRead, NULL))
        {
            throw WriteReadPipeException("ReadFile failed, error");
        }
    }

    void WriteToHandle()
    {
        if (!WriteFile(hChildStdInWrite, bufferIn, BUFSIZE, &dwWritten, NULL))
        {
            throw WriteReadPipeException("WriteFile failed, error");
        }
    }

    BOOL Update()
    {
        if (GetExitCodeProcess(piProcInfo.hProcess, &dwExitCode); dwExitCode != STILL_ACTIVE)
        {
            printf("\nChild Proccess stopped\n");
            return FALSE;
        }

        printf("Enter a string (or 'exit' to quit): ");
        fgets(bufferIn, BUFSIZE, stdin);
        WriteToHandle();

        if (strcmp(bufferIn, "exit\n") == 0)
        {
            return FALSE;
        }

        ReadFromHandle();
        printf("Received from child: %s\n", bufferOut);
        return TRUE;
    }

    ProcessInfoSingle(const ProcessInfoSingle&) = delete;
    ProcessInfoSingle& operator=(const ProcessInfoSingle&) = delete;

private:
    ProcessInfoSingle() {}

    ~ProcessInfoSingle() {}

    static ProcessInfoSingle* instance;
    static std::mutex mutex;

    HANDLE hChildStdOutRead;
    HANDLE hChildStdInWrite;
    SECURITY_ATTRIBUTES saAttr;
    PROCESS_INFORMATION piProcInfo;
    STARTUPINFO siStartInfo;
    char bufferIn[BUFSIZE];
    char bufferOut[BUFSIZE];
    DWORD dwRead, dwWritten, dwExitCode;
};

ProcessInfoSingle* ProcessInfoSingle::instance = nullptr;
std::mutex ProcessInfoSingle::mutex;

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        printf("No file argument to write in parent");
        return 1;
    }

    HANDLE hChildStdInRead, hChildStdInWrite;
    HANDLE hChildStdOutRead, hChildStdOutWrite;
    ProcessInfoSingle& ProcessInfo = ProcessInfoSingle::GetInstance();
    ProcessInfo.Set_SECURITY_ATTRIBUTES(sizeof(SECURITY_ATTRIBUTES), NULL, TRUE);

    ProcessInfo.CreatePipe(&hChildStdInRead, &hChildStdInWrite);
    ProcessInfo.CreatePipe(&hChildStdOutRead, &hChildStdOutWrite);

    ProcessInfo.Set_STARTUPINFO(sizeof(STARTUPINFO), hChildStdInRead, hChildStdOutWrite, STARTF_USESTDHANDLES);
    ProcessInfo.SetHandles(hChildStdInWrite, hChildStdOutRead);

    char commandLine[BUFSIZE];
    snprintf(commandLine, BUFSIZE, "child.exe");
    for (int i = 1; i < argc; i++)
    {
        strcat(commandLine, " ");
        strcat(commandLine, argv[i]);
    }

    ProcessInfo.CreateChildProcess(commandLine);

    ProcessInfoSingle::CloseHandle(hChildStdInRead);
    ProcessInfoSingle::CloseHandle(hChildStdOutWrite);

    while (ProcessInfo.Update()) {}

    ProcessInfo.CloseChild();

    return 0;
}