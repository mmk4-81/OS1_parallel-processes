#include <windows.h>
#include <iostream>
#include <chrono>
#include <string>

using namespace std;
using namespace std::chrono;

int main()
{
    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);
    int Core = sysinfo.dwNumberOfProcessors;

    HANDLE hData = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, TEXT("SharedMemory-data"));
    HANDLE hResult = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, TEXT("SharedMemory-result"));
    HANDLE hProcessInfo = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, TEXT("SharedMemory-ProcessInfo"));

    return 0;
}