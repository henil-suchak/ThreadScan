#include <iostream>
#include <filesystem>
#include <vector>
#include<string>
#include "file_utils.hpp"  
#include "process_manager.hpp"
using namespace std;

int main(int argc,char* argv[]) {
    if(argc<2){
        std::cerr << "Usage: " << argv[0] << " <directory_path>\n";
        return 1;
    }
    string dirPath=argv[1];
    vector<string> files=get_files(dirPath);

    if(files.empty()){
        cout<<"no files found\n";
    }else{
        cout<<"files to process\n";
        for(string s:files){
            cout<<s<<endl;
        }
    }

    // --- Hand off the file list to the process manager ---
    launch_processes_for_files(files);

    

    return 0;
}
