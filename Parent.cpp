#include <iostream>
#include <windows.h>
#include <fstream>
#include <string>
#include <chrono>

using namespace std;
#define Filepath "C:\\Users\\Desktop\\dataset\\"



int main() {
    string filename;
    cout << "Enter dataset filename(1-6) : ";
    cin >> filename;

    ifstream file(Filepath + filename + ".txt");
    if (!file) {
        cout << "Cannot open file: " << filename << endl;
        return 1;
    }

    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);
    int Core = sysinfo.dwNumberOfProcessors;
    
    double runtime = 0;
    cout << "Enter runtime (seconds): ";
    cin >> runtime;


    return 0;
}