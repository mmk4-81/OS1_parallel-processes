#include <iostream>
#include <windows.h>
#include <fstream>
#include <string>
#include <chrono>

using namespace std;
#define Filepath "C:\\Users\\Desktop\\dataset\\"

int main()
{
    string filename;
    cout << "Enter dataset filename(1-6) : ";
    cin >> filename;

    ifstream file(Filepath + filename + ".txt");
    if (!file)
    {
        cout << "Cannot open file: " << filename << endl;
        return 1;
    }

    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);
    int Core = sysinfo.dwNumberOfProcessors;

    double runtime = 0;
    cout << "Enter runtime (seconds): ";
    cin >> runtime;

    int DataSize = 0;
    double number;
    while (file >> number)
    {
        DataSize++;
    }

    cout << endl
         << filename << " with " << DataSize << " records and runtime " << runtime << " seconds and with " << Core << " core" << endl
         << endl;

    HANDLE hData = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, DataSize * sizeof(double), TEXT("SharedMemory-data"));
    double *sharedNumbers = (double *)MapViewOfFile(hData, FILE_MAP_ALL_ACCESS, 0, 0, DataSize * sizeof(double));

    HANDLE hResult = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, Core * (DataSize + 4) * sizeof(double), TEXT("SharedMemory-result"));
    double *result = (double *)MapViewOfFile(hResult, FILE_MAP_ALL_ACCESS, 0, 0, Core * (DataSize + 4) * sizeof(double));

    return 0;
}