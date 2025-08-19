#include "thread_worker.hpp"
#include "common.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <cctype>
#include <algorithm>

void* process_chunk_dispatcher(void* args) {
    ThreadArgs* thread_args = static_cast<ThreadArgs*>(args);
    switch (thread_args->operation) {
        case OperationType::CHAR_COUNT:
            count_chars_in_chunk(args);
            break;
        case OperationType::WORD_COUNT:
            count_words_in_chunk(args);
            break;
        case OperationType::KEYWORD_SEARCH:
            search_words_in_chunk(args);
            break;
    }
    return nullptr; // No return value needed
}

void count_chars_in_chunk(void* args) {
    ThreadArgs* thread_args = static_cast<ThreadArgs*>(args);
    long count = thread_args->chunk_size; // Already calculated

    // Safely add this chunk's count to the total
    pthread_mutex_lock(thread_args->count_mutex);
    *(thread_args->total_count) += count;
    pthread_mutex_unlock(thread_args->count_mutex);
}

void count_words_in_chunk(void* args) {
    ThreadArgs* thread_args = static_cast<ThreadArgs*>(args);
    std::ifstream file(*thread_args->file_path);
    if (!file.is_open()) return;

    file.seekg(thread_args->start_offset);
    std::string chunk_str;
    chunk_str.resize(thread_args->chunk_size);
    file.read(&chunk_str[0], thread_args->chunk_size);
    
    std::stringstream ss(chunk_str);
    std::string word;
    long word_count = 0;
    while(ss >> word) {
        word_count++;
    }

    // Safely add this chunk's count to the total
    pthread_mutex_lock(thread_args->count_mutex);
    *(thread_args->total_count) += word_count;
    pthread_mutex_unlock(thread_args->count_mutex);
}

void* search_words_in_chunk(void* args) {
    // This function remains the same as the last correct version.
    ThreadArgs* thread_args = static_cast<ThreadArgs*>(args);
    std::ifstream file(*thread_args->file_path);
    if (!file.is_open()) { return nullptr; }

    file.seekg(thread_args->start_offset);
    std::string chunk_str;
    chunk_str.resize(thread_args->chunk_size);
    file.read(&chunk_str[0], thread_args->chunk_size);
    
    std::stringstream ss(chunk_str);
    std::string word;
    while (ss >> word) {
        std::string normalized_word;
        for (char c : word) {
            if (std::isalnum(static_cast<unsigned char>(c))) {
                normalized_word += std::tolower(static_cast<unsigned char>(c));
            }
        }
        if (normalized_word.empty()) continue;

        auto it = std::find(thread_args->keywords_to_search->begin(), thread_args->keywords_to_search->end(), normalized_word);
        if (it != thread_args->keywords_to_search->end()) {
            pthread_mutex_lock(thread_args->map_mutex);
            (*thread_args->keyword_counts)[*it]++;
            pthread_mutex_unlock(thread_args->map_mutex);
        }
    }
    return nullptr;
}