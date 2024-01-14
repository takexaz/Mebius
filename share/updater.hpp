#pragma once
#include <debug.hpp>
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

//d ����A�b�v�f�[�^�[�����
namespace mebius::updater {
    class Updater {
    private:
        std::string _github_id;
        std::string _file_relative_path;
        std::string _old_version;
        std::optional<std::string> _new_version;
        std::string get_file_name(void) {
            const std::filesystem::path path = _file_relative_path;
            return path.filename().string();
        }
        std::string get_dir_path(void) {
            // �ԋp�l
            CString res = (LPCTSTR)nullptr;
            // �p�X�A�h���C�u���A�f�B���N�g�����A�t�@�C�����A�g���q
            TCHAR path[_MAX_PATH], drive[_MAX_PATH], dir[_MAX_PATH], file[_MAX_PATH], ext[_MAX_PATH];
            // �t���p�X���擾
            if (::GetModuleFileName(NULL, path, _MAX_PATH) != 0)
            {
                // �t�@�C���p�X�𕪊�
                ::_tsplitpath_s(path, drive, _MAX_PATH, dir, _MAX_PATH, file, _MAX_PATH, ext, _MAX_PATH);
                // �h���C�u���ƃf�B���N�g����������
                res = ::PathCombine(path, drive, dir);
            }
            return CStringA(res).GetBuffer();
        }
        std::string get_full_path(void) {
            std::string dir = get_dir_path();
            return std::format("{}\\{}", dir, this->_file_relative_path);
        }
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
                // �}�b�`�����������o��
                PLOGD << "tag_name: " << match[1];
                return match[1];
            }
            else {
                PLOGE << "Could not found tag_name.";
                return std::nullopt;
            }
        }
    public:
        Updater(std::string github_id, std::string file_relative_path, std::string old_version)
            : _github_id(github_id), _file_relative_path(file_relative_path), _old_version(old_version)
        {
            _new_version = get_latest_version();
        }

        std::optional<std::string> get_version(void) {
            return this->_new_version;
        }

        bool update(void) {
            PLOGD << "Updating " << this->get_file_name() << "...";
            if (!this->get_version().has_value()) {
                PLOGE << "Version check failed.";
                return false;
            }
            else if (this->get_version().value() == _old_version) {
                PLOGD << this->_file_relative_path << " is the latest.";
                return true;
            }
            else {
                std::string old_path = this->get_full_path() + ".old";
                std::string new_path = this->get_full_path();
                std::filesystem::remove(old_path);
                if (std::filesystem::exists(new_path)) {
                    std::filesystem::rename(new_path, old_path);
                }

                std::string url = std::format("https://github.com/{}/releases/download/{}/{}", this->_github_id, this->get_version().value(), this->get_file_name());

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