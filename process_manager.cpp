#include "process_manager.hpp"
#include "thread_pool.hpp"
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
#include <thread>

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

            const unsigned int num_threads = std::thread::hardware_concurrency();
            ThreadPool pool(num_threads);

            // --- Setup Shared Resources ---
            std::map<std::string, int> keyword_counts;
            pthread_mutex_t map_mutex;
            long total_count = 0;
            pthread_mutex_t count_mutex;

            if (operation == OperationType::KEYWORD_SEARCH) {
                pthread_mutex_init(&map_mutex, nullptr);
                for (const auto& kw : keywords) keyword_counts[kw] = 0;
            } else {
                pthread_mutex_init(&count_mutex, nullptr);
            }

            std::ifstream file_stream(file_path, std::ios::binary | std::ios::ate);
            if (!file_stream) { 
                std::cerr << "    [❌] Error: Could not open file " << file_path << "." << std::endl;
                exit(1);
            }
            long file_size = file_stream.tellg();

            if (file_size > 0) {
                // --- CORRECTED: Use different chunking strategies based on operation ---
                if (operation == OperationType::CHAR_COUNT) {
                    // For char_count, use simple, fixed-size chunks for accuracy.
                    long chunk_size = std::ceil(static_cast<double>(file_size) / num_threads);
                    for (unsigned int i = 0; i < num_threads; ++i) {
                        ThreadArgs args;
                        args.start_offset = i * chunk_size;
                        args.chunk_size = chunk_size;
                        if (args.start_offset >= file_size) continue;

                        args.operation = operation;
                        args.file_path = &file_path;
                        args.total_count = &total_count;
                        args.count_mutex = &count_mutex;
                        pool.add_task(args);
                    }
                } else {
                    // For word_count and search, use the "word-safe" chunking logic.
                    long base_chunk_size = file_size / num_threads;
                    long current_pos = 0;
                    for (unsigned int i = 0; i < num_threads; ++i) {
                        ThreadArgs args;
                        args.start_offset = current_pos;
                        
                        long end_pos = current_pos + base_chunk_size;
                        if (i < num_threads - 1 && end_pos < file_size) {
                            file_stream.seekg(end_pos);
                            char c;
                            while(file_stream.get(c) && !isspace(c));
                            end_pos = file_stream.tellg();
                        } else {
                            end_pos = file_size;
                        }
                        args.chunk_size = end_pos - current_pos;
                        current_pos = end_pos;

                        if (args.chunk_size <= 0) continue;

                        args.operation = operation;
                        args.file_path = &file_path;
                        if (operation == OperationType::KEYWORD_SEARCH) {
                            args.keywords_to_search = &keywords;
                            args.keyword_counts = &keyword_counts;
                            args.map_mutex = &map_mutex;
                        } else {
                            args.total_count = &total_count;
                            args.count_mutex = &count_mutex;
                        }
                        pool.add_task(args);
                    }
                }
            }
            file_stream.close();

            pool.wait_for_completion();

            // --- Print Results ---
            if (operation == OperationType::KEYWORD_SEARCH) {
                std::cout << "    [📊] Keyword results for " << file_path << ":\n";
                for (const auto& pair : keyword_counts) {
                    std::cout << "      - " << pair.first << ": " << pair.second << "\n";
                }
                pthread_mutex_destroy(&map_mutex);
            } else {
                // For char_count, the final result is simply the file size.
                if (operation == OperationType::CHAR_COUNT) {
                    total_count = file_size;
                }
                const char* result_unit = (operation == OperationType::CHAR_COUNT) ? "characters" : "words";
                std::cout << "    [📊] Result for " << file_path << ": " << total_count << " " << result_unit << "." << std::endl;
                pthread_mutex_destroy(&count_mutex);
            }
            
            exit(0);
        }
        
        child_pids.push_back(pid);
    }

    // --- PARENT waiting logic remains the same ---
    std::cout << "[👨] Parent (PID: " << getpid() << ") is waiting for children to finish." << std::endl;
    int status;
    for (pid_t child_pid : child_pids) {
        waitpid(child_pid, &status, 0);
        std::cout << "  [✅] Parent detected that child (PID: " << child_pid << ") has finished." << std::endl;
    }
    std::cout << "\n[🎉] All work is complete!" << std::endl;
}