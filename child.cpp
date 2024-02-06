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

    double *sharedProcessInfo = (double *)MapViewOfFile(hProcessInfo, FILE_MAP_ALL_ACCESS, 0, 0, 3 * sizeof(double));

    double totalLands = sharedProcessInfo[0];
    int DataSize = sharedProcessInfo[1];
    double runtime = sharedProcessInfo[2];

    double *lands = (double *)MapViewOfFile(hData, FILE_MAP_ALL_ACCESS, 0, 0, DataSize * sizeof(double));
    double *result = (double *)MapViewOfFile(hResult, FILE_MAP_ALL_ACCESS, 0, 0, Core * (DataSize + 4) * sizeof(double));

    if (lands == NULL || result == NULL || sharedProcessInfo == NULL)
    {
        cout << "Could not open file mapping object (" << GetLastError() << ")" << endl;
        CloseHandle(hData);
        CloseHandle(hResult);
        CloseHandle(hProcessInfo);
        return 1;
    }

    int pid = GetCurrentProcessId();
    int index = 0;

    while (result[index] != pid)
    {
        index += (DataSize + 4);
        if (index > (Core - 1) * (DataSize + 4))
        {
            index = 0;
        }
    }

    double currentdiff = abs(result[index + DataSize + 1] - 0.4 * totalLands) + abs(result[index + DataSize + 2] - 0.4 * totalLands) + abs(result[index + DataSize + 3] - 0.2 * totalLands);

    auto start_time = high_resolution_clock::now();

    while (true)
    {

        double *distribution = new double[DataSize];
        int sum[3] = {0, 0, 0};

        srand(time(NULL) + pid);
        for (int i = 0; i < DataSize; i++)
        {
            int randomindex = rand() % 3;
            sum[randomindex] += lands[i];
            distribution[i] = randomindex;
        }

        double newdiff = abs(sum[0] - 0.4 * totalLands) + abs(sum[1] - 0.4 * totalLands) + abs(sum[2] - 0.2 * totalLands);

        if (newdiff < currentdiff)
        {
            currentdiff = newdiff;

            for (int i = 0; i < DataSize; i++)
            {
                result[index + i + 1] = distribution[i];
            }
            for (int i = 0; i < 3; i++)
            {
                result[index + DataSize + i + 1] = sum[i];
            }
        }

        auto end_time = high_resolution_clock::now();
        auto elapsed_time = duration_cast<duration<double>>(end_time - start_time).count();

        if (elapsed_time >= runtime)
        {
            break;
        }
    }

    UnmapViewOfFile(lands);
    UnmapViewOfFile(result);
    UnmapViewOfFile(sharedProcessInfo);

    CloseHandle(hData);
    CloseHandle(hResult);
    CloseHandle(hProcessInfo);

    return 0;
}