#ifndef PROCESS_MANAGER_HPP
#define PROCESS_MANAGER_HPP

#include <vector>
#include <string>

/**
 * @brief Launches a child process for each file path provided.
 *
 * The parent process will wait for all child processes to complete.
 *
 * @param files A vector of strings, where each string is a path to a file.
 */
void launch_processes_for_files(const std::vector<std::string>& files);

#endif // PROCESS_MANAGER_HPP