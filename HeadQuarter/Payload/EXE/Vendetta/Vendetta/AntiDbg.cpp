// �f�o�b�K�����m����֐����`
#include "AntiDbg.h"


// IsDebuggerPresent�֐����g�������m
void IsDebuggerPresentByAPI() {
    if (IsDebuggerPresent()) {
        // ���m
        exit(1);
    }
    return;
}


// NtGlobalFlag���g�������m
void NtGlobalFlagByPEB() {
    PPEB ppeb = (PPEB)__readgsqword(0x60);
    DWORD NtGlobalFlag = ppeb->NtGlobalFlag;
    if (NtGlobalFlag & 0x70) {
        //���m
        exit(1);
    }
}



// tls�R�[���o�b�N���g�������m
#define NtCurrentProcess() (HANDLE)-1

void NTAPI __stdcall TLS_CALLBACK(PVOID DllHandle, DWORD dwReason, PVOID Reserved){
    HANDLE DebugPort = NULL;
    if (!NtQueryInformationProcess(NtCurrentProcess(), 7, &DebugPort, sizeof(HANDLE), NULL))
    {
        if (DebugPort)
        {
            // ���m
            exit(1);
        }
    }
}


EXTERN_C const PIMAGE_TLS_CALLBACK _tls_callback[] = { TLS_CALLBACK, 0 };


// CheckRemoteDebuggerPresent�֐����g�������m
void CheckRemoteDebugger() {

    BOOL isDebug = FALSE;
    if (CheckRemoteDebuggerPresent(GetCurrentProcess(), &isDebug)) {
        if (isDebug) {
            // ���m
            exit(1);
        }
    }
}



// �E�B���h�E���ł̌��m
void WindowNameBlock() {

    LPCWSTR windowName = L"x64dbg";
    if (FindWindow(NULL, windowName)) {
        // ���m
        exit(1);
    }
}

// �v���Z�X���ł̌��m
void IsProcessRunning() {
    bool result = false;
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot != INVALID_HANDLE_VALUE) {
        PROCESSENTRY32 pe32;
        pe32.dwSize = sizeof(PROCESSENTRY32);
        if (Process32First(hSnapshot, &pe32)) {
            do {
                if (wcscmp(pe32.szExeFile, L"x64dbg.exe") == 0) {
                    result = true;
                    break;
                }
            } while (Process32Next(hSnapshot, &pe32));
        }
        CloseHandle(hSnapshot);
        
        if (result) {
            // ���m
            exit(1);
        }
    }
}

