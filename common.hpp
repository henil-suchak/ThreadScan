#ifndef COMMON_HPP
#define COMMON_HPP

#include <string>
#include <vector>
#include <map>
#include <pthread.h>

enum class OperationType {
    CHAR_COUNT,
    WORD_COUNT,
    KEYWORD_SEARCH
};

struct ThreadArgs {
    OperationType operation;
    const std::string* file_path;
    long start_offset;
    long chunk_size;

    // --- For keyword search ---
    const std::vector<std::string>* keywords_to_search;
    std::map<std::string, int>* keyword_counts;
    pthread_mutex_t* map_mutex;

    // --- For char_count and word_count ---
    long* total_count;          // Pointer to the shared total count
    pthread_mutex_t* count_mutex; // Mutex to protect the shared total count
};

#endif // COMMON_HPP