#include <Mebius.hpp>
#include <_Mebius.hpp>
#include <algorithm>

#define BUFSIZE 1024
#define MD5LEN  16

bool mebius::util::checksum(void) {
    mebius::debug::Logger meblog(std::cout, FOREGROUND_LIME);
    mebius::debug::Logger meberr(std::cerr, FOREGROUND_PINK);

    // ハッシュを計算
    std::string md5 = calc_md5_self();

    // ハッシュログを表示
    if (md5.empty()) {
        return false;
    }

    // 実行ファイルのハッシュから本体を識別
    try {
        meblog << std::format("Registered WINMUGEN: {}", MUGEN_HASH_LIST.at(md5)) << std::endl;
        meblog << std::format("MD5Hash: {}", md5) << std::endl;
        return true;
    }
    catch (const std::out_of_range& e) {
        meberr << "Unregistered WINMUGEN." << std::endl;
        meberr << std::format("MD5Hash: {}", md5) << std::endl;
        return false;
    }

}

std::string mebius::util::calc_md5_self()
{
    mebius::debug::Logger utilerr(std::cerr, FOREGROUND_PINK);
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
        utilerr << std::format("GetModuleFileName failed: {}", dwStatus) << std::endl;
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
        utilerr << std::format("CreateFile failed: {}", dwStatus) << std::endl;
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
        utilerr << std::format("CryptAcquireContext failed: {}", dwStatus) << std::endl;
        CloseHandle(hFile);
        return "";
    }

    if (!CryptCreateHash(hProv, CALG_MD5, 0, 0, &hHash))
    {
        dwStatus = GetLastError();
        utilerr << std::format("CryptCreateHash failed: {}", dwStatus) << std::endl;
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
            utilerr << std::format("CryptHashData failed: {}", dwStatus) << std::endl;
            CryptReleaseContext(hProv, 0);
            CryptDestroyHash(hHash);
            CloseHandle(hFile);
            return "";
        }
    }

    if (!bResult)
    {
        dwStatus = GetLastError();
        utilerr << std::format("ReadFile failed: {}", dwStatus) << std::endl;
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
        printf("CryptGetHashParam failed: %d\n", dwStatus);
        return "";
    }

    CryptDestroyHash(hHash);
    CryptReleaseContext(hProv, 0);
    CloseHandle(hFile);

    return md5;
}
