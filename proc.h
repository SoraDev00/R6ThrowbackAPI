#ifndef PROC_H
#define PROC_H
#include <vector>
#include <Windows.h>
#include <TlHelp32.h>

class proc
{
public:
    proc();

    DWORD GetProcID(const wchar_t* procName);

    uintptr_t GetModuleBaseAddress(DWORD procId, const wchar_t* modName);

    uintptr_t FindDMAAddy(HANDLE hProc, uintptr_t ptr, std::vector<unsigned int> offsets);
};

#endif // PROC_H
