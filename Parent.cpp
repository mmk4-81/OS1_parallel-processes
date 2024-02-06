#include <iostream>
#include <windows.h>
#include <fstream>
#include <string>
#include <chrono>

using namespace std;
#define Filepath "..\\dataset\\"
#define Childpath "..\\os_child.exe"

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

    double idealboy1 = totalLands / 5 * 2;
    double idealboy2 = totalLands / 5 * 2;
    double idealgirl = totalLands / 5;
    cout << "total lands =>" << totalLands << endl;
    cout << "Ideal for boy 1 => " << idealboy1 << endl;
    cout << "Ideal for boy 2 => " << idealboy2 << endl;
    cout << "Ideal for girl => " << idealgirl << endl;
    cout << endl;

    for (int i = 0; i < Core; i++) {
        WaitForSingleObject(p[i].hProcess, INFINITE);
        CloseHandle(p[i].hProcess);
        CloseHandle(p[i].hThread);
    }

    delete[] s;
    delete[] p;

    double* bestResult = nullptr;
    double* distribution = new double[DataSize];
    int sum[3] = {0,0,0};
    double bestPid = result[0];
    double* F = new double[Core];

    for (int i = 0; i < DataSize; i++)
    {
        distribution[i] = result[i+ 1];
    }
    for (int i = 0; i < 3; i++)
    {
        sum[i] = result[DataSize + i + 1];
    }

    double bestDiff = abs(sum[0] - 0.4 * totalLands) + abs(sum[1] - 0.4 * totalLands) + abs(sum[2] - 0.2 * totalLands);

    cout << "Note: boy1 => 1  **** boy2 => 2  **** girl => 3\n\n";

    for (int i = 0; i < Core; i++) {

        double* currentResult = result + i * (DataSize + 4);
        double currentPid = currentResult[0];

        double currentDiff = abs(currentResult[DataSize + 1] - 0.4 * totalLands) + abs(currentResult[DataSize + 2] - 0.4 * totalLands) + abs(currentResult[DataSize + 3] - 0.2 * totalLands);

        cout << "Process " << i + 1 << " (PID = " << currentPid << ") >>\n [ ";
        for (int j = 0; j < DataSize; j++) {
            cout << sharedNumbers[j] << "(" << currentResult[j+1 ] << ") , ";
        }
        cout << " ] \n ";
        cout << "boy1 => " << currentResult[DataSize + 1] << " ****  boy2 => " << currentResult[DataSize + 2] << " ****  girl => " << currentResult[DataSize + 3];
        cout << " **** F => " << currentDiff;
        cout << endl;
        cout << "-------------------------------------------------------------------------------------------------------------------\n";


        if (currentDiff < bestDiff) {
            bestDiff = currentDiff;
            bestResult = currentResult;
            bestPid = currentPid;
        }
    }
    cout << "Best PID (" << bestPid << ") => f = " << bestDiff << endl;

    UnmapViewOfFile(sharedNumbers);
    UnmapViewOfFile(result);
    UnmapViewOfFile(sharedProcessInfo);

    CloseHandle(hData);
    CloseHandle(hResult);
    CloseHandle(hProcessInfo);


    return 0;
}