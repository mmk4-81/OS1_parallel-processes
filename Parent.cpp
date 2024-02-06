#include <iostream>
#include <windows.h>
#include <fstream>
#include <string>
#include <chrono>

using namespace std;
#define Filepath "C:\\Users\\Desktop\\dataset\\"
#define Childpath "C:\\Users\\Desktop\\os_child\\Debug\\os_child.exe"

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

    HANDLE hProcessInfo = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, 3 * sizeof(double), TEXT("SharedMemory-ProcessInfo"));
    double *sharedProcessInfo = (double *)MapViewOfFile(hProcessInfo, FILE_MAP_ALL_ACCESS, 0, 0, 3 * sizeof(double));

    file.clear();
    file.seekg(0, ios::beg);

    for (int i = 0; i < DataSize; i++)
    {
        file >> sharedNumbers[i];
    }
    file.close();

    double totalLands = 0;
    for (int i = 0; i < DataSize; i++)
    {
        totalLands += sharedNumbers[i];
    }

    sharedProcessInfo[0] = totalLands;
    sharedProcessInfo[1] = DataSize;
    sharedProcessInfo[2] = runtime;

    STARTUPINFO *s = new STARTUPINFO[Core];
    PROCESS_INFORMATION *p = new PROCESS_INFORMATION[Core];

    for (int k = 0; k < Core; k++)
    {
        ZeroMemory(&s[k], sizeof(s[k]));
        s[k].cb = sizeof(s[k]);
        ZeroMemory(&p[k], sizeof(p[k]));

        bool b = CreateProcess(TEXT(Childpath), NULL, NULL, NULL, FALSE, 0, NULL, NULL, &s[k], &p[k]);
        if (!b)
        {
            cout << "Create Process failed" << endl;
            return 1;
        }
        else
        {
            result[k * (DataSize + 4)] = p[k].dwProcessId;
            cout << "Process " << k + 1 << " PID: " << p[k].dwProcessId << endl;
        }
    }
    cout << endl;

    return 0;
}