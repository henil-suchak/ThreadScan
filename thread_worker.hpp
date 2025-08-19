// thread_worker.hpp
// Declares all available thread worker functions.

#ifndef THREAD_WORKER_HPP
#define THREAD_WORKER_HPP

// Main entry point for all threads.
void* process_chunk_dispatcher(void* args);

// Specific worker functions.
void* count_chars_in_chunk(void* args);
void* count_words_in_chunk(void* args);
void* search_words_in_chunk(void* args);

#endif // THREAD_WORKER_HPP
