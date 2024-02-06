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

    double* sharedProcessInfo = (double*)MapViewOfFile(hProcessInfo, FILE_MAP_ALL_ACCESS, 0, 0, 3 * sizeof(double));

    double totalLands = sharedProcessInfo[0];
    int DataSize = sharedProcessInfo[1];
    double runtime = sharedProcessInfo[2];

    double* lands = (double*)MapViewOfFile(hData, FILE_MAP_ALL_ACCESS, 0, 0, DataSize * sizeof(double));
    double* result = (double*)MapViewOfFile(hResult, FILE_MAP_ALL_ACCESS, 0, 0, Core * (DataSize + 4) * sizeof(double));



    return 0;
}