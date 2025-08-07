#include "file_utils.hpp"
#include <filesystem>
#include <vector>
#include <string>
#include <iostream>

namespace fs = std::filesystem;

std::vector<std::string> get_files(const std::string& dirpath) {
    std::vector<std::string> files;

    if (!fs::exists(dirpath) || !fs::is_directory(dirpath)) {
        std::cerr << "[❌] Invalid directory: " << dirpath << "\n";
        return files;
    }

    for (const auto& entry : fs::directory_iterator(dirpath)) {
        if (fs::is_regular_file(entry)) {
            files.push_back(entry.path().string());
        }
    }

    return files;
}
