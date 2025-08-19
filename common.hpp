// common.hpp
// Defines shared data structures and operation types.

#ifndef COMMON_HPP
#define COMMON_HPP

#include <string>
#include <vector>
#include <map>
#include <pthread.h> // Required for pthread_mutex_t

// Defines the types of operations our program can perform.
enum class OperationType {
    CHAR_COUNT,
    WORD_COUNT,
    KEYWORD_SEARCH
};

// Holds all the information a single thread needs for any operation.
struct ThreadArgs {
    OperationType operation;
    const std::string* file_path;
    long start_offset;
    long chunk_size;

    // --- Members for keyword search ---
    const std::vector<std::string>* keywords_to_search;
    std::map<std::string, int>* keyword_counts;
    pthread_mutex_t* map_mutex;
};

#endif // COMMON_HPP
