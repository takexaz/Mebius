#pragma once
#include <debug.hpp>
#include <Windows.h>
#include <unordered_set>

#ifdef MEBIUS_EXPORT
#define MEBIUSAPI __declspec(dllexport)
#else
#define MEBIUSAPI __declspec(dllimport)
#endif

namespace mebius::loader {

    std::vector<std::string> listup_file_paths_in_dir(const std::string directoryPath, const std::string extension, bool is_recursive = false);

    class MEBIUSAPI Plugins
    {
    private:
        std::unordered_set<HMODULE> _handles;
        static std::vector<Plugins*> _plugins;
        Plugins(const std::string dir_path, const std::string extension, bool is_recursive = false) {
            std::vector<std::string> plugin_paths = listup_file_paths_in_dir(dir_path, extension, is_recursive);

            for (auto& path : plugin_paths) {
                HMODULE h = LoadLibraryA(path.c_str());
                if (h) {
                    PLOGD << "Loaded Plugin: " << path;
                    _handles.insert(h);
                }
                else {
                    PLOGE << "Failed to load Plugin: " << path;
                }
            }
        }
    public:
        static Plugins* create(const std::string dir_path, const std::string extension, bool is_recursive = false) {
            Plugins* p = new Plugins(dir_path, extension, is_recursive);
            _plugins.push_back(p);
            return p;
        }
        ~Plugins() {
            for (auto& handle : _handles) {
                FreeLibrary(handle);
            }
        }
    };
}