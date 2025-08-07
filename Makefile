all:
	clang++ -std=c++17 -pthread main.cpp file_utils.cpp process_manager.cpp -o file_processor

clean:
	rm -f file_processor
