#include "variables.h"
#include "proc.h"


DWORD GetProcID(const wchar_t* procName);

uintptr_t GetModuleBaseAddress(DWORD procId, const wchar_t* modName);

uintptr_t FindDMAAddy(HANDLE hProc, uintptr_t ptr, std::vector<unsigned int> offsets);

HANDLE hConsole;
HANDLE hProcess;
uintptr_t baseAddress;

uintptr_t GetBaseAddress(uintptr_t moduleOffset);
bool OnMatch();
bool OnSituation();

uintptr_t onMatchBaseAddress;
uintptr_t onMatchFinalAddress;
uintptr_t onSituationBaseAddress;
uintptr_t onSituationFinalAddress;
uintptr_t playerBaseAddress;
uintptr_t playerFinalAddress;
bool onMatch;

uintptr_t GetBaseAddress(uintptr_t moduleOffset, const wchar_t* moduleName){
    DWORD procId = GetProcID(L"RainbowSix.exe");
    if(procId != 0)
    {
        hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        //GetModuleBaseAddress
        uintptr_t moduleBase = GetModuleBaseAddress(procId, moduleName);
        //Get Handle to Process
        hProcess = OpenProcess(PROCESS_ALL_ACCESS, NULL, procId);

        return(moduleBase + moduleOffset);
    }
    else
        return 0;

}

bool OnMatch(){
    char retValue;
    ReadProcessMemory(hProcess, (BYTE*)onMatchFinalAddress, &retValue, sizeof(retValue), nullptr);
    if(retValue == 1)
        return true;
    else
        return false;
}

bool OnSituation(){
    char retValue;
    ReadProcessMemory(hProcess, (BYTE*)onSituationFinalAddress, &retValue, sizeof(retValue), nullptr);
    if(retValue == 1)
        return true;
    else
        return false;
}

bool OnMainMenu(){
    if(!OnSituation() && !OnMatch())
        return true;
    else
        return false;
}
std::string PlayerName(){
    char retValue[100];
    ReadProcessMemory(hProcess, (BYTE*)playerFinalAddress, &retValue, sizeof(retValue), nullptr);
    return retValue;
}

int GetAllBaseAddress(){
    onMatchBaseAddress = GetBaseAddress(0x05880AA8, L"RainbowSix.exe");
    onMatchFinalAddress = FindDMAAddy(hProcess, onMatchBaseAddress, {0x7F4});
    onSituationBaseAddress = GetBaseAddress(0x045DB0B0, L"RainbowSix.exe");
    onSituationFinalAddress = FindDMAAddy(hProcess, onSituationBaseAddress, {0xB0, 0xD70});
    playerBaseAddress = GetBaseAddress(0x043F79C8, L"RainbowSix.exe");
    playerFinalAddress = FindDMAAddy(hProcess, playerBaseAddress, {0x18,0x120, 0x8, 0x40, 0x0});
    return 0;
}


variables::variables()
{

}
