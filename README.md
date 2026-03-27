 **ThreadScan** is a high-performance, concurrent file analysis engine. It uses a hybrid concurrency model: it spawns a separate child process (using `fork`) for each file in a directory, and within each process, it utilizes a custom **Thread Pool** to divide the file into chunks and process them in parallel. 

Here is a breakdown of the three main operations this engine can perform:

1. **Character Count (`char_count`)**:
   * **What it does:** Calculates the total number of characters (bytes) in the files.
   * **How it works:** It divides the file size into mathematically equal chunks based on the hardware concurrency (number of available CPU cores) and assigns each chunk to a thread. 

2. **Word Count (`word_count`)**:
   * **What it does:** Counts the total number of words across the files.
   * **How it works:** It divides the file into chunks but uses a "word-safe" boundary checking algorithm. If a chunk boundary falls in the middle of a word, it dynamically extends the chunk to the next whitespace to prevent a single word from being counted twice or split across two threads.

3. **Keyword Search (`search`)**:
   * **What it does:** Scans the files for specific, user-defined keywords and returns the frequency of each keyword.
   * **How it works:** It uses the same word-safe chunking as the word counter. Threads normalize the text (converting to lowercase and stripping punctuation) and safely update a shared frequency map using mutex locks to prevent race conditions. 

---

Here is a complete, formatted `README.md` file you can use for your repository:

```markdown
# ThreadScan

ThreadScan is a high-performance, concurrent file analysis engine written in C++. It is designed to quickly process multiple files in a directory by leveraging both multi-processing and multi-threading. 

## 🚀 Architecture
- **Multi-processing:** The parent process uses `fork()` to spawn a dedicated child process for every file in the target directory.
- **Multi-threading:** Within each child process, a Thread Pool is initialized based on the system's hardware concurrency. The file is divided into chunks, and worker threads process these chunks in parallel.
- **Safe Chunking:** For word counting and searching, the engine dynamically adjusts chunk boundaries to ensure words are not split across different threads.

## 🛠️ Build Instructions

The project uses a standard Makefile. It requires a compiler that supports C++17 and the `pthread` library (the default compiler in the Makefile is `clang++`).

To build the project, simply run:
```bash
make
```
This will compile the source files and generate an executable named `file_processor`.

To clean up the compiled binaries, run:
```bash
make clean
```

## 💻 Usage

**Base Command Syntax:**
```bash
./file_processor <directory_path> --operation <op_name> [options]
```

### 1. Character Count
Counts the total number of characters in each file within the specified directory.
```bash
./file_processor ./my_directory --operation char_count
```

### 2. Word Count
Calculates the total number of words in each file concurrently.
```bash
./file_processor ./my_directory --operation word_count
```

### 3. Keyword Search
Searches for specific keywords and returns their occurrence count in each file. You must provide a comma-separated list of words (no spaces between words) using the `--keywords` flag. The search is case-insensitive.
```bash
./file_processor ./my_directory --operation search --keywords "error,warning,critical"
```

## 📂 Example Output
```text
Files to process:
- ./logs/server.log
- ./logs/system.log

[🚀] Parent process (PID: 1045) is starting...
  [👶] Child (PID: 1046) is processing file: ./logs/server.log
  [👶] Child (PID: 1047) is processing file: ./logs/system.log
[👨] Parent (PID: 1045) is waiting for children to finish.
    [📊] Keyword results for ./logs/server.log:
      - error: 12
      - warning: 45
      - critical: 1
  [✅] Parent detected that child (PID: 1046) has finished.
    [📊] Keyword results for ./logs/system.log:
      - error: 3
      - warning: 10
      - critical: 0
  [✅] Parent detected that child (PID: 1047) has finished.

[🎉] All work is complete!
```
```
