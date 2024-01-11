#include <loader.hpp>
#include <filesystem>

namespace mebius::loader {
	std::vector<std::string> listup_file_paths_in_dir(const std::string dir_path, const std::string extension, bool is_recursive) {
		std::vector<std::string> paths;

        for (const auto& entry : std::filesystem::directory_iterator(dir_path)) {
            if (entry.is_directory() && is_recursive) {
                // �ċA�I�ɃT�u�f�B���N�g����T��
                auto sub_dir = listup_file_paths_in_dir(entry.path().string(), extension);
                paths.insert(paths.end(), sub_dir.begin(), sub_dir.end());
            }
            else if (entry.is_regular_file() && entry.path().extension() == "." + extension) {
                // �t�@�C���̃p�X�����X�g�ɒǉ�
                paths.push_back(entry.path().string());
            }
        }
		return paths;
	}
    std::vector<Plugins*> Plugins::_plugins;
}