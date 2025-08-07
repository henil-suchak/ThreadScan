#include "process_manager.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <unistd.h>    // Required for fork(), getpid(), getppid()
#include <sys/wait.h>  // Required for waitpid()

void launch_processes_for_files(const std::vector<std::string>& files) {
    std::cout << "\n[🚀] Parent process (PID: " << getpid() << ") is starting..." << std::endl;
    std::vector<pid_t> child_pids;

    // Loop through each file and fork a new process for it
    for (const auto& file_path : files) {
        pid_t pid = fork();

        if (pid == -1) {
            // Fork failed; handle the error
            perror("[❌] fork failed");
            break; 
        } 
        
        if (pid == 0) {
            // --- This block is executed ONLY by the CHILD process ---
            // highlight-start
            std::cout << "  [👶] Child (PID: " << getpid() << ", Parent PID: " << getppid() << ") is processing file: " << file_path << std::endl;
            // highlight-end
            
            // In the future, the file processing logic will go here.
            
            exit(0); // IMPORTANT: The child process must exit here.
        } 
        
        // --- This block is executed ONLY by the PARENT process ---
        child_pids.push_back(pid); // Store the child's PID
    }

    // --- Parent waits for all children to finish ---
    std::cout << "[👨] Parent (PID: " << getpid() << ") is waiting for children to finish." << std::endl;
    int status;
    for (pid_t child_pid : child_pids) {
        waitpid(child_pid, &status, 0); // Wait for a specific child to terminate
        std::cout << "  [✅] Parent detected that child (PID: " << child_pid << ") has finished." << std::endl;
    }

    std::cout << "\n[🎉] All work is complete!" << std::endl;
}