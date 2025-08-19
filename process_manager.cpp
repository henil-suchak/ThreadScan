// process_manager.cpp
// CORRECTED: This file now contains the logic to create "word-safe" chunks,
// ensuring that no word is ever split between two threads.

#include "process_manager.hpp"
#include "thread_worker.hpp"
#include "common.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <unistd.h>
#include <sys/wait.h>
#include <pthread.h>
#include <fstream>
#include <cmath>
#include <map>
#include <cctype>

#define NUM_THREADS 4

void launch_processes_for_files(const std::vector<std::string>& files, OperationType operation, const std::vector<std::string>& keywords) {
    std::cout << "\n[🚀] Parent process (PID: " << getpid() << ") is starting..." << std::endl;
    std::vector<pid_t> child_pids;

    for (const auto& file_path : files) {
        pid_t pid = fork();
        if (pid == -1) {
            perror("[❌] fork failed");
            break;
        }

        if (pid == 0) {
            // --- CHILD PROCESS LOGIC ---
            std::cout << "  [👶] Child (PID: " << getpid() << ") is processing file: " << file_path << std::endl;

            std::map<std::string, int> keyword_counts;
            pthread_mutex_t map_mutex;
            if (operation == OperationType::KEYWORD_SEARCH) {
                pthread_mutex_init(&map_mutex, nullptr);
                for (const auto& kw : keywords) {
                    keyword_counts[kw] = 0;
                }
            }

            std::ifstream file_stream(file_path, std::ios::binary | std::ios::ate);
            if (!file_stream) {
                std::cerr << "    [❌] Error: Could not open file " << file_path << "." << std::endl;
                exit(1);
            }
            long file_size = file_stream.tellg();

            if (file_size == 0) {
                 std::cout << "    [📊] Result for " << file_path << ": 0 (file is empty)." << std::endl;
                 exit(0);
            }
            
            std::vector<pthread_t> threads(NUM_THREADS);
            std::vector<ThreadArgs> args_list(NUM_THREADS);
            long total_result = 0;
            long base_chunk_size = file_size / NUM_THREADS;
            long current_pos = 0;

            // --- NEW: Word-Safe Chunking Logic ---
            for (int i = 0; i < NUM_THREADS; ++i) {
                args_list[i].start_offset = current_pos;

                if (i == NUM_THREADS - 1) {
                    // The last thread gets the rest of the file.
                    args_list[i].chunk_size = file_size - current_pos;
                } else {
                    // Find the end of the chunk by looking for a space.
                    long end_pos = current_pos + base_chunk_size;
                    if (end_pos >= file_size) {
                        end_pos = file_size;
                    } else {
                        file_stream.seekg(end_pos);
                        char c;
                        while (file_stream.get(c) && !isspace(c)) {
                            end_pos++; // Keep moving forward until a space is found.
                        }
                    }
                    args_list[i].chunk_size = end_pos - current_pos;
                }
                
                current_pos += args_list[i].chunk_size;
            }
            file_stream.close(); // Close the file stream used for chunking.

            // --- Launch Threads ---
            for (int i = 0; i < NUM_THREADS; ++i) {
                args_list[i].operation = operation;
                args_list[i].file_path = &file_path;
                
                if (operation == OperationType::KEYWORD_SEARCH) {
                    args_list[i].keywords_to_search = &keywords;
                    args_list[i].keyword_counts = &keyword_counts;
                    args_list[i].map_mutex = &map_mutex;
                }
                
                if (args_list[i].chunk_size > 0) {
                    pthread_create(&threads[i], nullptr, process_chunk_dispatcher, &args_list[i]);
                } else {
                    threads[i] = 0;
                }
            }

            // --- Join Threads and Aggregate Results ---
            for (int i = 0; i < NUM_THREADS; ++i) {
                if (threads[i] != 0) {
                    void* thread_result = nullptr;
                    pthread_join(threads[i], &thread_result);
                    if (operation != OperationType::KEYWORD_SEARCH && thread_result) {
                        total_result += *static_cast<long*>(thread_result);
                        delete static_cast<long*>(thread_result);
                    }
                }
            }

            // --- Print Results ---
            if (operation == OperationType::KEYWORD_SEARCH) {
                std::cout << "    [📊] Keyword results for " << file_path << ":\n";
                for (const auto& pair : keyword_counts) {
                    std::cout << "      - " << pair.first << ": " << pair.second << "\n";
                }
                pthread_mutex_destroy(&map_mutex);
            } else {
                const char* result_unit = (operation == OperationType::CHAR_COUNT) ? "characters" : "words";
                std::cout << "    [📊] Result for " << file_path << ": " << total_result << " " << result_unit << "." << std::endl;
            }
            
            exit(0);
        }
        
        child_pids.push_back(pid);
    }

    // --- PARENT waits for all children to finish ---
    std::cout << "[👨] Parent (PID: " << getpid() << ") is waiting for children to finish." << std::endl;
    int status;
    for (pid_t child_pid : child_pids) {
        waitpid(child_pid, &status, 0);
        std::cout << "  [✅] Parent detected that child (PID: " << child_pid << ") has finished." << std::endl;
    }

    std::cout << "\n[🎉] All work is complete!" << std::endl;
}
