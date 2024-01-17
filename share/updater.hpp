#pragma once
#include <debug.hpp>
#include <utility.hpp>
#include <Windows.h>
#include <Urlmon.h>   // URLOpenBlockingStreamW()
#include <atlbase.h>  // CComPtr
#include <atlstr.h>
#include <format>
#include <filesystem>
#include <regex>

#ifdef MEBIUS_EXPORT
#define MEBIUSAPI __declspec(dllexport)
#else
#define MEBIUSAPI __declspec(dllimport)
#endif

using namespace mebius::util;

//d 今後アップデーターを作る
namespace mebius::updater {
    class Updater {
    private:
        std::string _github_id;
        HMODULE _hModule;
        std::string _old_version;
        std::optional<std::string> _new_version;
        std::optional<std::string> get_latest_version(void) {
            std::string str;
            CComPtr<IStream> pStream;

            auto url = std::format("https://api.github.com/repos/{}/releases", this->_github_id);
            HRESULT hr;
            try {
                hr = URLOpenBlockingStreamA(nullptr, url.c_str(), &pStream, 0, nullptr);
            }
            catch (const std::exception& ex) {
                PLOGE << ex.what();
            }
            if (FAILED(hr))
            {
                PLOGE << "Could not connect. HRESULT: 0x" << std::hex << hr << std::dec;
                return std::nullopt;
            }

            // Download the response and write it to stdout.
            char buffer[4096];
            do
            {
                DWORD bytesRead = 0;
                hr = pStream->Read(buffer, sizeof(buffer), &bytesRead);

                if (bytesRead > 0)
                {
                    str += std::string(buffer, bytesRead);
                }
            } while (SUCCEEDED(hr) && hr != S_FALSE);

            if (FAILED(hr))
            {
                PLOGE << "Failed to connect to github. HRESULT: 0x" << std::hex << hr << std::dec;
                return std::nullopt;
            }

            std::regex pattern("\"tag_name\":\\s*\"([^\"]*)\"");
            std::smatch match;
            if (std::regex_search(str, match, pattern)) {
                // マッチした部分を出力
                PLOGD << "tag_name: " << match[1];
                return match[1];
            }
            else {
                PLOGE << "Could not found tag_name.";
                return std::nullopt;
            }
        }
    public:
        Updater(std::string github_id, HMODULE hModule, std::string old_version)
            : _github_id(github_id), _hModule(hModule), _old_version(old_version)
        {
            _new_version = get_latest_version();
        }

        std::optional<std::string> get_version(void) {
            return this->_new_version;
        }

        bool update(void) {
            if (!this->get_version().has_value()) {
                PLOGE << "Version check failed.";
                return false;
            }
            else if (this->get_version().value() == _old_version) {
                PLOGD << _old_version << " is the latest.";
                return true;
            }
            else {
                PLOGD << "Updating " << get_module_filename(_hModule) << "...";
                std::string old_path = get_module_path(_hModule) + ".old";
                std::string new_path = get_module_path(_hModule);
                std::filesystem::remove(old_path);
                if (std::filesystem::exists(new_path)) {
                    std::filesystem::rename(new_path, old_path);
                }

                std::string url = std::format("https://github.com/{}/releases/download/{}/{}", this->_github_id, this->get_version().value(), get_module_filename(_hModule));

                PLOGD << "Downloading " << url << "...";
                HRESULT res = URLDownloadToFileA(NULL, url.c_str(), new_path.c_str(), 0, NULL);

                if (res == S_OK) {
                    PLOGD << "Successful update.";
                    return true;
                }
                else {
                    PLOGE << "Could not download.";
                    return false;
                }
            }
        }
    };

}