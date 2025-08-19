#ifndef PROCESS_MANAGER_HPP
#define PROCESS_MANAGER_HPP

#include <vector>
#include <string>
#include "common.hpp"

void launch_processes_for_files(const std::vector<std::string>& files, OperationType operation, const std::vector<std::string>& keywords);

#endif // PROCESS_MANAGER_HPP