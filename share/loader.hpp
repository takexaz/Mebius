#pragma once
#include <luaaa.hpp>
#include <debug.hpp>

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
                    plog << std::format("Loaded Plugin: {}", path);
                    _handles.insert(h);
                }
                else {
                    perr << std::format("Failed to load Plugin: {}", path);
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

namespace mebius::loader {
    class MEBIUSAPI Scripts
    {
    private:
        std::unordered_set<lua_State*> _states;

    public:
        Scripts(const std::string dir_path, const std::string extension, bool is_recursive = false) {
            mebius::debug::Logger slog(std::cout, FOREGROUND_LIME);
            mebius::debug::Logger serr(std::cerr, FOREGROUND_PINK);

            std::vector<std::string> scripts = listup_file_paths_in_dir(dir_path, extension, is_recursive);

            for (auto& script : scripts) {
                Script lua(script.c_str());
                lua_State* L = lua.get_state();
                if (!L) {
                    slog << std::format("Loaded Script: {}", script);
                    _states.insert(L);
                }
                else {
                    serr << std::format("Failed to load Script: {}", script);
                }
            }
        }
        ~Scripts() {};

        std::unordered_set<lua_State*> get_states() {
            return _states;
        }

        class Script
        {
        private:
            lua_State* _state;
        public:
            Script(const char* path) {
                luaL_openlibs(this->_state);
                if (luaL_loadfile(this->_state, path)) {
                    _state = nullptr;
                }
            }
            ~Script() {
                lua_close(this->_state);
            }
            lua_State* get_state() {
                return this->_state;
            }
        };
    };
}