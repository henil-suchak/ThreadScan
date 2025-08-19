// main.cpp
// Correct version that parses all arguments and calls the correct function.

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <cctype>
#include "file_utils.hpp"
#include "process_manager.hpp"
#include "common.hpp"

void print_usage(const char* prog_name) {
    std::cerr << "Usage: " << prog_name << " <directory_path> --operation <op_name> [options]\n\n";
    std::cerr << "Operations:\n";
    std::cerr << "  char_count\n";
    std::cerr << "  word_count\n";
    std::cerr << "  search --keywords \"word1,word2,word3\"\n";
}

int main(int argc, char* argv[]) {
    if (argc < 4) {
        print_usage(argv[0]);
        return 1;
    }

    std::string dirPath = argv[1];
    std::string op_flag = argv[2];
    std::string op_name = argv[3];
    OperationType operation;
    std::vector<std::string> keywords;

    if (op_flag != "--operation") {
        print_usage(argv[0]);
        return 1;
    }

    if (op_name == "char_count") {
        operation = OperationType::CHAR_COUNT;
    } else if (op_name == "word_count") {
        operation = OperationType::WORD_COUNT;
    } else if (op_name == "search") {
        operation = OperationType::KEYWORD_SEARCH;
        if (argc != 6 || std::string(argv[4]) != "--keywords") {
            std::cerr << "[❌] Error: The 'search' operation requires --keywords \"word1,word2\"\n";
            print_usage(argv[0]);
            return 1;
        }
        std::string keywords_str = argv[5];
        std::stringstream ss(keywords_str);
        std::string keyword;
        while (std::getline(ss, keyword, ',')) {
            std::transform(keyword.begin(), keyword.end(), keyword.begin(),
                           [](unsigned char c){ return std::tolower(c); });
            if (!keyword.empty()) {
                keywords.push_back(keyword);
            }
        }
    } else {
        std::cerr << "[❌] Error: Unknown operation '" << op_name << "'\n\n";
        print_usage(argv[0]);
        return 1;
    }

    std::vector<std::string> files = get_files(dirPath);

    if (files.empty()) {
        std::cout << "No regular files found in the specified directory.\n";
        return 0;
    }

    std::cout << "Files to process:\n";
    for (const std::string& s : files) {
        std::cout << "- " << s << std::endl;
    }
    
    launch_processes_for_files(files, operation, keywords);

    return 0;
}