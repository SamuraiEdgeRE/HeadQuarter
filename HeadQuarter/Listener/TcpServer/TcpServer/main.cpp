// ���o�[�XTCP
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <thread>
#include <atomic>
#include <windows.h>
#include <sstream>
#include <math.h>
#include <chrono>

#pragma comment(lib, "ws2_32.lib")

#define DEFAULT_BUFLEN 4096

// �\�P�b�g�ƃA�h���X���
SOCKET listenSocket = INVALID_SOCKET;
SOCKET clientSocket = INVALID_SOCKET;
std::atomic<bool> running(true);


// ���b�Z�[�W�𑗐M
void sendMessage() {
    std::string command;
    while (running) {
        std::getline(std::cin, command);

        if (command == "exit") {
            running = false;
            exit(0);
        }

        std::string msg = command + "\n";
        int totalSent = 0;
        int msgLength = static_cast<int>(msg.size());

        while (totalSent < msgLength) {
            int bytesSent = send(clientSocket, msg.c_str() + totalSent, msgLength - totalSent, 0);
            if (bytesSent == SOCKET_ERROR) {
                std::cerr << "���M�Ɏ��s���܂����B�G���[: " << WSAGetLastError() << std::endl;
                running = false;
                break;
            }
            totalSent += bytesSent;
        }
    }
}

// ���b�Z�[�W����M
void receiveMessage() {
    char recvBuffer[DEFAULT_BUFLEN];
    while (running) {
        memset(recvBuffer, 0, sizeof(recvBuffer));
        int bytesReceived = recv(clientSocket, recvBuffer, sizeof(recvBuffer), 0);
        if (bytesReceived > 0) {
            std::cout << std::string(recvBuffer, bytesReceived);
        }
        else if (bytesReceived == 0) {
            std::cerr << "�N���C�A���g���ڑ�����܂����B" << std::endl;
            running = false; // ���[�v���I��
            break;
        }
        else {
            std::cerr << "��M�Ɏ��s���܂����B�G���[: " << WSAGetLastError() << std::endl;
            running = false; // ���[�v���I��
            break;
        }
    }
}

// �ҋ@���b�Z�[�W��\��
void ShowListeningInfo(const std::string& ipAddress, const std::string& port) {
    std::cout << "[+] TCP���X�i�[" << std::endl;
    std::cout << "[+] IP�A�h���X: " << ipAddress << std::endl;
    std::cout << "[+] �|�[�g�ԍ�: " << port << std::endl;
    std::cout << "[+] �N���C�A���g�̐ڑ���҂��Ă��܂�..." << std::endl;
}

void ShowAcceptInfo(sockaddr_in& clientAddr) {
    char ipStr[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &clientAddr.sin_addr, ipStr, INET_ADDRSTRLEN);
    std::cout << "---------------------------------------------------------------------------------------------------------------" << std::endl;
    std::cout << "[+] �N���C�A���g���ڑ�����܂����B" << std::endl;
    std::cout << "[+] �N���C�A���g��IP�A�h���X: " << ipStr << std::endl;
    std::cout << "[+] �N���C�A���g�̃|�[�g�ԍ�: " << ntohs(clientAddr.sin_port) << std::endl;
}

// ���C���֐�
int main(int argc, char* argv[]) {

    if (argc != 3) {
        std::cout << "�g�p���@: " << argv[0] << " <IP�A�h���X> <�|�[�g�ԍ�>" << std::endl;
        return 1;
    }

    std::string ipAddress = argv[1];
    std::string port = argv[2];

    WSADATA wsaData;
    struct addrinfo* result = NULL, hints;

    WSAStartup(MAKEWORD(2, 2), &wsaData);

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    getaddrinfo(ipAddress.c_str(), port.c_str(), &hints, &result);

    listenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

    // �\�P�b�g�Ƀo�C���h
    int success = bind(listenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (success == SOCKET_ERROR) {
        std::cerr << "[-] �o�C���h�Ɏ��s���܂����B�G���[: " << WSAGetLastError() << std::endl;
        return 1;
    }

    freeaddrinfo(result);

    ShowListeningInfo(ipAddress, port);

    // �ڑ������b�X��
    listen(listenSocket, SOMAXCONN);

    sockaddr_in clientAddr;
    int clientAddrSize = sizeof(clientAddr);

    clientSocket = accept(listenSocket, (struct sockaddr*)&clientAddr, &clientAddrSize);

    ShowAcceptInfo(clientAddr);

    std::thread receiver(receiveMessage);
    std::thread sender(sendMessage);

    sender.join();
    receiver.join();

    closesocket(clientSocket);
    closesocket(listenSocket);
    WSACleanup();
    return 0;
}
