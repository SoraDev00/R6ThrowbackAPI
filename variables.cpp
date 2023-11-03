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
uintptr_t playerNameBaseAddress;
uintptr_t playerNameFinalAddress;
uintptr_t playerDeathsBaseAddress;
uintptr_t playerDeathsFinalAddress;
uintptr_t playerXPBaseAddress;
uintptr_t playerXPFinalAddress;
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
    ReadProcessMemory(hProcess, (BYTE*)playerNameFinalAddress, &retValue, sizeof(retValue), nullptr);
    return retValue;
}

int PlayerDeaths(){
    int retValue;
    ReadProcessMemory(hProcess, (BYTE*)playerDeathsFinalAddress, &retValue, sizeof(retValue), nullptr);
    return retValue;
}

int PlayerXP(){
    int retValue;
    ReadProcessMemory(hProcess, (BYTE*)playerXPFinalAddress, &retValue, sizeof(retValue), nullptr);
    return retValue;
}

int GetAllBaseAddress(){
    onMatchBaseAddress = GetBaseAddress(0x05880AA8, L"RainbowSix.exe");
    onMatchFinalAddress = FindDMAAddy(hProcess, onMatchBaseAddress, {0x7F4});
    onSituationBaseAddress = GetBaseAddress(0x045DB0B0, L"RainbowSix.exe");
    onSituationFinalAddress = FindDMAAddy(hProcess, onSituationBaseAddress, {0xB0, 0xD70});
    playerNameBaseAddress = GetBaseAddress(0x043F79C8, L"RainbowSix.exe");
    playerNameFinalAddress = FindDMAAddy(hProcess, playerNameBaseAddress, {0x18,0x120, 0x8, 0x40, 0x0});
    playerDeathsBaseAddress = GetBaseAddress(0x05880F40, L"RainbowSix.exe");
    playerDeathsFinalAddress = FindDMAAddy(hProcess, playerDeathsBaseAddress, {0xB8,0x38, 0xB8C});
    playerXPBaseAddress = GetBaseAddress(0x058821E8, L"RainbowSix.exe");
    playerXPFinalAddress = FindDMAAddy(hProcess, playerXPBaseAddress, {0x48,0x18, 0xB84});
    return 0;
}


variables::variables()
{

}
