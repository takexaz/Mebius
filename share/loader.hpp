#pragma once
#include <debug.hpp>
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
    public:
        Plugins(const std::string dir_path, const std::string extension, bool is_recursive = false) {
            mebius::debug::Logger plog(std::cout, FOREGROUND_LIME);
            mebius::debug::Logger perr(std::cerr, FOREGROUND_PINK);

            std::vector<std::string> plugin_paths = listup_file_paths_in_dir(dir_path, extension, is_recursive);

            for (auto& path : plugin_paths) {
                HMODULE h = LoadLibraryA(path.c_str());
                if (h) {
                    plog << std::format("Loaded Plugin: {}\n", path);
                    _handles.insert(h);
                }
                else {
                    perr << std::format("Failed to load Plugin: {}\n", path);
                }
            }
        }
        ~Plugins() {
            for (auto& handle : _handles) {
                FreeLibrary(handle);
            }
        }
    };
}