#include <_utility.hpp>
#include <_debug.hpp>

#include <algorithm>
#include <format>

#define BUFSIZE 1024
#define MD5LEN  16




uint32_t mebius::util::detect_mugen(void) {
    // ハッシュを計算
    std::string md5 = calc_md5_self();

    // ハッシュログを表示
    if (md5.empty()) {
        return false;
    }

    // 実行ファイルのハッシュから本体を識別
    try {
        auto result = MUGEN_HASH_LIST.at(md5);
        PLOGD << "Registered WINMUGEN: " << result.first;
        PLOGD << "MD5Hash: " << md5;
        return result.second;
    }
    catch (const std::out_of_range& e) {
        PLOGE << "Unregistered WINMUGEN.";
        PLOGE << "MD5Hash: " << md5;
        return 0xFFFFFFFF;
    }

}

std::string mebius::util::calc_md5_self()
{
    std::string md5{};

    DWORD dwStatus = 0;
    BOOL bResult = FALSE;
    HCRYPTPROV hProv = 0;
    HCRYPTHASH hHash = 0;
    HANDLE hFile = NULL;
    BYTE rgbFile[BUFSIZE];
    DWORD cbRead = 0;
    BYTE rgbHash[MD5LEN];
    DWORD cbHash = 0;
    CHAR rgbDigits[] = "0123456789abcdef";


    WCHAR filename[MAX_PATH];
    DWORD size = sizeof(filename) / sizeof(filename[0]);
    DWORD result = GetModuleFileName(NULL, filename, MAX_PATH);
    if (result == 0) {
        dwStatus = GetLastError();
        PLOGE << "GetModuleFileName failed :" << dwStatus;
        return "";
    }

    hFile = CreateFile(filename,
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_FLAG_SEQUENTIAL_SCAN,
        NULL);

    if (INVALID_HANDLE_VALUE == hFile)
    {
        dwStatus = GetLastError();
        PLOGE << "CreateFile failed :" << dwStatus;
        return "";
    }

    // Get handle to the crypto provider
    if (!CryptAcquireContext(&hProv,
        NULL,
        NULL,
        PROV_RSA_FULL,
        CRYPT_VERIFYCONTEXT))
    {
        dwStatus = GetLastError();
        PLOGE << "CryptAcquireContext failed :" << dwStatus;
        CloseHandle(hFile);
        return "";
    }

    if (!CryptCreateHash(hProv, CALG_MD5, 0, 0, &hHash))
    {
        dwStatus = GetLastError();
        PLOGE << "CryptCreateHash failed :" << dwStatus;
        CloseHandle(hFile);
        return "";
    }

    while (bResult = ReadFile(hFile, rgbFile, BUFSIZE,
        &cbRead, NULL))
    {
        if (0 == cbRead)
        {
            break;
        }

        if (!CryptHashData(hHash, rgbFile, cbRead, 0))
        {
            dwStatus = GetLastError();
            PLOGE << "CryptHashData failed :" << dwStatus;
            CryptReleaseContext(hProv, 0);
            CryptDestroyHash(hHash);
            CloseHandle(hFile);
            return "";
        }
    }

    if (!bResult)
    {
        dwStatus = GetLastError();
        PLOGE << "ReadFile failed :" << dwStatus;
        CryptReleaseContext(hProv, 0);
        CryptDestroyHash(hHash);
        CloseHandle(hFile);
        return "";
    }

    cbHash = MD5LEN;
    if (CryptGetHashParam(hHash, HP_HASHVAL, rgbHash, &cbHash, 0))
    {
        for (DWORD i = 0; i < cbHash; i++)
        {
            md5 += std::format("{}{}", rgbDigits[rgbHash[i] >> 4], rgbDigits[rgbHash[i] & 0xf]);
        }
    }
    else
    {
        dwStatus = GetLastError();
        PLOGE << "CryptGetHashParam failed :" << dwStatus;
        return "";
    }

    CryptDestroyHash(hHash);
    CryptReleaseContext(hProv, 0);
    CloseHandle(hFile);

    return md5;
}
