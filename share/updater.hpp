#pragma once

#ifdef MEBIUS_EXPORT
#define MEBIUSAPI __declspec(dllexport)
#else
#define MEBIUSAPI __declspec(dllimport)
#endif


//d 今後アップデーターを作る
namespace mebius::updater {
    class MEBIUSAPI Update
    {
    public:
        Update(std::string version, std::string github_id) {

        }
        ~Update() {
            WCHAR filename[MAX_PATH];
            DWORD size = sizeof(filename) / sizeof(filename[0]);
            DWORD result = GetModuleFileName(NULL, filename, MAX_PATH);
        }
    private:

    };
}