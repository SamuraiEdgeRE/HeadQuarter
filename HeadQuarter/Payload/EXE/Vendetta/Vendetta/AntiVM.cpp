// ���z�}�V�������m����֐����`
#include "AntiVM.h"

void CheckBIOS() {
    int cpuInfo[4];
    char manufacturer[13];

    // CPUID���g�p����BIOS�x���_�[�����擾
    __cpuid(cpuInfo, 0);
    memcpy(manufacturer, &cpuInfo[1], 4);
    memcpy(manufacturer + 4, &cpuInfo[3], 4);
    memcpy(manufacturer + 8, &cpuInfo[2], 4);
    manufacturer[12] = '\0';

    // ���z���̊m�F
    if (strcmp(manufacturer, "VMwareVMware") == 0) {
        exit(1);
    }
    else if (strcmp(manufacturer, "Microsoft Hv") == 0) {
        exit(1);

    }
    else if (strcmp(manufacturer, "KVMKVMKVMKVM") == 0) {
        exit(1);
    }
    else if (strcmp(manufacturer, "VBoxVBoxVBox") == 0) {
        exit(1);
    }
    else {
    }
}

// �v���Z�X���X�g�ɂ�錟�o
void CheckProcesses() {
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        return;
    }

    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);

    if (Process32First(hSnapshot, &pe32)) {
        do {
            // VMware�֘A�v���Z�X�̌��o
            if (wcscmp(pe32.szExeFile, L"vmtoolsd.exe") == 0 ||
                wcscmp(pe32.szExeFile, L"vmwaretray.exe") == 0 ||
                wcscmp(pe32.szExeFile, L"vmwareuser.exe") == 0) {
                exit(1);
            }
            // VirtualBox�֘A�v���Z�X�̌��o
            else if (wcscmp(pe32.szExeFile, L"VBoxService.exe") == 0 ||
                wcscmp(pe32.szExeFile, L"VBoxTray.exe") == 0) {
                exit(1);
            }
        } while (Process32Next(hSnapshot, &pe32));
    }

    CloseHandle(hSnapshot);
}

// �f�o�C�X�h���C�o�ɂ�錟�o
void CheckDrivers() {
    SC_HANDLE hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ENUMERATE_SERVICE);
    if (hSCM == NULL) {
        return;
    }

    DWORD bytesNeeded = 0;
    DWORD servicesReturned = 0;
    DWORD resumeHandle = 0;


    EnumServicesStatusEx(hSCM, SC_ENUM_PROCESS_INFO, SERVICE_DRIVER,
        SERVICE_STATE_ALL, NULL, 0, &bytesNeeded, &servicesReturned,
        &resumeHandle, NULL);

    if (bytesNeeded > 0) {
        LPBYTE lpServices = (LPBYTE)malloc(bytesNeeded);
        if (lpServices != NULL) {
            if (EnumServicesStatusEx(hSCM, SC_ENUM_PROCESS_INFO, SERVICE_DRIVER,
                SERVICE_STATE_ALL, lpServices, bytesNeeded, &bytesNeeded,
                &servicesReturned, &resumeHandle, NULL)) {

                LPENUM_SERVICE_STATUS_PROCESS services = (LPENUM_SERVICE_STATUS_PROCESS)lpServices;
                for (DWORD i = 0; i < servicesReturned; i++) {
                    // VMware�֘A�̃h���C�o�����o
                    if (wcscmp(services[i].lpDisplayName, L"VMware Tools") == 0 ||
                        wcscmp(services[i].lpDisplayName, L"VMware") == 0) {
                        exit(1);
                    }
                    // VirtualBox�֘A�̃h���C�o�����o
                    else if (wcscmp(services[i].lpDisplayName, L"VirtualBox") == 0 ||
                        wcscmp(services[i].lpDisplayName, L"VBox") == 0) {
                        exit(1);
                    }
                }
            }
            free(lpServices);
        }
    }

    CloseServiceHandle(hSCM);
}

// ���W�X�g���ɂ�錟�o
void CheckRegistry() {
    HKEY hKey;

    // VMware Tools �̃��W�X�g���L�[���`�F�b�N
    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SOFTWARE\\VMware, Inc.\\VMware Tools", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        RegCloseKey(hKey);
        exit(1);
    }

    // VirtualBox Guest Additions �̃��W�X�g���L�[���`�F�b�N
    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SOFTWARE\\Oracle\\VirtualBox Guest Additions", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        RegCloseKey(hKey);
        exit(1);
    }
}

