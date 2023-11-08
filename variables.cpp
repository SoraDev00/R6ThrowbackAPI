#include "variables.h"
#include "proc.h"
#include <ostream>
#include <QFile>
#include <QTextStream>




DWORD GetProcID(const wchar_t* procName);

uintptr_t GetModuleBaseAddress(DWORD procId, const wchar_t* modName);

uintptr_t FindDMAAddy(HANDLE hProc, uintptr_t ptr, std::vector<unsigned int> offsets);

HANDLE hConsole;
HANDLE hProcess;
uintptr_t baseAddress;

uintptr_t GetBaseAddress(uintptr_t moduleOffset);
bool OnMatch();
bool OnSituation();
const char* GetText();

uintptr_t onMatchBaseAddress;
uintptr_t onMatchFinalAddress;
uintptr_t onSituationBaseAddress;
uintptr_t onSituationFinalAddress;
uintptr_t playerDeathsBaseAddress;
uintptr_t playerDeathsFinalAddress;
uintptr_t playerXPBaseAddress;
uintptr_t playerXPFinalAddress;
uintptr_t playerKillsBaseAddress;
uintptr_t playerKillsFinalAddress;
uintptr_t playerRemainingPlayersBaseAddress;
uintptr_t playerRemainingPlayersFinalAddress;
uintptr_t playerWinBaseAddress;
uintptr_t playerWinFinalAddress;
uintptr_t isPlayingBaseAddress;
uintptr_t isPlayingFinalAddress;

bool onMatch;

uintptr_t GetBaseAddress(uintptr_t moduleOffset, const wchar_t* moduleName){
    DWORD procId = GetProcID(L"RainbowSix.exe");
    if(procId != 0)
    {
        hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

        uintptr_t moduleBase = GetModuleBaseAddress(procId, moduleName);

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

int PlayerKills(){
    int retValue;
    ReadProcessMemory(hProcess, (BYTE*)playerKillsFinalAddress, &retValue, sizeof(retValue), nullptr);
    return retValue;
}

int RemainingEnemies(){
    int retValue;
    ReadProcessMemory(hProcess, (BYTE*)playerRemainingPlayersFinalAddress, &retValue, sizeof(retValue), nullptr);
    return retValue;
}

int PlayerWin(){
    int retValue;
    ReadProcessMemory(hProcess, (BYTE*)playerWinFinalAddress, &retValue, sizeof(retValue), nullptr);
    return retValue;
}

int IsPlaying(){
    int retValue;
    ReadProcessMemory(hProcess, (BYTE*)isPlayingFinalAddress, &retValue, sizeof(retValue), nullptr);
    return retValue;
}

int GetAllBaseAddress(){
    onMatchBaseAddress = GetBaseAddress(0x061B5468, L"RainbowSix.exe");
    onMatchFinalAddress = FindDMAAddy(hProcess, onMatchBaseAddress, {0x88, 0x8, 0x18, 0x68, 0x854});
    onSituationBaseAddress = GetBaseAddress(0x061E0720, L"RainbowSix.exe");
    onSituationFinalAddress = FindDMAAddy(hProcess, onSituationBaseAddress, {0x48, 0x0, 0x60, 0xCE0});
    playerDeathsBaseAddress = GetBaseAddress(0x058821E8, L"RainbowSix.exe");
    playerDeathsFinalAddress = FindDMAAddy(hProcess, playerDeathsBaseAddress, {0x48 ,0x18, 0xD0, 0x8C});
    playerXPBaseAddress = GetBaseAddress(0x058821E8, L"RainbowSix.exe");
    playerXPFinalAddress = FindDMAAddy(hProcess, playerXPBaseAddress, {0x48 ,0x18, 0xD0, 0x84});
    playerKillsBaseAddress = GetBaseAddress(0x058821E8, L"RainbowSix.exe");
    playerKillsFinalAddress = FindDMAAddy(hProcess, playerKillsBaseAddress, {0x48 ,0x18, 0xD0, 0x70});
    playerRemainingPlayersBaseAddress = GetBaseAddress(0x058828D0, L"RainbowSix.exe");
    playerRemainingPlayersFinalAddress = FindDMAAddy(hProcess, playerRemainingPlayersBaseAddress, {0x18 ,0x70, 0x18, 0x1B0});
    playerWinBaseAddress = GetBaseAddress(0x061C3100, L"RainbowSix.exe");
    playerWinFinalAddress = FindDMAAddy(hProcess, playerWinBaseAddress, {0x70});
    isPlayingBaseAddress = GetBaseAddress(0x05880AA8, L"RainbowSix.exe");
    isPlayingFinalAddress = FindDMAAddy(hProcess, isPlayingBaseAddress, {0x2D8});


    return 0;
}

QString GetPath() {
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, GetProcID(L"RainbowSix.exe"));
    if (hProcess) {
        char filePath[MAX_PATH];
        DWORD size = sizeof(filePath);

        if (QueryFullProcessImageNameA(hProcess, 0, filePath, &size)) {
            QString fullPath = QString::fromLocal8Bit(filePath);
            size_t lastSlash = fullPath.lastIndexOf('\\');
            if (lastSlash != -1) {
                QString folderPath = fullPath.left(lastSlash);
                CloseHandle(hProcess);
                return folderPath;
            }
        }
        CloseHandle(hProcess);
    }

    return "Null";
}


variables::variables()
{

}
