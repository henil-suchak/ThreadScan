#ifndef THREAD_WORKER_HPP
#define THREAD_WORKER_HPP

void* process_chunk_dispatcher(void* args);

// These functions now correctly return void, as they update a shared counter
// instead of returning a value.
void count_chars_in_chunk(void* args);
void count_words_in_chunk(void* args);

// This function modifies a shared map, so its return type is also void*.
// We keep it as void* for consistency with the dispatcher, even though it returns nullptr.
void* search_words_in_chunk(void* args);

#endif // THREAD_WORKER_HPP