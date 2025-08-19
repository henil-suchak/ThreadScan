// process_manager.hpp
// Declares the function that manages child processes.

#ifndef PROCESS_MANAGER_HPP
#define PROCESS_MANAGER_HPP

#include <vector>
#include <string>
#include "common.hpp"

// Correct declaration with all required arguments.
void launch_processes_for_files(const std::vector<std::string>& files, OperationType operation, const std::vector<std::string>& keywords);

#endif // PROCESS_MANAGER_HPP
