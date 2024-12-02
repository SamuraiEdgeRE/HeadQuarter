// TCP�N���C�A���g
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <windows.h>
#include <thread>
#pragma comment(lib, "ws2_32.lib")

#define DEFAULT_BUFLEN 4096

// ���b�Z�[�W����M
void receiveMessage(SOCKET clientSocket) {
    char recvBuffer[DEFAULT_BUFLEN];
    while (true) {
        memset(recvBuffer, 0, sizeof(recvBuffer));
        int bytesReceived = recv(clientSocket, recvBuffer, sizeof(recvBuffer), 0);
        if (bytesReceived > 0) {
            std::cout << std::string(recvBuffer, bytesReceived);
        }
        else if (bytesReceived == 0) {
            std::cerr << "�T�[�o�[���ڑ�����܂����B" << std::endl;
            break;
        }
        else {
            std::cerr << "��M�Ɏ��s���܂����B�G���[: " << WSAGetLastError() << std::endl;
            break;
        }
    }
}

// ���b�Z�[�W�𑗐M
void sendMessage(SOCKET clientSocket) {
    std::string command;
    while (true) {
        std::getline(std::cin, command);

        if (command == "exit") {
            break;
        }

        std::string msg = command + "\n";
        int totalSent = 0;
        int msgLength = static_cast<int>(msg.size());

        while (totalSent < msgLength) {
            int bytesSent = send(clientSocket, msg.c_str() + totalSent, msgLength - totalSent, 0);
            if (bytesSent == SOCKET_ERROR) {
                std::cerr << "���M�Ɏ��s���܂����B�G���[: " << WSAGetLastError() << std::endl;
                break;
            }
            totalSent += bytesSent;
        }
    }
}

// �ڑ������\��
void ShowConnectingInfo(const std::string& ipAddress, const std::string& port) {
    std::cout << "[+] TCP�N���C�A���g" << std::endl;
    std::cout << "[+] �ڑ���IP�A�h���X: " << ipAddress << std::endl;
    std::cout << "[+] �ڑ���|�[�g�ԍ�: " << port << std::endl;
    std::cout << "[+] �T�[�o�[�ɐڑ������݂Ă��܂�..." << std::endl;
}


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

    SOCKET clientSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

    std::cout << "[+] �T�[�o�[�ɐڑ���..." << std::endl;
    ShowConnectingInfo(ipAddress, port);
    // �T�[�o�[�ɐڑ�
    int success = connect(clientSocket, result->ai_addr, (int)result->ai_addrlen);
    if (success == SOCKET_ERROR) {
        std::cerr << "�ڑ��Ɏ��s���܂����B�G���[: " << WSAGetLastError() << std::endl;
        return 1;
    }

    freeaddrinfo(result);

    std::cout << "[+] �T�[�o�[�ɐڑ����܂����B" << std::endl;

    // �T�[�o�[����̃��b�Z�[�W����M����X���b�h
    std::thread receiver(receiveMessage, clientSocket);

    // �T�[�o�[�Ƀ��b�Z�[�W�𑗐M����X���b�h
    std::thread sender(sendMessage, clientSocket);

    sender.join();
    receiver.join();

    closesocket(clientSocket);
    WSACleanup();
    return 0;
}
